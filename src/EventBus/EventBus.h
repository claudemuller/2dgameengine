#ifndef EVENTBUS_H
#define EVENTBUS_H

#include <map>
#include <typeindex>
#include <list>
#include <functional>
#include "../Logger/Logger.h"
#include "./Event.h"

class BaseEventCallback {
private:
	virtual void call(Event &e) = 0;

public:
	virtual ~BaseEventCallback() = default;

	void Execute(Event &e) {
		call(e);
	}
};

template <typename TOwner, typename TEvent>
class EventCallback: public BaseEventCallback {
private:
	typedef void (TOwner::*CallbackFunc)(TEvent&);

	TOwner *ownerInstance;
	CallbackFunc callbackFunc;

	virtual void call(Event &e) override {
		std::invoke(callbackFunc, ownerInstance, static_cast<TEvent&>(e));
	}

public:
	EventCallback(TOwner *ownerInstance, CallbackFunc callbackFunc) {
		this->ownerInstance = ownerInstance;
		this->callbackFunc = callbackFunc;
	}
	~EventCallback() override = default;
};

typedef std::list<std::unique_ptr<BaseEventCallback>> HandlerList;

class EventBus {
private:
	std::map<std::type_index, std::unique_ptr<HandlerList>> subscribers;

public:
	EventBus() {
		Logger::Info("EventBus constructed");
	}
	~EventBus() {
		Logger::Info("EventBus destroyed");
	}

	void Reset() {
		subscribers.clear();
	}

	template <typename TEvent, typename TOwner>
	void SubscribeToEvent(TOwner *ownerInstance, void (TOwner::*callbackFunc)(TEvent&)) {
		if (!subscribers[typeid(TEvent)].get()) {
			subscribers[typeid(TEvent)] = std::make_unique<HandlerList>();
		}
		auto subscriber = std::make_unique<EventCallback<TOwner, TEvent>>(ownerInstance, callbackFunc);
		subscribers[typeid(TEvent)]->push_back(std::move(subscriber));
	}

	template <typename TEvent, typename ...TArgs>
	void EmitEvent(TArgs&& ...args) {
		auto handlers = subscribers[typeid(TEvent)].get();
		if (handlers) {
			for (auto it = handlers->begin(); it != handlers->end(); it++) {
				auto handler = it->get();
				TEvent event(std::forward<TArgs>(args)...);
				handler->Execute(event);
			}
		}
	}
};

#endif // EVENTBUS_H
