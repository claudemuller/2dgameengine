#ifndef SPRITE_COMPONENT_H
#define SPRITE_COMPONENT_H

#include <string>
#include <SDL2/SDL.h>

struct SpriteComponent {
	std::string assetId;
	int width;
	int height;
	int zIndex;
	bool isFixed;
	SDL_RendererFlip isFlipped;
	SDL_Rect srcRect;

	SpriteComponent(
		std::string assetId = "",
		int width = 0,
		int height = 0,
		int zIndex = 0,
		bool isFixed = false,
		int srcRectX = 0,
		int srcRectY = 0,
		SDL_RendererFlip isFlipped = SDL_FLIP_NONE
	) {
		this->assetId = assetId;
		this->width = width;
		this->height = height;
		this->zIndex = zIndex;
		this->isFixed = isFixed;
		this->srcRect = {srcRectX, srcRectY, width, height};
		this->isFlipped = isFlipped;
	}
};

#endif // SPRITE_COMPONENT_H

