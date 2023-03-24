#include <iostream>
#include <fstream>
#include <SDL2/SDL_image.h>
#include <glm/glm.hpp>
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
#include "../Events/KeyPressedEvent.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"
#include "../Components/AnimationComponent.h"
#include "../Components/BoxColliderComponent.h"
#include "../Components/KeyboardControlComponent.h"
#include "../Components/CameraFollowComponent.h"

int Game::WindowWidth;
int Game::WindowHeight;
int Game::MapWidth;
int Game::MapHeight;

Game::Game() {
	isRunning = false;
	isDebug = false;
	entityManager = std::make_unique<EntityManager>();
	assetStore = std::make_unique<AssetStore>();
	eventBus = std::make_unique<EventBus>();
}

Game::~Game() {
}

void Game::Init() {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		Logger::Err("error initing SDL");
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

	SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
	isRunning = true;

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

	assetStore->AddTexture(renderer, "tank-image", "./assets/images/tank-panther-right.png");
	assetStore->AddTexture(renderer, "truck-image", "./assets/images/truck-ford-right.png");
	assetStore->AddTexture(renderer, "chopper-image", "./assets/images/chopper-spritesheet.png");
	assetStore->AddTexture(renderer, "radar-image", "./assets/images/radar.png");
	assetStore->AddTexture(renderer, "tilemap-image", "./assets/tilemaps/jungle.png");

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
	radar.AddComponent<TransformComponent>(glm::vec2(WindowWidth - 74, 10.0), glm::vec2(1.0, 1.0), 0.0);
	radar.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 0.0));
	radar.AddComponent<SpriteComponent>("radar-image", 64, 64, 2, true);
	radar.AddComponent<AnimationComponent>(8, 5, true);

	Entity chopper = entityManager->CreatEntity();
	chopper.AddComponent<TransformComponent>(glm::vec2(100.0, 100.0), glm::vec2(1.0, 1.0), 0.0);
	chopper.AddComponent<RigidBodyComponent>(glm::vec2(0.0, 0.0));
	chopper.AddComponent<SpriteComponent>("chopper-image", 32, 32, 1);
	chopper.AddComponent<AnimationComponent>(2, 15, true);
	chopper.AddComponent<KeyboardControlComponent>(glm::vec2(0, -80), glm::vec2(80, 0), glm::vec2(0, 80), glm::vec2(-80, 0));
	chopper.AddComponent<CameraFollowComponent>();

	Entity tank = entityManager->CreatEntity();
	tank.AddComponent<TransformComponent>(glm::vec2(500.0, 10.0), glm::vec2(1.0, 1.0), 0.0);
	tank.AddComponent<RigidBodyComponent>(glm::vec2(50.0, 0.0));
	tank.AddComponent<SpriteComponent>("tank-image", 32, 32, 2);
	tank.AddComponent<BoxColliderComponent>(32, 32);

	Entity truck = entityManager->CreatEntity();
	truck.AddComponent<TransformComponent>(glm::vec2(10.0, 10.0), glm::vec2(1.0, 1.0), 0.0);
	truck.AddComponent<RigidBodyComponent>(glm::vec2(50.0, 00.0));
	truck.AddComponent<SpriteComponent>("truck-image", 32, 32, 1);
	truck.AddComponent<BoxColliderComponent>(32, 32);
}

void Game::Setup() {
	LoadLevel(1);
}

void Game::ProcessInput() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
			isRunning = false;
			break;
		case SDL_KEYDOWN:
			if (event.key.keysym.sym == SDLK_ESCAPE) {
				isRunning = false;
			}
			if (event.key.keysym.sym == SDLK_d) {
				isDebug = !isDebug;
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

	eventBus->Reset();
	entityManager->GetSystem<DamageSystem>().SubscribeToEvents(eventBus);
	entityManager->GetSystem<RenderColliderSystem>().SubscribeToEvents(eventBus);
	entityManager->GetSystem<KeyboardControlSystem>().SubscribeToEvents(eventBus);

	entityManager->Update();

	entityManager->GetSystem<MovementSystem>().Update(deltaTime);
	entityManager->GetSystem<AnimationSystem>().Update();
	entityManager->GetSystem<CollisionSystem>().Update(eventBus);
	entityManager->GetSystem<CameraMovementSystem>().Update(camera);
}

void Game::Render() {
	SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
	SDL_RenderClear(renderer);

	entityManager->GetSystem<RenderSystem>().Update(renderer, assetStore, camera);
	if (isDebug) entityManager->GetSystem<RenderColliderSystem>().Update(renderer, camera);

	SDL_RenderPresent(renderer);
}

void Game::Cleanup() {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
