#ifndef RENDER_COLLIDER_SYSTEM_H
#define RENDER_COLLIDER_SYSTEM_H

#include <SDL2/SDL.h>
#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/BoxColliderComponent.h"

class RenderColliderSystem : public System {
public:
	RenderColliderSystem() {
		RequireComponent<TransformComponent>();
		RequireComponent<BoxColliderComponent>();
	}

	void Update(SDL_Renderer *renderer) {
		for (auto entity: GetSystemEntities()) {
			const auto transform = entity.GetComponent<TransformComponent>();
			const auto collider = entity.GetComponent<BoxColliderComponent>();

			SDL_Rect colliderRect = {
				static_cast<int>(transform.position.x + collider.offset.x),
				static_cast<int>(transform.position.y + collider.offset.y),
				static_cast<int>(collider.width),
				static_cast<int>(collider.height),
			};
			SDL_SetRenderDrawColor(renderer, collider.colour.r, collider.colour.g, collider.colour.b, collider.colour.a);
			SDL_RenderDrawRect(renderer, &colliderRect);
		}
	}
};

#endif // RENDER_COLLIDER_SYSTEM_H
