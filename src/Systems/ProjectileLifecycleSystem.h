#ifndef PROJECTILE_LIFECYCLE_SYSTEM_H
#define PROJECTILE_LIFECYCLE_SYSTEM_H

#include "../ECS/ECS.h"
#include "../Components/ProjectileComponent.h"

class ProjectileLifecycleSystem: public System {
public:
	ProjectileLifecycleSystem() {
		RequireComponent<ProjectileComponent>();
	}

	void Update() {
		for (auto entity: GetSystemEntities()) {
			auto projectile = entity.GetComponent<ProjectileComponent>();

			if (SDL_GetTicks() - projectile.startTime > projectile.duration) {
				entity.Kill();
			}
		}
	}
};

#endif // PROJECTILE_LIFECYCLE_SYSTEM_H
