#ifndef SPRITE_COMPONENT_H
#define SPRITE_COMPONENT_H

struct SpriteComponent {
	int width;
	int height;

	SpriteComponent(int width = 0, int height = 0) {
		this->width = width;
		this->height = height;
	}
};

#endif // SPRITE_COMPONENT_H

