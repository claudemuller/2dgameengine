#include <SDL2/SDL_image.h>
#include <glm/glm.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdlrenderer.h>
#include <imgui/imgui_impl_sdl2.h>
#include "Game.h"
#include "LevelLoader.h"
#include "../ECS/ECS.h"
#include "../Logger/Logger.h"
#include "../Systems/MovementSystem.h"
#include "../Systems/RenderSystem.h"
#include "../Systems/AnimationSystem.h"
#include "../Systems/CollisionSystem.h"
#include "../Systems/RenderColliderSystem.h"
#include "../Systems/DamageSystem.h"
#include "../Systems/KeyboardControlSystem.h"
#include "../Systems/CameraMovementSystem.h"
#include "../Systems/ProjectileEmitSystem.h"
#include "../Systems/ProjectileLifecycleSystem.h"
#include "../Systems/RenderTextSystem.h"
#include "../Systems/RenderHealthBarSystem.h"
#include "../Systems/RenderGUISystem.h"
#include "../Systems/ScriptSystem.h"
#include "../Events/KeyPressedEvent.h"

int Game::WindowWidth;
int Game::WindowHeight;
int Game::MapWidth;
int Game::MapHeight;

const char *Game::Groups[] = {"ui", "tiles", "world", "player", "enemies", "projectiles"};

Game::Game() {
    isRunning = false;
    isDebug = false;
    isPaused = false;
    millisecsPreviousFrame = 0;
    entityManager = std::make_unique<EntityManager>();
    assetStore = std::make_unique<AssetStore>();
    eventBus = std::make_unique<EventBus>();
}

Game::~Game() {}

void Game::Init(bool debug) {
    Logger::Log = true;

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
	Logger::Err("error initing SDL");
	return;
    }

    if (TTF_Init() != 0) {
	Logger::Err("error initing TTF");
	return;
    }

    // SDL_DisplayMode displayMode;
    // SDL_GetCurrentDisplayMode(0, &displayMode);
    WindowWidth = 1280; //displayMode.w;
    WindowHeight = 720; //displayMode.h;

    window = SDL_CreateWindow(
	    NULL,
	    SDL_WINDOWPOS_CENTERED,
	    SDL_WINDOWPOS_CENTERED,
	    WindowWidth,
	    WindowHeight,
	    SDL_WINDOW_BORDERLESS
    );
    if (window == nullptr) {
	Logger::Err("error creating window");
	return;
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == nullptr) {
	Logger::Err("error creating renderer");
	return;
    }

    if (!debug)
	SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

    isRunning = true;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    // ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer_Init(renderer);

    camera.x = 0;
    camera.y = 0;
    camera.w = WindowWidth;
    camera.h = WindowHeight;

    Logger::Info("game successfully initialised");
}

void Game::Run() {
    Setup();

    while (isRunning) {
	ProcessInput();
	Update();
	Render();
    }
}

void Game::Setup() {
    entityManager->AddSystem<MovementSystem>();
    entityManager->AddSystem<RenderSystem>();
    entityManager->AddSystem<AnimationSystem>();
    entityManager->AddSystem<CollisionSystem>();
    entityManager->AddSystem<RenderColliderSystem>();
    entityManager->AddSystem<DamageSystem>();
    entityManager->AddSystem<KeyboardControlSystem>();
    entityManager->AddSystem<CameraMovementSystem>();
    entityManager->AddSystem<ProjectileEmitSystem>();
    entityManager->AddSystem<ProjectileLifecycleSystem>();
    entityManager->AddSystem<RenderTextSystem>();
    entityManager->AddSystem<RenderHealthBarSystem>();
    entityManager->AddSystem<RenderGUISystem>();
    entityManager->AddSystem<ScriptSystem>();

    entityManager->GetSystem<ScriptSystem>().CreateScriptBindings(lua);

    LevelLoader loader;
    lua.open_libraries(sol::lib::base, sol::lib::os, sol::lib::math);
    loader.LoadLevel(lua, entityManager, assetStore, renderer, 2);
}

void Game::ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
	ImGui_ImplSDL2_ProcessEvent(&event);
	ImGuiIO &io = ImGui::GetIO();

	int mouseX, mouseY;
	const int buttons = SDL_GetMouseState(&mouseX, &mouseY);

	io.MousePos = ImVec2(mouseX, mouseY);
	io.MouseDown[0] = buttons & SDL_BUTTON(SDL_BUTTON_LEFT);
	io.MouseDown[1] = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);

	switch (event.type) {
	    case SDL_QUIT:
		isRunning = false;
	    break;
	    case SDL_KEYDOWN:
		if (event.key.keysym.sym == SDLK_ESCAPE) {
		    isRunning = false;
		}
		if (event.key.keysym.sym == SDLK_F1) {
		    isDebug = !isDebug;
		}
		if (event.key.keysym.sym == SDLK_p) {
		    isPaused = !isPaused;
		}
		eventBus->EmitEvent<KeyPressedEvent>(event.key.keysym.sym);
	    break;
	}
    }
}

void Game::Update() {
    int timeToWait = MILLISECS_PER_FRAME - (SDL_GetTicks() - millisecsPreviousFrame);
    if (timeToWait > 0 && timeToWait <= MILLISECS_PER_FRAME) SDL_Delay(timeToWait);

    double deltaTime = (SDL_GetTicks() - millisecsPreviousFrame) / 1000.0;
    millisecsPreviousFrame = SDL_GetTicks();

    if (isPaused)
	return;

    eventBus->Reset();
    entityManager->GetSystem<DamageSystem>().SubscribeToEvents(eventBus);
    entityManager->GetSystem<RenderColliderSystem>().SubscribeToEvents(eventBus);
    entityManager->GetSystem<KeyboardControlSystem>().SubscribeToEvents(eventBus);
    entityManager->GetSystem<ProjectileEmitSystem>().SubscribeToEvents(eventBus);
    entityManager->GetSystem<MovementSystem>().SubscribeToEvents(eventBus);

    entityManager->Update();

    entityManager->GetSystem<MovementSystem>().Update(deltaTime);
    entityManager->GetSystem<AnimationSystem>().Update();
    entityManager->GetSystem<CollisionSystem>().Update(eventBus);
    entityManager->GetSystem<ProjectileEmitSystem>().Update(entityManager);
    entityManager->GetSystem<CameraMovementSystem>().Update(camera);
    entityManager->GetSystem<ProjectileLifecycleSystem>().Update();
    entityManager->GetSystem<ScriptSystem>().Update(deltaTime, SDL_GetTicks());
}

void Game::Render() {
    SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
    SDL_RenderClear(renderer);

    entityManager->GetSystem<RenderSystem>().Update(renderer, camera, assetStore);
    entityManager->GetSystem<RenderTextSystem>().Update(renderer, camera, assetStore);
    entityManager->GetSystem<RenderHealthBarSystem>().Update(renderer, camera, assetStore);
    if (isDebug) {
	entityManager->GetSystem<RenderColliderSystem>().Update(renderer, camera);
	entityManager->GetSystem<RenderGUISystem>().Update(entityManager, camera);
    }

    SDL_RenderPresent(renderer);
}

void Game::Cleanup() {
    ImGui_ImplSDLRenderer_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    // TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
