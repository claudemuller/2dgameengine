#ifndef COLLISION_SYSTEM_H
#define COLLISION_SYSTEM_H

#include "../ECS/ECS.h"
#include "../Logger/Logger.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/TransformComponent.h"

class CollisionSystem : public System {
public:
	CollisionSystem() {
		RequireComponent<BoxColliderComponent>();
		RequireComponent<TransformComponent>();
	}

	void Update() {
		std::vector<Entity> entities = GetSystemEntities();
		for (auto i = entities.begin(); i != entities.end(); i++) {
			Entity entityA = *i;
			TransformComponent transformA = entityA.GetComponent<TransformComponent>();
			BoxColliderComponent &colliderA = entityA.GetComponent<BoxColliderComponent>();

			for (auto j = i; j != entities.end(); j++) {
				Entity entityB = *j;

				if (entityA == entityB) {
					continue;
				}

				TransformComponent transformB = entityB.GetComponent<TransformComponent>();
				BoxColliderComponent &colliderB = entityB.GetComponent<BoxColliderComponent>();

				colliderA.colour.g = 255;
				colliderB.colour.g = 255;

				bool isColliding = CheckAABBCollision(
					transformA.position.x, transformA.position.y,
					colliderA.width, colliderA.height,
					transformB.position.x, transformB.position.y,
					colliderB.width, colliderB.height
				);
				if (isColliding) {
					entityA.Kill();
					entityB.Kill();
					colliderA.colour.g = 0;
					colliderB.colour.g = 0;
				}
			}
		}
	}

	bool CheckAABBCollision(double aX, double aY, double aW, double aH, double bX, double bY, double bW, double bH) {
		return aX < bX + bW
			&& aX + aW > bX
			&& aY < bY + bH
			&& aY + aH > bY;
	}
};

#endif // COLLISION_SYSTEM_H
