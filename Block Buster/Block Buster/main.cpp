#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
using namespace std;

const int ROWS = 8;
const int COLS = 10;
int bricks[ROWS][COLS];

const int Window_W = 600;
const int Window_H = 700;

const float Brick_W = 60.f;
const float Brick_H = 25.f;
const float Brick_Y = 100.f;

const float Paddle_W = 100.f;
const float Paddle_H = 14.f;
const float	Paddle_Y = 650.f;
int  PaddleX = 250;

const float Ball_Size = 12.f;
int ballX = 0;
int ballY = 0;
bool ballLaunched = false;

int dx = 0;
int dy = 0;
const int Hud_H = 50;

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

void drawPaddle(sf::RenderWindow& window) {

	sf::RectangleShape paddle({ Paddle_W, Paddle_H });
	paddle.setFillColor(sf::Color::White);
	paddle.setPosition({(float)PaddleX, Paddle_Y});
	window.draw(paddle);
}

void drawBall(sf::RenderWindow& window) {

	sf::CircleShape ball(Ball_Size / 2);
	ball.setFillColor(sf::Color::White);
	ball.setPosition({ (float)ballX, (float)ballY });
	window.draw(ball);
}


void handleInput(sf::RenderWindow& window) {

	while (const std::optional event = window.pollEvent()){

		if (event->is<sf::Event::Closed>()) {
			window.close();
		}
		else if (const auto* key = event->getIf<sf::Event::KeyPressed>())
		{
			if (key->code == sf::Keyboard::Key::Space && !ballLaunched)
			{
				ballLaunched = true;
				dy = -5;
				if (rand() % 2 == 0)
					dx = 3;
				else
					dx = -3;
			}
		}
	}

	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
		PaddleX = PaddleX - 8;
	if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
		PaddleX = PaddleX + 8;

	if (PaddleX < 0)
		PaddleX = 0;

	if (PaddleX + (int)Paddle_W > Window_W)
		PaddleX = Window_W - (int)Paddle_W;
}

void bounceOnPaddle()
{
	bool touching = ballX < PaddleX + (int)Paddle_W &&
		ballX + (int)Ball_Size > PaddleX &&
		ballY < (int)Paddle_Y + (int)Paddle_H &&
		ballY + (int)Ball_Size >(int)Paddle_Y;

	if (touching && dy > 0)
	{
		ballY = 638;
		dy = -5;

		int centre = ballX + (int)Ball_Size / 2;
		int part = (centre - PaddleX) / ((int)Paddle_W / 4);

		if (part < 0) part = 0;
		if (part > 3) part = 3;

		int speeds[4] = { -6, -3, 3, 6 };
		dx = speeds[part];
	}
}

void updateGame() {

	if (!ballLaunched)
	{
		ballX = PaddleX + 44;
		ballY = 638;
	}
	else
	{
		ballX = ballX + dx;
		if (ballX < 0 || ballX + (int)Ball_Size > Window_W) 
		{
			ballX = ballX - dx;
			dx = -dx;
		}

		ballY = ballY + dy;
		if(ballY < Hud_H)
		{
			ballY = ballY - dy;
			dy = -dy;
		}

		bounceOnPaddle();

		if (ballY > 700)
		{
			ballLaunched = false;
		}
	}
	
}

void drawGame(sf::RenderWindow& window) {

	window.clear();
	drawBricks(window);
	drawPaddle(window);
	drawBall(window);
	window.display();
}

int main() {

	sf::RenderWindow window(sf::VideoMode({ Window_W, Window_H }), "Block Buster");
	window.setFramerateLimit(60);

	srand((unsigned)time(0));

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