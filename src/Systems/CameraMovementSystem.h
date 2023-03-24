#ifndef CAMERA_MOVEMENT_SYSTEM_H
#define CAMERA_MOVEMENT_SYSTEM_H

#include <SDL2/SDL.h>
#include "../Game/Game.h"
#include "../ECS/ECS.h"
#include "../Components/CameraFollowComponent.h"
#include "../Components/TransformComponent.h"

class CameraMovementSystem : public System {
public:
	CameraMovementSystem() {
		RequireComponent<CameraFollowComponent>();
		RequireComponent<TransformComponent>();
	}

	void Update(SDL_Rect &camera) {
		for (auto entity: GetSystemEntities()) {
			auto &transform = entity.GetComponent<TransformComponent>();

			if (transform.position.x + (camera.w / 2) < Game::MapWidth) {
				camera.x = transform.position.x - (Game::WindowWidth / 2);
			}
			if (transform.position.y + (camera.h / 2) < Game::MapHeight) {
				camera.y = transform.position.y - (Game::WindowHeight / 2);
			}

			camera.x = camera.x < 0 ? 0 : camera.x;
			camera.y = camera.y < 0 ? 0 : camera.y;
			camera.x = camera.x > camera.w ? camera.w : camera.x;
			camera.y = camera.y > camera.h ? camera.h : camera.y;
		}
	}
};

#endif // CAMERA_MOVEMENT_SYSTEM_H

