#ifndef KEYBOARD_MOVEMENT_SYSTEM_H
#define KEYBOARD_MOVEMENT_SYSTEM_H

#include "../ECS/ECS.h"
#include "../EventBus/EventBus.h"
#include "../Events/KeyPressedEvent.h"

class KeyboardMovementSystem: public System {
public:
	KeyboardMovementSystem() {
	}

	void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
		eventBus->SubscribeToEvent<KeyPressedEvent>(this, &KeyboardMovementSystem::OnKeyPressed);
	}

	void OnKeyPressed(KeyPressedEvent& event) {
		std::string keyCode = std::to_string(event.symbol);
		std::string keySymbol(1, event.symbol);
		Logger::Info("The key " + keySymbol + "[" + keyCode + "]" + " was pressed.");
	}

	void Update() {

	}
};

#endif // KEYBOARD_MOVEMENT_SYSTEM_H
