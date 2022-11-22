#include "Engine.h"
#include <stdlib.h>
#include <iostream>


sf::Clock clock_next;
sf::RenderWindow window;
sf::View view;
int const size_cell{ 5 };
int const windowX{ 1920 };
int const windowY{ 1050 };
int const line{ windowY / size_cell };
int const col{ windowX / size_cell };

int grid[line][col];
int next[line][col];
int upd{ 100 }, bande_infos{ 30 }, rule{ 0 };

enum class Commande { Stop, Play, Accelerated, Slow, Default, Reset, Clear, Grid, Zoom, DeZoom };
Commande commande_act{ Commande::Default };
enum class MouseCommande { Write, ViewReset, Default };
MouseCommande commande_mouse{ MouseCommande::Default };
bool keyPress{ false }, mousePress{ false }, Play{ false }, mouse{ false }, pause{ true }, bool_grid{ false };

void show(sf::RenderWindow&);
void setup();
void update();
void reset();
void Dline(int, int, int, int);
void infos_partie(int compteurCell);
Commande gestion_clavier();
void gestion_sourie(sf::RenderWindow&);
int countCell(int, int);

int main()
{
	window.create(sf::VideoMode(windowX, windowY + bande_infos), "Game of life");
	view.setSize(sf::Vector2f(windowX, windowY + bande_infos));
	view.setCenter(windowX / 2,( windowY + bande_infos) / 2);

	setup();

	while (window.isOpen()) {
		
		sf::Event event;
		while (window.pollEvent(event)) {
			switch (event.type)
			{
			case sf::Event::Closed:
				window.close();
				break;
			case sf::Event::MouseWheelScrolled:
				if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel)
					if (event.mouseWheelScroll.delta == 1) {
						view.zoom(0.7);
						//std::cout << "view.getSize().x : " << view.getSize().x << std::endl;
						//std::cout << "view.getCenter().x : " << view.getCenter().x << std::endl;
					}
					view.setCenter((sf::Vector2f)sf::Mouse::getPosition(window));
					if (event.mouseWheelScroll.delta == -1) {
						view.zoom(1.3);
						view.setCenter((sf::Vector2f)sf::Mouse::getPosition(window));

						if (view.getSize().x >= window.getSize().x && view.getSize().y >= window.getSize().y)
							view.reset(sf::FloatRect(0, 0, window.getSize().x, window.getSize().y));
					}
				break;
			}
			

		}
		gestion_sourie(window);
		switch (gestion_clavier())
		{
		case Commande::Stop:
			pause = true;
			break;
		case Commande::Play:
			pause = false;
			break;
		case Commande::Accelerated:
			upd -= 20;
			break;
		case Commande::Slow:
			upd += 20;
			break;
		case Commande::Default:
			break;
		case Commande::Reset:
			setup();
			break;
		case Commande::Clear:
			upd = 100;
			reset();
			break;
		case Commande::Grid:
			if (bool_grid) 
				bool_grid = false;
			else 
				bool_grid = true;
			break;
		case Commande::Zoom:
			view.zoom(0.9);
			view.setCenter((sf::Vector2f)sf::Mouse::getPosition(window));
			break;
		case Commande::DeZoom:
			view.zoom(1.1);
			view.setCenter((sf::Vector2f)sf::Mouse::getPosition(window));
			break;
		default:
			break;
		}

		window.clear(sf::Color::White);
		if (clock_next.getElapsedTime().asMilliseconds() > upd && !pause) {
			update();
			clock_next.restart();
		}
		show(window);
		window.setView(view);
		window.display();
	}

	return 0;
}

void gestion_sourie(sf::RenderWindow& window)
{
	
	if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
		commande_mouse = MouseCommande::Write;
	}
	else if (sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
		commande_mouse = MouseCommande::ViewReset;
	}
	else {
		mousePress = false;
	}
	int mouseX = sf::Mouse::getPosition(window).x;
	int mouseY = sf::Mouse::getPosition(window).y - bande_infos;

	if (commande_mouse == MouseCommande::Write && !mousePress && mouseX <= window.getSize().x && mouseX >= 0 && mouseY <= window.getSize().y && mouseY >= 0 && view.getSize() == (sf::Vector2f)window.getSize()) {

		mouseX /= size_cell;
		mouseY /= size_cell;

		grid[mouseY][mouseX] = (grid[mouseY][mouseX] + 1) % 2;
		mousePress = true;
	}
	else if (commande_mouse == MouseCommande::ViewReset) {
		view.reset(sf::FloatRect(0, 0, window.getSize().x, window.getSize().y));
	}

	commande_mouse = MouseCommande::Default;
	
}

void reset()
{
	for (int i{ 0 }; i < line; i++) {
		for (int j{ 0 }; j < col; j++) {
			grid[i][j] = 0;
		}
	}
	rule = 0;
	view.reset(sf::FloatRect(0, 0 , window.getSize().x, window.getSize().y));
}

