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

static void HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort) && ImGui::BeginTooltip())
    {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

class RenderGUISystem: public System {
private:
	void renderInfoOverlay(const std::unique_ptr<EntityManager> &entityManager);
	void renderAddEnemies(const std::unique_ptr<EntityManager> &entityManager);

public:
	RenderGUISystem() = default;

	void Update(const std::unique_ptr<EntityManager> &entityManager) {
		ImGui_ImplSDLRenderer_NewFrame();
        ImGui_ImplSDL2_NewFrame();
		ImGui::NewFrame();

		renderInfoOverlay(entityManager);
		renderAddEnemies(entityManager);

		ImGui::Render();
        ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
	}
};

void RenderGUISystem::renderInfoOverlay(const std::unique_ptr<EntityManager> &entityManager) {
	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

	ImGui::SetNextWindowBgAlpha(0.35f);
    if (ImGui::Begin("World Info", nullptr, windowFlags)) {
		ImGui::Text("World Info");
        ImGui::Separator();

		ImGuiIO& io = ImGui::GetIO();
		ImGui::Text("Map Co-ords:  (%.1f,%.1f)", io.MousePos.x, io.MousePos.y);
        ImGui::Separator();

		ImGui::Text("NumEntites: %zu", entityManager->NumEntites());
        ImGui::Separator();

		ImGui::Text("Entities by Group");
        ImGui::Separator();

		for (size_t i = 0; i < 5; i++) {
			auto n = entityManager->GetEntitiesByGroup(Game::Groups[i]);
			if (!n.empty())
				ImGui::Text("%s: %zu", Game::Groups[i], n.size());
		}

	}
	ImGui::End();
}

void RenderGUISystem::renderAddEnemies(const std::unique_ptr<EntityManager> &entityManager) {
	if (ImGui::Begin("Spawn Enemies", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize)) {
		ImGui::Text("Spawn enemies here by entering some values.");

		const char* groupItems[] = {"ui", "player", "enemies", "tiles", "projectiles"};
		static int curGroupIdx = 0;

		ImGui::SeparatorText("Properties");
		if (ImGui::CollapsingHeader("Group", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::Combo("group", &curGroupIdx, groupItems, IM_ARRAYSIZE(groupItems));
		}
		ImGui::Spacing();

		const char* spriteItems[] = {"tank-image", "truck-image", "chopper-image"};
		static int curSpriteIdx = 0;

		if (ImGui::CollapsingHeader("Sprite", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::Combo("sprite", &curSpriteIdx, spriteItems, IM_ARRAYSIZE(spriteItems));
		}
		ImGui::Spacing();

		static int health = 100;
		if (ImGui::CollapsingHeader("Health", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::InputInt("health", &health);
		}
		ImGui::Spacing();

		char *sliderToolTip = "CTRL+click to input value.";

		static int x = 0, y = 0;
		static float scaleX = 1.0, scaleY = 1.0;
		static float rot = 0.0;
		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::SliderInt("position x", &x, 0, 1920);
			ImGui::SameLine(); HelpMarker(sliderToolTip);
			ImGui::SliderInt("position y", &y, 0, 720);
			ImGui::SameLine(); HelpMarker(sliderToolTip);

			ImGui::SliderFloat("scale x", &scaleX, 1, 100);
			ImGui::SameLine(); HelpMarker(sliderToolTip);
			ImGui::SliderFloat("scale y", &scaleY, 1, 100);
			ImGui::SameLine(); HelpMarker(sliderToolTip);

			ImGui::SliderAngle("rotation", &rot, 0, 360);
			ImGui::SameLine(); HelpMarker(sliderToolTip);
		}
		ImGui::Spacing();

		static float velX = 0.0, velY = 0.0;
		if (ImGui::CollapsingHeader("Rigid body", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::SameLine(); HelpMarker(sliderToolTip);
			ImGui::SliderFloat("velocity x", &velX, 0, 1000);
			ImGui::SameLine(); HelpMarker(sliderToolTip);
			ImGui::SliderFloat("velocity y", &velY, 0, 1000);
			ImGui::SameLine(); HelpMarker(sliderToolTip);
		}
		ImGui::Spacing();

		static float projectileVelX = 100.0, projectileVelY = 0;
		static int projectileFreq = 1000, projectileDuration = 2000, projectileHitDamage = 10;
		if (ImGui::CollapsingHeader("Projectile Emitter", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::SliderFloat("velocity x", &projectileVelX, 0, 1000);
			ImGui::SliderFloat("velocity y", &projectileVelY, 0, 1000);

			ImGui::InputInt("frequency", &projectileFreq);
			ImGui::InputInt("duration", &projectileDuration);
			ImGui::InputInt("hit damage", &projectileHitDamage);
		}
		ImGui::Spacing();

		if (ImGui::Button("Spawn Enemy")) {
			Entity entity = entityManager->CreatEntity();
			entity.Group(Game::Groups[Game::ENEMIES]);
			entity.Group(groupItems[curGroupIdx]);
			entity.AddComponent<TransformComponent>(glm::vec2(x, y), glm::vec2(scaleX, scaleY), rot);
			entity.AddComponent<RigidBodyComponent>(glm::vec2(velX, velY));
			entity.AddComponent<SpriteComponent>(spriteItems[curSpriteIdx], 32, 32, 2);
			entity.AddComponent<BoxColliderComponent>(32, 32);
			entity.AddComponent<ProjectileEmitterComponent>(
				glm::vec2(projectileVelX, projectileVelY),
				projectileFreq,
				projectileDuration,
				projectileHitDamage,
				false
			);
			entity.AddComponent<HealthComponent>(health);
		}
	}
	ImGui::End();
}

#endif // RENDER_GUI_SYSTEM_H
