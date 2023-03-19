#include <iostream>
#include "Game.h"

Game::Game() {
	isRunning = false;
}

Game::~Game() {

}

void Game::setup() {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		std::cerr << "error initing SDL" << std::endl;
		return;
	}

	SDL_DisplayMode displayMode;
	SDL_GetCurrentDisplayMode(1, &displayMode);
	windowWidth = 800; //displayMode.w;
	windowHeight = 600; //displayMode.h;

	window = SDL_CreateWindow(
			NULL,
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			windowWidth,
			windowHeight,
			SDL_WINDOW_BORDERLESS
	);
	if (window == nullptr) {
		std::cerr << "error creating window" << std::endl;
		return;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (renderer == nullptr) {
		std::cerr << "error creating renderer" << std::endl;
		return;
	}

	SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
	isRunning = true;
}

void Game::run() {
	while (isRunning) {
		processInput();
		update();
		render();
	}
}

void Game::processInput() {
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

void Game::update() {

}

void Game::render() {
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	SDL_RenderClear(renderer);

	SDL_RenderPresent(renderer);
}

void Game::cleanup() {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}
