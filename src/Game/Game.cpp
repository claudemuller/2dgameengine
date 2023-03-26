#include <iostream>
#include <fstream>
#include <SDL2/SDL_image.h>
#include <glm/glm.hpp>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdlrenderer.h>
#include <imgui/imgui_impl_sdl2.h>
#include "Game.h"
#include "../Logger/Logger.h"
#include "../ECS/ECS.h"
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
#include "../Events/KeyPressedEvent.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/AnimationComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/KeyboardControlComponent.h"
#include "../Components/CameraFollowComponent.h"
#include "../Components/ProjectileEmitterComponent.h"
#include "../Components/HealthComponent.h"
#include "../Components/TextLabelComponent.h"

int Game::WindowWidth;
int Game::WindowHeight;
int Game::MapWidth;
int Game::MapHeight;

const char *Game::Groups[] = {"ui", "tiles", "world", "player", "enemies", "projectiles"};

Game::Game() {
	isRunning = false;
	isDebug = false;
	isPaused = false;
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

void Game::LoadLevel(int level) {
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

	assetStore->AddTexture(renderer, "tank-image", "./assets/images/tank-panther-right.png");
	assetStore->AddTexture(renderer, "truck-image", "./assets/images/truck-ford-right.png");
	assetStore->AddTexture(renderer, "tree-image", "./assets/images/tree.png");
	assetStore->AddTexture(renderer, "chopper-image", "./assets/images/chopper-spritesheet.png");
	assetStore->AddTexture(renderer, "radar-image", "./assets/images/radar.png");
	assetStore->AddTexture(renderer, "tilemap-image", "./assets/tilemaps/jungle.png");
	assetStore->AddTexture(renderer, "bullet-image", "./assets/images/bullet.png");
	assetStore->AddFont("charriot-font-20", "./assets/fonts/charriot.ttf", 20);
	assetStore->AddFont("pico8-font-5", "./assets/fonts/pico8.ttf", 5);
	assetStore->AddFont("pico8-font-10", "./assets/fonts/pico8.ttf", 10);

	int tileSize = 32;
	double tileScale = 2.0;
	int mapNumCols = 25;
	int mapNumRows = 20;
	std::fstream mapFile;
	mapFile.open("./assets/tilemaps/jungle.map");

	for (int y = 0; y < mapNumRows; y++) {
		for (int x = 0; x < mapNumCols; x++) {
			char ch;
			mapFile.get(ch);
			int srcRectY = std::atoi(&ch) * tileSize;
			mapFile.get(ch);
			int srcRectX = std::atoi(&ch) * tileSize;
			mapFile.ignore();

			Entity tile = entityManager->CreatEntity();
			tile.Group(Game::Groups[TILES]);
			tile.AddComponent<TransformComponent>(
				glm::vec2(x * (tileScale*tileSize), y * (tileScale*tileSize)),
				glm::vec2(tileScale, tileScale),
				0.0
			);
			tile.AddComponent<SpriteComponent>("tilemap-image", tileSize, tileSize, 0, false, srcRectX, srcRectY);
		}
	}
	mapFile.close();
	MapWidth = tileSize * mapNumCols * tileScale;
	MapHeight = tileSize * mapNumRows * tileScale;

	Entity radar = entityManager->CreatEntity();
	radar.Group(Game::Groups[UI]);
	radar.AddComponent<TransformComponent>(glm::vec2(WindowWidth - 74, 10.0), glm::vec2(1.0, 1.0), 0.0);
	radar.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 0.0));
	radar.AddComponent<SpriteComponent>("radar-image", 64, 64, 2, true);
	radar.AddComponent<AnimationComponent>(8, 5, true);

	Entity chopper = entityManager->CreatEntity();
	chopper.Tag("player");
	chopper.Group(Game::Groups[PLAYER]);
	chopper.AddComponent<TransformComponent>(glm::vec2(250.0, 120.0), glm::vec2(1.0, 1.0), 0.0);
	chopper.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 0.0));
	chopper.AddComponent<SpriteComponent>("chopper-image", 32, 32, 1);
	chopper.AddComponent<AnimationComponent>(2, 15, true);
	chopper.AddComponent<BoxColliderComponent>(32, 32);
	chopper.AddComponent<ProjectileEmitterComponent>(glm::vec2(150.0, 150.0), 0, 10000, 10, true);
	chopper.AddComponent<KeyboardControlComponent>(glm::vec2(0, -80), glm::vec2(80, 0), glm::vec2(0, 80), glm::vec2(-80, 0));
	chopper.AddComponent<CameraFollowComponent>();
	chopper.AddComponent<HealthComponent>(100);

	Entity tank = entityManager->CreatEntity();
	tank.Group(Game::Groups[ENEMIES]);
	tank.AddComponent<TransformComponent>(glm::vec2(450.0, 490.0), glm::vec2(1.0, 1.0), 0.0);
	tank.AddComponent<RigidBodyComponent>(glm::vec2(20.0, 0.0));
	tank.AddComponent<SpriteComponent>("tank-image", 32, 32, 2);
	tank.AddComponent<BoxColliderComponent>(32, 32);
	tank.AddComponent<ProjectileEmitterComponent>(glm::vec2(100.0, 0.0), 5000, 3000, 10, false);
	tank.AddComponent<HealthComponent>(100);

	Entity truck = entityManager->CreatEntity();
	truck.Group(Game::Groups[ENEMIES]);
	truck.AddComponent<TransformComponent>(glm::vec2(770.0, 140.0), glm::vec2(1.0, 1.0), 0.0);
	truck.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 0.0));
	truck.AddComponent<SpriteComponent>("truck-image", 32, 32, 1);
	truck.AddComponent<BoxColliderComponent>(32, 32);
	truck.AddComponent<ProjectileEmitterComponent>(glm::vec2(0.0, 100.0), 2000, 5000, 10, false);
	truck.AddComponent<HealthComponent>(100);

	Entity treeA = entityManager->CreatEntity();
	treeA.Group(Game::Groups[WORLD]);
	treeA.AddComponent<TransformComponent>(glm::vec2(600.0, 490.0), glm::vec2(1.0, 1.0), 0.0);
	treeA.AddComponent<RigidBodyComponent>(glm::vec2(0));
	treeA.AddComponent<SpriteComponent>("tree-image", 16, 32, 2);
	treeA.AddComponent<BoxColliderComponent>(16, 32);

	Entity treeB = entityManager->CreatEntity();
	treeB.Group(Game::Groups[WORLD]);
	treeB.AddComponent<TransformComponent>(glm::vec2(400.0, 490.0), glm::vec2(1.0, 1.0), 0.0);
	treeB.AddComponent<RigidBodyComponent>(glm::vec2(0));
	treeB.AddComponent<SpriteComponent>("tree-image", 16, 32, 2);
	treeB.AddComponent<BoxColliderComponent>(16, 32);

	Entity label = entityManager->CreatEntity();
	label.Group(Game::Groups[UI]);
	SDL_Color green = {0, 255, 0};
	label.AddComponent<TextLabelComponent>(glm::vec2(WindowWidth / 2 - 40, 10), "Chopper v1.0", "charriot-font-20", green);
}

void Game::Setup() {
	LoadLevel(1);
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
