#ifndef RENDER_COLLIDER_SYSTEM_H
#define RENDER_COLLIDER_SYSTEM_H

#include <SDL2/SDL.h>
#include "../ECS/ECS.h"
#include "../EventBus/EventBus.h"
#include "../Events/CollisionEvent.h"
#include "../Components/TransformComponent.h"
#include "../Components/BoxColliderComponent.h"

class RenderColliderSystem : public System {
public:
	RenderColliderSystem() {
		RequireComponent<TransformComponent>();
		RequireComponent<BoxColliderComponent>();
	}

	void SubscribeToEvents(std::unique_ptr<EventBus> &eventBus) {
		eventBus->SubscribeToEvent<CollisionEvent>(this, &RenderColliderSystem::onCollision);
	}

	void onCollision(CollisionEvent &event) {
		event.a.GetComponent<BoxColliderComponent>().colour = {255, 0, 0, 255};
		event.b.GetComponent<BoxColliderComponent>().colour = {255, 0, 0, 255};
	}

	void Update(SDL_Renderer *renderer, SDL_Rect camera) {
		for (auto entity: GetSystemEntities()) {
			const auto transform = entity.GetComponent<TransformComponent>();
			auto &collider = entity.GetComponent<BoxColliderComponent>();

			SDL_Rect colliderRect = {
				static_cast<int>(transform.position.x + collider.offset.x - camera.x),
				static_cast<int>(transform.position.y + collider.offset.y - camera.y),
				static_cast<int>(collider.width * transform.scale.x),
				static_cast<int>(collider.height * transform.scale.y),
			};
			SDL_SetRenderDrawColor(renderer, collider.colour.r, collider.colour.g, collider.colour.b, collider.colour.a);
			SDL_RenderDrawRect(renderer, &colliderRect);

			// TODO: fix this :(
			// collider.colour = {255, 255, 0, 255};
		}
	}
};

#endif // RENDER_COLLIDER_SYSTEM_H
