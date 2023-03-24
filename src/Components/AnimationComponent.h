#ifndef ANIMATION_COMPONENT_H
#define ANIMATION_COMPONENT_H

#include <SDL2/SDL.h>

struct AnimationComponent {
	int numFrames;
	int currentFrame;
	int frameRateSpeed;
	bool isLoop;
	int startTime;

	AnimationComponent(int numFrames = 1, int frameRateSpeed = 1, bool isLoop = true) {
		this->numFrames = numFrames;
		this->currentFrame = 1;
		this->frameRateSpeed = frameRateSpeed;
		this->isLoop = isLoop;
		this->startTime = SDL_GetTicks();
	}
};

#endif // ANIMATION_COMPONENT_H
