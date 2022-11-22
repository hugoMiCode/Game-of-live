#pragma once

#include <SFML/Graphics.hpp>

class Engine
{
public:
	static void init();
	static bool running();
	static void handle_events();
	static void render();

private:
	static sf::RenderWindow window;
};

