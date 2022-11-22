#include "Engine.h"

sf::RenderWindow Engine::window;

void Engine::init()
{
	window.create(sf::VideoMode(400, 400), "Game of live");
}

bool Engine::running()
{
	return window.isOpen();
}

void Engine::handle_events()
{
	sf::Event event;
	while (window.pollEvent(event)) {
		switch (event.type)
		{
		case sf::Event::Closed:
			window.close();
			break;
		default:
			break;
		}
	}
}

void Engine::render()
{
	window.clear();
	window.display();
}
