#include "RenderGUISystem.h"

static void
HelpMarker(const char* desc)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort) && ImGui::BeginTooltip()) {
        ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
        ImGui::TextUnformatted(desc);
        ImGui::PopTextWrapPos();
        ImGui::EndTooltip();
    }
}

void
RenderGUISystem::renderInfoOverlay(const std::unique_ptr<EntityManager> &entityManager, SDL_Rect &camera)
{
	ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;

	ImGui::SetNextWindowBgAlpha(0.35f);
    if (ImGui::Begin("World Info", nullptr, windowFlags)) {
		ImGui::Text("World Info");
        ImGui::Separator();

		ImGuiIO& io = ImGui::GetIO();
		ImGui::Text("Map Co-ords:  (%.1f,%.1f)", io.MousePos.x+camera.x, io.MousePos.y+camera.x);
        ImGui::Separator();

		ImGui::Text("Total Entites: %zu", entityManager->NumEntites());
        ImGui::Separator();

		ImGui::SeparatorText("Entities by Group");
        ImGui::Separator();

		for (size_t i = 0; i < 5; i++) {
			auto n = entityManager->GetEntitiesByGroup(Game::Groups[i]);
			if (!n.empty())
				ImGui::Text("%s: %zu", Game::Groups[i], n.size());
		}

	}
	ImGui::End();
}

void
RenderGUISystem::renderAddEnemies(const std::unique_ptr<EntityManager> &entityManager, SDL_Rect &camera)
{
	if (ImGui::Begin("Spawn Enemies", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoResize)) {
		ImGui::Text("Spawn enemies here by entering some values.");

		const char* groupItems[] = {"ui", "player", "enemies", "tiles", "projectiles"};
		static int curGroupIdx = 2;

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
			ImGui::InputInt("%", &health);
		}
		ImGui::Spacing();

		std::string sliderToolTip = "CTRL+click to input value.";

		static int x = 0, y = 0;
		static int scaleX = 1, scaleY = 1;
		static float rot = 0.0;
		if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::SliderInt("position x", &x, 0, 1920);
			ImGui::SameLine(); HelpMarker(sliderToolTip.c_str());
			ImGui::SliderInt("position y", &y, 0, 720);
			ImGui::SameLine(); HelpMarker(sliderToolTip.c_str());

			ImGui::SliderInt("scale x", &scaleX, 1, 5);
			ImGui::SameLine(); HelpMarker(sliderToolTip.c_str());
			ImGui::SliderInt("scale y", &scaleY, 1, 5);
			ImGui::SameLine(); HelpMarker(sliderToolTip.c_str());

			ImGui::SliderAngle("rotation", &rot, 0, 360);
			ImGui::SameLine(); HelpMarker(sliderToolTip.c_str());
		}
		ImGui::Spacing();

		static float velX = 0.0, velY = 0.0;
		if (ImGui::CollapsingHeader("Rigid body", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::SameLine(); HelpMarker(sliderToolTip.c_str());
			ImGui::SliderFloat("velocity x", &velX, 0, 1000);
			ImGui::SameLine(); HelpMarker(sliderToolTip.c_str());
			ImGui::SliderFloat("velocity y", &velY, 0, 1000);
			ImGui::SameLine(); HelpMarker(sliderToolTip.c_str());
		}
		ImGui::Spacing();

		static float projectileSpeed = 100.0;
		static int projectileFreq = 1, projectileDuration = 2, projectileHitDamage = 10;
		if (ImGui::CollapsingHeader("Projectile Emitter", ImGuiTreeNodeFlags_DefaultOpen)) {
			ImGui::SliderFloat("speed", &projectileSpeed, 0, 1000);
			ImGui::InputInt("frequency (s)", &projectileFreq);
			ImGui::InputInt("duration (s)", &projectileDuration);
			ImGui::InputInt("hit damage", &projectileHitDamage);
		}

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		if (ImGui::Button("Spawn Enemy")) {
			Entity entity = entityManager->CreatEntity();
			entity.Group(groupItems[curGroupIdx]);
			entity.AddComponent<TransformComponent>(glm::vec2(x, y), glm::vec2(scaleX, scaleY), glm::degrees(rot));
			entity.AddComponent<RigidBodyComponent>(glm::vec2(velX, velY));
			entity.AddComponent<SpriteComponent>(spriteItems[curSpriteIdx], 32, 32, 2);
			entity.AddComponent<BoxColliderComponent>(32, 32);
			double projectileVelX = cos(rot) * projectileSpeed;
			double projectileVelY = sin(rot) * projectileSpeed;
			entity.AddComponent<ProjectileEmitterComponent>(
				glm::vec2(projectileVelX, projectileVelY),
				projectileFreq * 1000,
				projectileDuration * 1000,
				projectileHitDamage,
				false
			);
			entity.AddComponent<HealthComponent>(health);
		}
	}
	ImGui::End();
}

