#include <iostream>
#include "Game.h"

int main(int argc, char* argv[]) {
	Game game;

	game.Init();
	game.Run();
	game.Cleanup();


	return 0;
}