void show(sf::RenderWindow& window)
{
	sf::RectangleShape cell(sf::Vector2f(size_cell, size_cell));

	int compteurCell{ 0 };
	for(int i{0}; i < line; i++) {
		for (int j{ 0 }; j < col; j++) {
			
			if (grid[i][j] == 1)
			{
				compteurCell++;
				cell.setFillColor(sf::Color::Black);
				cell.setPosition(sf::Vector2f(size_cell * j, size_cell * i + bande_infos));
				window.draw(cell);
			}
		}
	}

	if (bool_grid) {
		for (int i{ 0 }; i < window.getSize().x; i += size_cell) {
			Dline(i, bande_infos, i, window.getSize().y);
		} for (int i{ bande_infos }; i < window.getSize().y; i += size_cell) {
			Dline(0, i, window.getSize().x, i);
		}
	}
	infos_partie(compteurCell);
}
void Dline(int x1, int y1, int x2, int y2)
{
	float outline{ 1 };
	sf::ConvexShape line(4);
	line.setPoint(0, sf::Vector2f(x1, y1));
	line.setPoint(1, sf::Vector2f(x2, y2));
	line.setPoint(2, sf::Vector2f(x2, y2));
	line.setPoint(3, sf::Vector2f(x1, y1));

	if (view.getSize().x > (double)window.getSize().x / 2 && view.getSize().y > (double)window.getSize().y / 2)
		outline = 0.5;
	if (view.getSize().x <= (double)window.getSize().x / 2 && view.getSize().y <= (double)window.getSize().y / 2)
		outline = 0.4;
	if (view.getSize().x <= (double)window.getSize().x / 3 && view.getSize().y <= (double)window.getSize().y / 3)
		outline = 0.3;
	if (view.getSize().x <= (double)window.getSize().x / 4 && view.getSize().y <= (double)window.getSize().y / 4)
		outline = 0.2;
	if (view.getSize().x <= (double)window.getSize().x / 5 && view.getSize().y <= (double)window.getSize().y / 5)
		outline = 0.1;

	line.setOutlineThickness(outline);
	line.setOutlineColor(sf::Color(168, 168, 168));
	
	window.draw(line);
}

void infos_partie(int compteurCell)
{
	sf::Font font;
	if (!font.loadFromFile("SIXTY.ttf")) {
		std::cout << "Erreur de chargement de la police!" << std::endl;
	}
	sf::Text infos;
	infos.setFont(font);
	infos.setCharacterSize(20);
	infos.setFillColor(sf::Color::Black);
	infos.setPosition(10, 0);

	sf::Text infos_rule;
	infos_rule.setFont(font);
	infos_rule.setCharacterSize(20);
	infos_rule.setFillColor(sf::Color::Black);
	infos_rule.setPosition(window.getSize().x - 200, 0);
	infos_rule.setString(std::to_string(rule));

	std::string s_pause{ "" };
	if (pause) {
		s_pause = "Pause";
	}
	else {
		s_pause = "Lecture";
	}

	std::string vitesse{ "Undef" };
	if (upd <= 1) {
		upd = 0;
		vitesse = "Max" ;
	} else if (upd >= 200) {
		upd = 200;
		vitesse = "Min" ;
	}
	else {
		vitesse = std::to_string(-upd / 10 + 20) ;
	}
	infos.setString("Infos : vitesse : " + vitesse + ".  Nombre de cellues : " + std::to_string(compteurCell) + ".    " + s_pause + ". ");
	window.draw(infos);
	window.draw(infos_rule);
}

Commande gestion_clavier()
{	
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
		commande_act = Commande::Accelerated;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
		commande_act = Commande::Slow;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
		commande_act = Commande::Stop;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) {
		commande_act = Commande::Reset;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::C)) {
		commande_act = Commande::Clear;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::G)) {
		commande_act = Commande::Grid;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
		commande_act = Commande::Zoom;
	}
	else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
		commande_act = Commande::DeZoom;
	}
	else {
		commande_act = Commande::Default;
		keyPress = false;
	}

	if (!keyPress)
	{
		if (commande_act == Commande::Accelerated) {
			keyPress = true;
			return Commande::Accelerated;
		}
		else if (commande_act == Commande::Slow) {
			keyPress = true;
			return Commande::Slow;
		}
		else if (commande_act == Commande::Stop && Play == true) {
			keyPress = true;
			Play = false;
			return Commande::Stop;
		}
		else if (commande_act == Commande::Stop && Play == false) {
			keyPress = true;
			Play = true;
			return Commande::Play;
		}
		else if (commande_act == Commande::Reset) {
			keyPress = true;
			return Commande::Reset;
		}
		else if (commande_act == Commande::Clear) {
			keyPress = true;
			return Commande::Clear;
		}
		else if (commande_act == Commande::Grid) {
			keyPress = true;
			return Commande::Grid;
		}
		else if (commande_act == Commande::Default) {
			return Commande::Default;
		}
		else if (commande_act == Commande::Zoom) {
			keyPress = true;
			return Commande::Zoom;
		}
		else if (commande_act == Commande::DeZoom) {
			keyPress = true;
			return Commande::DeZoom;
		}
	}
	return Commande::Default;
}

void update()
{

	for (int i{ 0 }; i < line; i++) {
		for (int j{ 0 }; j < col; j++) {
			int neighbors = countCell(i, j);

			if (i == 0 || i == line - 1 || j == 0 || j == col - 1) {
				next[i][j] = grid[i][j];
			}

			int state = grid[i][j];
			if (state == 0 && neighbors == 3) {
				next[i][j] = 1;
			}
			else if (state == 1 && (neighbors < 2 || neighbors > 3)) {
				next[i][j] = 0;
			}
			else {
				next[i][j] = state;
			}
		}
	}
	rule++;


	for (int i{ 0 }; i < line; i++) {
		for (int j{ 0 }; j < col; j++) {
			grid[i][j] = next[i][j];
		}
	}
}

int countCell(int x, int y)
{
	int s{ 0 };
	for (int i{ -1 }; i < 2; i++) {
		for (int j{ -1 }; j < 2; j++) {
			s += grid[x + i][ y + j];
		}
	}

	s -= grid[x][y];

	return s;
}

void setup()
{
	for (int i{ 0 }; i < line; i++) {
		for (int j{ 0 }; j < col; j++) {
			grid[i][j] = rand() % (0, 2);
			//grid[i][j] = 1;
		}
	}
	rule = 0;
	view.reset(sf::FloatRect(0, 0, window.getSize().x, window.getSize().y));
}

