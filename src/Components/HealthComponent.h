#ifndef HEALTH_COMPONENT_H
#define HEALTH_COMPONENT_H

#include <string>
#include <SDL2/SDL.h>

struct HealthComponent {
	int healthPercentage;

	HealthComponent(int healthPercentage = 0) {
		this->healthPercentage = healthPercentage;
	}
};

#endif // HEALTH_COMPONENT_H
