#ifndef TEXT_LABEL_COMPONENT_H
#define TEXT_LABEL_COMPONENT_H

#include <string>
#include <glm/glm.hpp>
#include <SDL2/SDL.h>

struct TextLabelComponent {
	glm::vec2 position;
	std::string text;
	std::string assetId;
	SDL_Color color;
	bool isFixed;

	TextLabelComponent(
		glm::vec2 position = glm::vec2(0),
		std::string text = "",
		std::string assetId = "",
		SDL_Color color = {0, 0, 0},
		bool isFixed = true
	) {
		this->position = position;
		this->text = text,
		this->assetId = assetId,
		this->color = color;
		this->isFixed = isFixed;
	}
};

#endif // TEXT_LABEL_COMPONENT_H
