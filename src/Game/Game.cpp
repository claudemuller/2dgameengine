#include <iostream>
#include <fstream>
#include <SDL2/SDL_image.h>
#include <glm/glm.hpp>
#include "Game.h"
#include "../Logger/Logger.h"
#include "../ECS/ECS.h"
#include "../Systems/MovementSystem.h"
#include "../Systems/RenderSystem.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"
#include "../Components/SpriteComponent.h"

Game::Game() {
	isRunning = false;
	entityManager = std::make_unique<EntityManager>();
	assetStore = std::make_unique<AssetStore>();
}

Game::~Game() {
}

void Game::Init() {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		Logger::Err("error initing SDL");
		return;
	}

	SDL_DisplayMode displayMode;
	SDL_GetCurrentDisplayMode(1, &displayMode);
	WindowWidth = displayMode.w;
	WindowHeight = displayMode.h;

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

	assetStore->AddTexture(renderer, "tank-image", "./assets/images/tank-panther-right.png");
	assetStore->AddTexture(renderer, "truck-image", "./assets/images/truck-ford-right.png");
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
			tile.AddComponent<SpriteComponent>("tilemap-image", tileSize, tileSize, 0, srcRectX, srcRectY);
		}
	}
	mapFile.close();

	Entity tank = entityManager->CreatEntity();
	tank.AddComponent<TransformComponent>(glm::vec2(10.0, 10.0), glm::vec2(1.0, 1.0), 0.0);
	tank.AddComponent<RigidBodyComponent>(glm::vec2(40.0, 0.0));
	tank.AddComponent<SpriteComponent>("tank-image", 32, 32, 2);

	Entity truck = entityManager->CreatEntity();
	truck.AddComponent<TransformComponent>(glm::vec2(10.0, 10.0), glm::vec2(1.0, 1.0), 0.0);
	truck.AddComponent<RigidBodyComponent>(glm::vec2(50.0, 00.0));
	truck.AddComponent<SpriteComponent>("truck-image", 32, 32, 1);
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
			break;
		}
	}
}

void Game::Update() {
	int timeToWait = MILLISECS_PER_FRAME - (SDL_GetTicks() - millisecsPreviousFrame);
	if (timeToWait > 0 && timeToWait <= MILLISECS_PER_FRAME) SDL_Delay(timeToWait);

	double deltaTime = (SDL_GetTicks() - millisecsPreviousFrame) / 1000.0;
	millisecsPreviousFrame = SDL_GetTicks();

	entityManager->GetSystem<MovementSystem>().Update(deltaTime);

	entityManager->Update();
}

void Game::Render() {
	SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
	SDL_RenderClear(renderer);

	entityManager->GetSystem<RenderSystem>().Update(renderer, assetStore);

	SDL_RenderPresent(renderer);
}

void Game::Cleanup() {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
