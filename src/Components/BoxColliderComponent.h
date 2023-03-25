#ifndef BOX_COLLIDER_COMPONENT_H
#define BOX_COLLIDER_COMPONENT_H

#include <glm/glm.hpp>
#include <SDL2/SDL.h>

struct BoxColliderComponent {
	int width;
	int height;
	glm::vec2 offset;
	SDL_Color colour;

	BoxColliderComponent(int width = 0, int height = 0, glm::vec2 offset = glm::vec2(0)) {
		this->width = width;
		this->height = height;
		this->offset = offset;
		this->colour = {255, 255, 0, 255};
	}
};

#endif // BOX_COLLIDER_COMPONENT_H

