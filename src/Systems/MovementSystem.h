#ifndef MOVEMENT_SYSTEM_H
#define MOVEMENT_SYSTEM_H

#include "../Game/Game.h"
#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Events/CollisionEvent.h"

class MovementSystem : public System {
public:
	MovementSystem() {
		RequireComponent<TransformComponent>();
		RequireComponent<RigidBodyComponent>();
	}

	void SubscribeToEvents(const std::unique_ptr<EventBus>& eventBus) {
		eventBus->SubscribeToEvent<CollisionEvent>(this, &MovementSystem::OnCollision);
	}

	void OnCollision(CollisionEvent& event) {
		Entity a = event.a;
		Entity b = event.b;

		if (a.InGroup(Game::Groups[Game::ENEMIES]) && b.InGroup(Game::Groups[Game::WORLD])) {
			HandleEnemyHitsWorldItem(a, b);
		}
		if (a.HasTag(Game::Groups[Game::WORLD]) && b.HasTag(Game::Groups[Game::ENEMIES])) {
			HandleEnemyHitsWorldItem(b, a);
		}
	}

	void HandleEnemyHitsWorldItem(Entity enemy, Entity obstacle) {
		if (!enemy.HasComponent<RigidBodyComponent>()) {
			return;
		}

		Logger::Info("collision: turn around");

		auto &rigidBody = enemy.GetComponent<RigidBodyComponent>();
		auto &sprite = enemy.GetComponent<SpriteComponent>();
		if (rigidBody.velocity.x != 0) {
			rigidBody.velocity.x *= -1;
			sprite.isFlipped = sprite.isFlipped == SDL_FLIP_NONE ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
		}
		if (rigidBody.velocity.y != 0) {
			rigidBody.velocity.y *= -1;
		}
	}

	void Update(const double deltaTime) {
		for (auto entity: GetSystemEntities()) {
			auto &transform = entity.GetComponent<TransformComponent>();
			const auto &rigidBody = entity.GetComponent<RigidBodyComponent>();

			transform.position.x += rigidBody.velocity.x * deltaTime;
			transform.position.y += rigidBody.velocity.y * deltaTime;

			bool isEntityOutsideMap = transform.position.x < 0 || transform.position.x > Game::MapWidth
				|| transform.position.y < 0 || transform.position.y > Game::MapHeight;

			if (isEntityOutsideMap && !entity.HasTag("player")) {
				entity.Kill();
			}
		}
	}
};

#endif // MOVEMENT_SYSTEM_H
