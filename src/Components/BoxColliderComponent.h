#ifndef BOX_COLLIDER_COMPONENT_H
#define BOX_COLLIDER_COMPONENT_H

#include <glm/glm.hpp>

typedef struct {
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
} Colour;

struct BoxColliderComponent {
	int width;
	int height;
	glm::vec2 offset;
	Colour colour;

	BoxColliderComponent(int width = 0, int height = 0, glm::vec2 offset = glm::vec2(0)) {
		this->width = width;
		this->height = height;
		this->offset = offset;
		this->colour = {255, 255, 0, 255};
	}
};

#endif // BOX_COLLIDER_COMPONENT_H

