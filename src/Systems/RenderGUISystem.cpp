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

		const char* groupItems[] = {"ui", "player", "enemies", "projectiles", "tiles", "world"};
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
			Entity entity = entityManager->CreateEntity();
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

struct ExampleAppLog {
    ImGuiTextBuffer     Buf;
    ImGuiTextFilter     Filter;
    ImVector<int>       LineOffsets; // Index to lines offset. We maintain this with AddLog() calls.
    bool                AutoScroll;  // Keep scrolling if already at the bottom.

    ExampleAppLog()
    {
        AutoScroll = true;
        Clear();
    }

    void    Clear()
    {
        Buf.clear();
        LineOffsets.clear();
        LineOffsets.push_back(0);
    }

    void    AddLog(const char* fmt, ...) IM_FMTARGS(2)
    {
        int old_size = Buf.size();
        va_list args;
        va_start(args, fmt);
        Buf.appendfv(fmt, args);
        va_end(args);
        for (int new_size = Buf.size(); old_size < new_size; old_size++)
            if (Buf[old_size] == '\n')
                LineOffsets.push_back(old_size + 1);
    }

    void    Draw(const char* title, bool* p_open = NULL)
    {
        if (!ImGui::Begin(title, p_open))
        {
            ImGui::End();
            return;
        }

        // Options menu
        if (ImGui::BeginPopup("Options"))
        {
            ImGui::Checkbox("Auto-scroll", &AutoScroll);
            ImGui::EndPopup();
        }

        // Main window
        if (ImGui::Button("Options"))
            ImGui::OpenPopup("Options");
        ImGui::SameLine();
        bool clear = ImGui::Button("Clear");
        ImGui::SameLine();
        bool copy = ImGui::Button("Copy");
        ImGui::SameLine();
        Filter.Draw("Filter", -100.0f);

        ImGui::Separator();

        if (ImGui::BeginChild("scrolling", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar))
        {
            if (clear)
                Clear();
            if (copy)
                ImGui::LogToClipboard();

            ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
            const char* buf = Buf.begin();
            const char* buf_end = Buf.end();
            if (Filter.IsActive())
            {
                // In this example we don't use the clipper when Filter is enabled.
                // This is because we don't have random access to the result of our filter.
                // A real application processing logs with ten of thousands of entries may want to store the result of
                // search/filter.. especially if the filtering function is not trivial (e.g. reg-exp).
                for (int line_no = 0; line_no < LineOffsets.Size; line_no++)
                {
                    const char* line_start = buf + LineOffsets[line_no];
                    const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
                    if (Filter.PassFilter(line_start, line_end))
                        ImGui::TextUnformatted(line_start, line_end);
                }
            }
            else
            {
                // The simplest and easy way to display the entire buffer:
                //   ImGui::TextUnformatted(buf_begin, buf_end);
                // And it'll just work. TextUnformatted() has specialization for large blob of text and will fast-forward
                // to skip non-visible lines. Here we instead demonstrate using the clipper to only process lines that are
                // within the visible area.
                // If you have tens of thousands of items and their processing cost is non-negligible, coarse clipping them
                // on your side is recommended. Using ImGuiListClipper requires
                // - A) random access into your data
                // - B) items all being the  same height,
                // both of which we can handle since we have an array pointing to the beginning of each line of text.
                // When using the filter (in the block of code above) we don't have random access into the data to display
                // anymore, which is why we don't use the clipper. Storing or skimming through the search result would make
                // it possible (and would be recommended if you want to search through tens of thousands of entries).
                ImGuiListClipper clipper;
                clipper.Begin(LineOffsets.Size);
                while (clipper.Step())
                {
                    for (int line_no = clipper.DisplayStart; line_no < clipper.DisplayEnd; line_no++)
                    {
                        const char* line_start = buf + LineOffsets[line_no];
                        const char* line_end = (line_no + 1 < LineOffsets.Size) ? (buf + LineOffsets[line_no + 1] - 1) : buf_end;
                        ImGui::TextUnformatted(line_start, line_end);
                    }
                }
                clipper.End();
            }
            ImGui::PopStyleVar();

            // Keep up at the bottom of the scroll region if we were already at the bottom at the beginning of the frame.
            // Using a scrollbar or mouse-wheel will take away from the bottom edge.
            if (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
                ImGui::SetScrollHereY(1.0f);
        }
        ImGui::EndChild();
        ImGui::End();
    }
};

void
RenderGUISystem::renderLogs()
{
    static ExampleAppLog log;

    // For the demo: add a debug button _BEFORE_ the normal log window contents
    // We take advantage of a rarely used feature: multiple calls to Begin()/End() are appending to the _same_ window.
    // Most of the contents of the window will be added by the log.Draw() call.
    ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
    ImGui::Begin("Logs:", NULL);
    // IMGUI_DEMO_MARKER("Examples/Log");
    // if (ImGui::SmallButton("[Debug] Add 5 entries"))
    // {
    //     static int counter = 0;
    //     const char* categories[3] = { "info", "warn", "error" };
    //     const char* words[] = { "Bumfuzzled", "Cattywampus", "Snickersnee", "Abibliophobia", "Absquatulate", "Nincompoop", "Pauciloquent" };
    //     for (int n = 0; n < 5; n++) {
    //         const char* category = categories[counter % IM_ARRAYSIZE(categories)];
    //         const char* word = words[counter % IM_ARRAYSIZE(words)];
    //         log.AddLog("[%05d] [%s] Hello, current time is %.1f, here's a word: '%s'\n",
    //             ImGui::GetFrameCount(), category, ImGui::GetTime(), word);
    //         counter++;
    //     }
    // }

	// TODO: fix this
	const std::string emojis[] = {"ℹ️ ", "⚠️ ", "☠️ "};
	const std::string typeStrs[] = {"INFO", "WARN", "ERR"};

	for (LogEntry l: Logger::messages)
		log.AddLog("%s [%s] %s\n", emojis[l.type].c_str(), typeStrs[l.type].c_str(), l.message.c_str());
    ImGui::End();

    // Actually call in the regular Log helper (which will Begin() into the same window as we just did)
    log.Draw("Logs:", NULL);
}
