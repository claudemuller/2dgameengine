#ifndef KEYBOARD_MOVEMENT_SYSTEM_H
#define KEYBOARD_MOVEMENT_SYSTEM_H

#include "../ECS/ECS.h"
#include "../EventBus/EventBus.h"
#include "../Events/KeyPressedEvent.h"
#include "../Components/KeyboardControlComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/RigidBodyComponent.h"

class KeyboardControlSystem: public System {
public:
	KeyboardControlSystem() {
		RequireComponent<KeyboardControlComponent>();
		RequireComponent<SpriteComponent>();
		RequireComponent<RigidBodyComponent>();
	}

	void SubscribeToEvents(std::unique_ptr<EventBus>& eventBus) {
		eventBus->SubscribeToEvent<KeyPressedEvent>(this, &KeyboardControlSystem::OnKeyPressed);
	}

	void OnKeyPressed(KeyPressedEvent& event) {
		for (auto entity: GetSystemEntities()) {
			const auto keyboardControl = entity.GetComponent<KeyboardControlComponent>();
			auto &sprite = entity.GetComponent<SpriteComponent>();
			auto &rigidBody = entity.GetComponent<RigidBodyComponent>();

			// if (event.symbol == SDLK_RIGHT) rigidBody.velocity += keyboardControl.rightVelocity;
			// if (event.symbol == SDLK_DOWN) rigidBody.velocity += keyboardControl.downVelocity;

			switch (event.symbol) {
				case SDLK_UP:
					rigidBody.velocity = keyboardControl.upVelocity;
					sprite.srcRect.y = sprite.height * 0;
					break;
				case SDLK_RIGHT:
					rigidBody.velocity = keyboardControl.rightVelocity;
					sprite.srcRect.y = sprite.height * 1;
					break;
				case SDLK_DOWN:
					rigidBody.velocity = keyboardControl.downVelocity;
					sprite.srcRect.y = sprite.height * 2;
					break;
				case SDLK_LEFT:
					rigidBody.velocity = keyboardControl.leftVelocity;
					sprite.srcRect.y = sprite.height * 3;
					break;
			}
		}
	}
};

#endif // KEYBOARD_MOVEMENT_SYSTEM_H
