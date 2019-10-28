#include "Game.h"

int main(int argc, char** argv)
{
	Game* game = new Game(argc, argv);
	delete game;
	return 0;
}