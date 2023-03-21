#include <iostream>
#include <SDL2/SDL_image.h>
#include <glm/glm.hpp>
#include "Game.h"
#include "../Logger/Logger.h"
#include "../ECS/ECS.h"
#include "../Components/TransformComponent.h"
#include "../Components/RigidBodyComponent.h"

Game::Game() {
	isRunning = false;
	entityManager = std::make_unique<EntityManager>();
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
	WindowWidth = 800; //displayMode.w;
	WindowHeight = 600; //displayMode.h;

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

void Game::Setup() {
	Entity tank = entityManager->CreatEntity();

	entityManager->AddComponent<TransformComponent>(tank, glm::vec2(10.0, 30.0), glm::vec2(0.0, 0.0), 0.0);
	entityManager->AddComponent<RigidBodyComponent>(tank, glm::vec2(50.0, 0.0));
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

	float deltaTime = (SDL_GetTicks() - millisecsPreviousFrame) / 1000.0;

	millisecsPreviousFrame = SDL_GetTicks();

	// MovementSystem.Update();
	// CollisionSystem.Update();
}

void Game::Render() {
	SDL_SetRenderDrawColor(renderer, 21, 21, 21, 255);
	SDL_RenderClear(renderer);

	// TODO: Render entitities

	SDL_RenderPresent(renderer);
}

void Game::Cleanup() {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
