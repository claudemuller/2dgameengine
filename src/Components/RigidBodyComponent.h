#ifndef RIGID_BODY_COMPONENT_H
#define RIGID_BODY_COMPONENT_H

#include <glm/glm.hpp>

struct RigidBodyComponent {
	glm::vec2 velocity;

	RigidBodyComponent(glm::vec2 velocity = glm::vec2(0.0, 0.0)) {
		this->velocity = velocity;
	}
};

#endif // RIGID_BODY_COMPONENT_H
