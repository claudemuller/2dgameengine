#include <cstring>
#include <iostream>
#include "./Game/Game.h"

int main(int argc, char* argv[]) {
	bool debug = false;

	Game game;

	if (argc >= 2)
		debug = std::strcmp(argv[1], "debug") == 0;

	game.Init(debug);
	game.Run();
	game.Cleanup();


	return 0;
}
