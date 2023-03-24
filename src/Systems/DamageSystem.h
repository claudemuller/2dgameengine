#ifndef DAMAGE_SYSTEM_H
#define DAMAGE_SYSTEM_H

#include "../ECS/ECS.h"
#include "../EventBus/EventBus.h"
#include "../Events/CollisionEvent.h"
#include "../Components/BoxColliderComponent.h"

class DamageSystem: public System {
public:
	DamageSystem() {
		RequireComponent<BoxColliderComponent>();
	}

	void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
		eventBus->SubscribeToEvent<CollisionEvent>(this, &DamageSystem::onCollision);
	}

	void onCollision(CollisionEvent& event) {
		Logger::Info("The damage system received event: "
			   + std::to_string(event.a.GetId()) + " and " + std::to_string(event.b.GetId()));
		event.a.Kill();
		event.b.Kill();
	}

	void Update() {

	}
};

#endif // DAMAGE_SYSTEM_H
