#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <string>
using namespace std;

const int ROWS = 8;
const int COLS = 10;
int bricks[ROWS][COLS];
bool touched[ROWS][COLS];

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

int score = 0;
const int Points_Hit = 10;
const int Points_Break = 50;

const int Start_Lives = 3;
int lives = Start_Lives;
sf::Font font;

const int MENU = 0, PLAYING = 1, GAME_OVER = 2;
int gameState = MENU;
int menuChoice = 0;


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

void drawHud(sf::RenderWindow& window)
{
	sf::Text scoreText(font, "Score: " + to_string(score), 22);
	scoreText.setFillColor(sf::Color::White);
	scoreText.setPosition({ 20.f, 10.f });
	window.draw(scoreText);

	sf::Text livesText(font, "Lives: " + to_string(lives), 22);
	livesText.setFillColor(sf::Color::White);
	livesText.setPosition({ 460.f, 10.f });
	window.draw(livesText);
}

void drawText(sf::RenderWindow& window, string msg, float x, float y, int size, sf::Color color)
{
	sf::Text text(font, msg, size);
	text.setFillColor(color);
	text.setPosition({ x, y });
	window.draw(text);
}

void drawBall(sf::RenderWindow& window) {

	sf::CircleShape ball(Ball_Size / 2);
	ball.setFillColor(sf::Color::White);
	ball.setPosition({ (float)ballX, (float)ballY });
	window.draw(ball);
}

void drawMenu(sf::RenderWindow& window)
{
	drawText(window, "BLOCK BUSTER", 150, 150, 40, sf::Color::White);

	string options[3] = { "New Game", "Continue", "Exit" };
	for (int i = 0; i < 3; i++)
	{
		sf::Color color = sf::Color::White;
		if (i == menuChoice)
			color = sf::Color::Yellow;
		drawText(window, options[i], 220, 300 + i * 60, 28, color);
	}
}

void handleInput(sf::RenderWindow& window) {

	while (const std::optional event = window.pollEvent()) {

		if (event->is<sf::Event::Closed>()) {
			window.close();
		}
		else if (const auto* key = event->getIf<sf::Event::KeyPressed>())
		{
			if (gameState == MENU)
			{
				if (key->code == sf::Keyboard::Key::Down && menuChoice < 2)
					menuChoice++;

				if (key->code == sf::Keyboard::Key::Up && menuChoice > 0)
					menuChoice--;

				if (key->code == sf::Keyboard::Key::Enter)
				{
					if (menuChoice == 0)
						gameState = PLAYING;
					else if (menuChoice == 2)
						window.close();
				}
			}
			else if (gameState == PLAYING)
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
	}

	if (gameState == PLAYING)
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) ||
			sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
			PaddleX = PaddleX - 8;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) ||
			sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
			PaddleX = PaddleX + 8;

		if (PaddleX < 0)
			PaddleX = 0;

		if (PaddleX + (int)Paddle_W > Window_W)
			PaddleX = Window_W - (int)Paddle_W;
	}
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

void markBrickAt(int x, int y)
{
	if (x < 0 || x >= Window_W || y < (int)Brick_Y || y >= (int)Brick_Y + ROWS * (int)Brick_H)
		return;

	int c = x / (int)Brick_W;
	int r = (y - (int)Brick_Y) / (int)Brick_H;

	if (bricks[r][c] > 0)
		touched[r][c] = true;
}

bool hitBricks()
{
	for (int r = 0; r < ROWS; r++)
		for (int c = 0; c < COLS; c++)
			touched[r][c] = false;

	int last = (int)Ball_Size - 1;

	markBrickAt(ballX, ballY);
	markBrickAt(ballX + last, ballY);
	markBrickAt(ballX, ballY + last);
	markBrickAt(ballX + last, ballY + last);

	bool hit = false;
	for (int r = 0; r < ROWS; r++)
		for (int c = 0; c < COLS; c++)
			if (touched[r][c])
			{
				bricks[r][c]--;

				if (bricks[r][c] == 0)
					score += Points_Break;
				else
					score += Points_Hit;

				hit = true;
			}

	return hit;
}


void updateGame() {

	if (gameState != PLAYING)
		return;

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
		else if (hitBricks())
		{
			ballX = ballX - dx;
			dx = -dx;
		}

		ballY = ballY + dy;
		if (ballY < Hud_H)
		{
			ballY = ballY - dy;
			dy = -dy;
		}
		else if (hitBricks())
		{
			ballY = ballY - dy;
			dy = -dy;
		}

		bounceOnPaddle();

		if (ballY > 700)
		{
			lives--;
			ballLaunched = false;

			if (lives == 0)
				cout << "GAME OVER" << endl;
		}
	}
	
}

void drawGame(sf::RenderWindow& window) {

	window.clear();

	switch (gameState)
	{
	case MENU:
		drawMenu(window);
		break;
	case PLAYING:
		drawBricks(window);
		drawPaddle(window);
		drawBall(window);
		drawHud(window);
		break;
	case GAME_OVER:
		break;
	}

	window.display();
}

int main() {

	sf::RenderWindow window(sf::VideoMode({ Window_W, Window_H }), "Block Buster");
	window.setFramerateLimit(60);

	if (!font.openFromFile("C:/Windows/Fonts/arial.ttf"))
	{
		cout << "font not found" << endl;
		return 1;
	}

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