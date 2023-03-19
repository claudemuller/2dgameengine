#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>

class Game {
private:
	SDL_Window *window;
	SDL_Renderer *renderer;

	bool isRunning;

public:
	Game();
	~Game();

	void setup();
	void run();
	void processInput();
	void update();
	void render();
	void cleanup();

	int windowWidth;
	int windowHeight;
};

#endif // GAME_H
