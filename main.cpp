//Commands that begin with an asterisk(*) should be removed after all class headers have been included.

#include <SFML/Graphics.hpp>
#include "Game.h"

int main()
{
	Game g("config.txt");
	g.run();
}