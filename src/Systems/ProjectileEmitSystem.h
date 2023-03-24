#ifndef PROJECTILE_EMIT_SYSTEM_H
#define PROJECTILE_EMIT_SYSTEM_H

#include <SDL2/SDL.h>
#include "../ECS/ECS.h"
#include "../EventBus/EventBus.h"
#include "../Events/KeyPressedEvent.h"
#include "../Components/ProjectileEmitterComponent.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/ProjectileComponent.h"
#include "../Components/CameraFollowComponent.h"

class ProjectileEmitSystem : public System {
private:
	void emitProjectile(
		Entity entity,
		glm::vec2 projectilePos,
		glm::vec2 projectileVel,
		ProjectileEmitterComponent projectileEmitter
	) {
		Entity projectile = entity.entityManager->CreatEntity();
		projectile.AddComponent<TransformComponent>(projectilePos, glm::vec2(1.0, 1.0), 0.0);
		projectile.AddComponent<RigidBodyComponent>(projectileVel);
		projectile.AddComponent<SpriteComponent>("bullet-image", 4, 4, 4);
		projectile.AddComponent<BoxColliderComponent>(4, 4);
		projectile.AddComponent<ProjectileComponent>(projectileEmitter.isFriendly, projectileEmitter.hitPercentDamage, projectileEmitter.projectileDuration);
	}

public:
	ProjectileEmitSystem() {
		RequireComponent<ProjectileEmitterComponent>();
		RequireComponent<TransformComponent>();
	}

	void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
		eventBus->SubscribeToEvent<KeyPressedEvent>(this, &ProjectileEmitSystem::OnKeyPressed);
	}

	void OnKeyPressed(KeyPressedEvent& event) {
		if (event.symbol == SDLK_SPACE) {
			for (auto entity: GetSystemEntities()) {
				if (entity.HasComponent<CameraFollowComponent>()) {
					const auto projectileEmitter = entity.GetComponent<ProjectileEmitterComponent>();
					const auto transform = entity.GetComponent<TransformComponent>();
					const auto rigidBody = entity.GetComponent<RigidBodyComponent>();

					glm::vec2 projectilePosition = transform.position;
					if (entity.HasComponent<SpriteComponent>()) {
						auto sprite = entity.GetComponent<SpriteComponent>();
						projectilePosition.x += (transform.scale.x * sprite.width / 2);
						projectilePosition.y += (transform.scale.y * sprite.height / 2);
					}

					glm::vec2 projectileVelocity = projectileEmitter.projectileVelocity;
					int directionX = 0;
					int directionY = 0;
					if (rigidBody.velocity.x > 0) directionX = +1;
					if (rigidBody.velocity.x < 0) directionX = -1;
					if (rigidBody.velocity.y > 0) directionY = +1;
					if (rigidBody.velocity.y < 0) directionY = -1;
					projectileVelocity.x = projectileEmitter.projectileVelocity.x * directionX;
					projectileVelocity.y = projectileEmitter.projectileVelocity.y * directionY;

					emitProjectile(entity, projectilePosition, projectileVelocity, projectileEmitter);
				}
			}
		}
	}

	void Update(std::unique_ptr<EntityManager>& entityManager) {
		for (auto entity: GetSystemEntities()) {
			auto &projectileEmitter = entity.GetComponent<ProjectileEmitterComponent>();
			const auto transform = entity.GetComponent<TransformComponent>();

			if (projectileEmitter.repeatFreq == 0) {
				continue;
			}

			if (SDL_GetTicks() - projectileEmitter.lastEmissionTime > projectileEmitter.repeatFreq) {
				glm::vec2 projectilePosition = transform.position;
				if (entity.HasComponent<SpriteComponent>()) {
					auto sprite = entity.GetComponent<SpriteComponent>();
					projectilePosition.x += transform.scale.x * sprite.width / 2;
					projectilePosition.y += transform.scale.y * sprite.height / 2;
				}

				emitProjectile(entity, projectilePosition, projectileEmitter.projectileVelocity, projectileEmitter);

				projectileEmitter.lastEmissionTime = SDL_GetTicks();
			}
		}
	}
};

#endif // PROJECTILE_EMIT_SYSTEM_H
