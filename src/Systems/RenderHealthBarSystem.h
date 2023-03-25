#ifndef RENDER_HEALTH_BAR_SYSTEM_H
#define RENDER_HEALTH_BAR_SYSTEM_H

#include <SDL2/SDL.h>
#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/HealthComponent.h"
#include "../AssetStore/AssetStore.h"

class RenderHealthBarSystem : public System {
public:
    public:
        RenderHealthBarSystem() {
            RequireComponent<TransformComponent>();
            RequireComponent<SpriteComponent>();
            RequireComponent<HealthComponent>();
        }

        void Update(SDL_Renderer *renderer, const SDL_Rect &camera, const std::unique_ptr<AssetStore> &assetStore) {
            for (auto entity: GetSystemEntities()) {
                const auto transform = entity.GetComponent<TransformComponent>();
                const auto sprite = entity.GetComponent<SpriteComponent>();
                const auto health = entity.GetComponent<HealthComponent>();

                SDL_Color healthBarColor = {255, 255, 255};

                if (health.healthPercentage >= 0 && health.healthPercentage < 40) {
                    healthBarColor = {255, 0, 0};
                }
                if (health.healthPercentage >= 40 && health.healthPercentage < 80) {
                    healthBarColor = {255, 255, 0};
                }
                if (health.healthPercentage >= 80 && health.healthPercentage <= 100) {
                    healthBarColor = {0, 255, 0};
                }

                int healthBarWidth = 15;
                int healthBarHeight = 3;
                double healthBarPosX = (transform.position.x + (sprite.width * transform.scale.x)) - camera.x;
                double healthBarPosY = (transform.position.y) - camera.y;

                SDL_Rect healthBarRectangle = {
                    static_cast<int>(healthBarPosX),
                    static_cast<int>(healthBarPosY),
                    static_cast<int>(healthBarWidth * (health.healthPercentage / 100.0)),
                    static_cast<int>(healthBarHeight)
                };
                SDL_SetRenderDrawColor(renderer, healthBarColor.r, healthBarColor.g, healthBarColor.b, 255);
                SDL_RenderFillRect(renderer, &healthBarRectangle);

                std::string healthText = std::to_string(health.healthPercentage) + "%";
                SDL_Surface *surface = TTF_RenderText_Blended(
					assetStore->GetFont("pico8-font-5"),
					healthText.c_str(),
					healthBarColor
				);
                SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
                SDL_FreeSurface(surface);

                int labelWidth = 0;
                int labelHeight = 0;
                SDL_QueryTexture(texture, NULL, NULL, &labelWidth, &labelHeight);
                SDL_Rect healthBarTextRectangle = {
                    static_cast<int>(healthBarPosX),
                    static_cast<int>(healthBarPosY) + 5,
                    labelWidth,
                    labelHeight
                };

                SDL_RenderCopy(renderer, texture, NULL, &healthBarTextRectangle);

                SDL_DestroyTexture(texture);
            }
        }
};

#endif // RENDER_HEALTH_BAR_SYSTEM_H
