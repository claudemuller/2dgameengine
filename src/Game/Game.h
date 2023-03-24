#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include "../ECS/ECS.h"
#include "../AssetStore/AssetStore.h"
#include "../EventBus/EventBus.h"

const int FPS = 60;
const int MILLISECS_PER_FRAME = 1000/FPS;

class Game {
private:
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Rect camera;

	bool isRunning;
	bool isDebug;
	int millisecsPreviousFrame;

	std::unique_ptr<EntityManager> entityManager;
	std::unique_ptr<AssetStore> assetStore;
	std::unique_ptr<EventBus> eventBus;

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

	static int WindowWidth;
	static int WindowHeight;
	static int MapWidth;
	static int MapHeight;
};

#endif // GAME_H
