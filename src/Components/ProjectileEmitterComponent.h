#ifndef PROJECTILE_EMITTER_COMPONENT_H
#define PROJECTILE_EMITTER_COMPONENT_H

#include <SDL2/SDL.h>
#include <glm/glm.hpp>

struct ProjectileEmitterComponent {
	glm::vec2 projectileVelocity;
	int repeatFreq;
	int projectileDuration;
	int hitPercentDamage;
	bool isFriendly;
	int lastEmissionTime;

	ProjectileEmitterComponent(
		glm::vec2 projectileVelocity = glm::vec2(0),
		int repeatFreq = 0,
		int projectileDuration = 10000,
		int hitPercentDamage = 10,
		bool isFriendly = false
	) {
		this->projectileVelocity = projectileVelocity;
		this->repeatFreq = repeatFreq;
		this->projectileDuration = projectileDuration;
		this->hitPercentDamage = hitPercentDamage;
		this->isFriendly = isFriendly;
		this->lastEmissionTime = SDL_GetTicks();
	}
};

#endif // PROJECTILE_EMITTER_COMPONENT_H
