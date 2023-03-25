#ifndef DAMAGE_SYSTEM_H
#define DAMAGE_SYSTEM_H

#include "../ECS/ECS.h"
#include "../EventBus/EventBus.h"
#include "../Events/CollisionEvent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/ProjectileComponent.h"
#include "../Components/HealthComponent.h"

class DamageSystem: public System {
private:
	void onProjectileHitsPlayer(Entity projectile, Entity player) {
		auto projectileComponent = projectile.GetComponent<ProjectileComponent>();

		if (!projectileComponent.isFriendly) {
			handleEntityProjectileHit(projectile, projectileComponent, player);
		}
	}

	void onProjectileHitsEnemy(Entity projectile, Entity enemy) {
		auto projectileComponent = projectile.GetComponent<ProjectileComponent>();

		if (projectileComponent.isFriendly) {
			handleEntityProjectileHit(projectile, projectileComponent, enemy);
		}
	}

	void handleEntityProjectileHit(Entity projectile, ProjectileComponent projectileComponent, Entity entity) {
			auto &health = entity.GetComponent<HealthComponent>();
			health.healthPercentage -= projectileComponent.hitPercentDamage;

			if (health.healthPercentage <= 0) {
				entity.Kill();
			}

			projectile.Kill();
	}

public:
	DamageSystem() {
		RequireComponent<BoxColliderComponent>();
	}

	void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
		eventBus->SubscribeToEvent<CollisionEvent>(this, &DamageSystem::OnCollision);
	}

	void OnCollision(CollisionEvent& event) {
		Entity a = event.a;
		Entity b = event.b;

		Logger::Info("The damage system received event: "
			   + std::to_string(a.GetId()) + " and " + std::to_string(b.GetId()));

		if (a.InGroup("projectiles") && b.HasTag("player")) {
			onProjectileHitsPlayer(a, b);
		}
		if (b.InGroup("projectiles") && a.HasTag("player")) {
			onProjectileHitsPlayer(b, a);
		}
		if (a.InGroup("projectiles") && b.InGroup("enemies")) {
			onProjectileHitsEnemy(a, b);
		}
		if (b.InGroup("projectiles") && a.InGroup("enemies")) {
			onProjectileHitsEnemy(b, a);
		}
	}
};

#endif // DAMAGE_SYSTEM_H
