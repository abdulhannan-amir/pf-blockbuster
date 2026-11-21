#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
using namespace std;

const int ROWS = 8;
const int COLS = 10;
int bricks[ROWS][COLS];

const float Brick_W = 60.f;
const float Brick_H = 25.f;
const float Brick_Y = 100.f;

bool loadLevel() {

	ifstream fin("level.txt");
	if (!fin)
		return false;

	int rows, cols;
	fin >> rows >> cols;

	for (int i = 0; i < ROWS; i++) {
		for (int j = 0; j < COLS; j++) {
			fin >> bricks[i][j];
		}
	}

	return true;
}

void drawBricks(sf::RenderWindow& window)
{
	sf::Color colors[4] = { sf::Color(25, 25, 45),
							sf::Color::Green,
							sf::Color::Yellow,
							sf::Color::Red 
	};

	sf::RectangleShape brick({ Brick_W - 2, Brick_H - 2 });

	for (int r = 0; r < ROWS; r++)
	{
		for (int c = 0; c < COLS; c++)
		{
			if (bricks[r][c] == 0)
				continue;

			brick.setFillColor(colors[bricks[r][c]]);
			brick.setPosition({ c * Brick_W + 1, Brick_Y + r * Brick_H + 1 });
			window.draw(brick);
		}
	}
}


void handleInput(sf::RenderWindow& window) {

	while (const std::optional event = window.pollEvent()){

		if (event->is<sf::Event::Closed>()) {
			window.close();
		}
	}
}

void updateGame() {

	// Update game logic here
}

void drawGame(sf::RenderWindow& window) {

	window.clear();
	drawBricks(window);
	window.display();
}

int main() {

	sf::RenderWindow window(sf::VideoMode({ 600, 700 }), "Block Buster");
	window.setFramerateLimit(60);

	if (!loadLevel())
	{
		cout << "level.txt not found" << endl;
		return 1;
	}


	while (window.isOpen()) {

		handleInput(window);
		updateGame();
		drawGame(window);
	}

	return 0;
}