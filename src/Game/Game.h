#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include "../ECS/ECS.h"

const int FPS = 60;
const int MILLISECS_PER_FRAME = 1000/FPS;

class Game {
private:
	SDL_Window *window;
	SDL_Renderer *renderer;

	bool isRunning;
	int millisecsPreviousFrame;

	std::unique_ptr<EntityManager> entityManager;

public:
	Game();
	~Game();

	void Init();
	void Run();
	void Setup();
	void ProcessInput();
	void Update();
	void Render();
	void Cleanup();

	int WindowWidth;
	int WindowHeight;
};

#endif // GAME_H
