#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include "../ECS/ECS.h"
#include "../AssetStore/AssetStore.h"

const int FPS = 60;
const int MILLISECS_PER_FRAME = 1000/FPS;

class Game {
private:
	SDL_Window *window;
	SDL_Renderer *renderer;

	bool isRunning;
	bool isDebug;
	int millisecsPreviousFrame;

	std::unique_ptr<EntityManager> entityManager;
	std::unique_ptr<AssetStore> assetStore;

public:
	Game();
	~Game();

	void Init();
	void Run();
	void LoadLevel(int level);
	void Setup();
	void ProcessInput();
	void Update();
	void Render();
	void Cleanup();

	int WindowWidth;
	int WindowHeight;
};

#endif // GAME_H
