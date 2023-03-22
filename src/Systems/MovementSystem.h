#ifndef MOVEMENT_SYSTEM_H
#define MOVEMENT_SYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Logger/Logger.h"

class MovementSystem : public System {
public:
	MovementSystem() {
		RequireComponent<TransformComponent>();
		RequireComponent<RigidBodyComponent>();
	}

	void Update() {
		for (auto entity: GetSystemEntities()) {
			auto &transform = entity.GetComponent<TransformComponent>();
			const auto &rigidBody = entity.GetComponent<RigidBodyComponent>();

			transform.position.x += rigidBody.velocity.x;
			transform.position.y += rigidBody.velocity.y;

			Logger::Info("Entity id = " + std::to_string(entity.GetId()) + " position is now ("
				+ std::to_string(transform.position.x) + ", " + std::to_string(transform.position.y) + ")");
		}
	}
};

#endif // MOVEMENT_SYSTEM_H
