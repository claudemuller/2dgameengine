#ifndef RENDER_GUI_SYSTEM_H
#define RENDER_GUI_SYSTEM_H

# include <imgui/imgui_impl_sdlrenderer.h>
# include <imgui/imgui_impl_sdl2.h>
#include "../ECS/ECS.h"
#include "../Game/Game.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/ProjectileEmitterComponent.h"
#include "../Components/HealthComponent.h"

class RenderGUISystem: public System {
private:
	void renderInfoOverlay(const std::unique_ptr<EntityManager> &entityManager, SDL_Rect &camera);
	void renderAddEnemies(const std::unique_ptr<EntityManager> &entityManager, SDL_Rect &camera);
	void renderLogs();

public:
	RenderGUISystem() = default;

	void Update(const std::unique_ptr<EntityManager> &entityManager, SDL_Rect &camera) {
		ImGui_ImplSDLRenderer_NewFrame();
        ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		renderInfoOverlay(entityManager, camera);
		renderAddEnemies(entityManager, camera);
		renderLogs();

		ImGui::Render();
        ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
	}
};

#endif // RENDER_GUI_SYSTEM_H
