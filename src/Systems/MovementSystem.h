#ifndef MOVEMENT_SYSTEM_H
#define MOVEMENT_SYSTEM_H

#include "System.h"

class MovementSystem : public System {
public:
	MovementSystem() {

	}

	void Update() {
		for (auto entity: GetEntities()) {

		}
	}
};

#endif // MOVEMENT_SYSTEM_H
