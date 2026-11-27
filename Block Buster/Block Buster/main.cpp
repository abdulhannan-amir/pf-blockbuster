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
const int Paddle_Start_X = 250;
const int Paddle_Speed = 8;
int  PaddleX = Paddle_Start_X;

const float Ball_Size = 12.f;
const int Ball_Offset_X = 44;
const int Ball_Start_Y = 638;
const int Ball_Speed_Y = 5;
const int Ball_Speed_X = 3;
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
bool won = false;
bool noSave = false;
bool levelMissing = false;


// Reads level.txt into the bricks table.
// The first line (rows and columns) is read and skipped.
// Then every brick number is read row by row with two nested loops.
// Returns false if the file is missing, so the game can show an error.
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
	brick.setOutlineThickness(-1.f);
	brick.setOutlineColor(sf::Color(0, 0, 0, 120));

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
	sf::RectangleShape strip({ (float)Window_W, (float)Hud_H });
	strip.setFillColor(sf::Color(30, 30, 70));
	window.draw(strip);

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

void drawCenteredText(sf::RenderWindow& window, string msg, float y, int size, sf::Color color)
{
	sf::Text text(font, msg, size);
	text.setFillColor(color);
	sf::FloatRect box = text.getLocalBounds();
	text.setPosition({ (Window_W - box.size.x) / 2.f - box.position.x, y });
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
	drawCenteredText(window, "BLOCK BUSTER", 120, 52, sf::Color(255, 200, 0));
	drawCenteredText(window, "Break all the bricks!", 190, 20, sf::Color(150, 150, 180));

	string options[3] = { "New Game", "Continue", "Exit" };
	for (int i = 0; i < 3; i++)
	{
		float y = 300.f + i * 70;
		if (i == menuChoice)
		{
			sf::RectangleShape bar({ 260.f, 50.f });
			bar.setFillColor(sf::Color(40, 40, 90));
			bar.setOutlineThickness(2.f);
			bar.setOutlineColor(sf::Color(255, 200, 0));
			bar.setPosition({ (Window_W - 260) / 2.f, y - 6.f });
			window.draw(bar);
			drawCenteredText(window, options[i], y, 28, sf::Color(255, 200, 0));
		}
		else
			drawCenteredText(window, options[i], y, 28, sf::Color::White);
	}

	if (noSave)
		drawCenteredText(window, "No saved game", 540, 22, sf::Color::Red);

	drawCenteredText(window, "Up / Down + Enter", 650, 18, sf::Color(120, 120, 150));
}
void startNewGame()
{
	score = 0;
	lives = Start_Lives;
	loadLevel();
	PaddleX = Paddle_Start_X;
	ballLaunched = false;
	won = false;
}

// Writes the current game to savegame.txt.
// Line 1: score and lives. Next 8 lines: the bricks table, one row per line.
void saveGame()
{
	ofstream fout("savegame.txt");

	fout << score << " " << lives << endl;

	for (int r = 0; r < ROWS; r++)
	{
		for (int c = 0; c < COLS; c++)
			fout << bricks[r][c] << " ";
		fout << endl;
	}
}

// Reads savegame.txt: score and lives on the first line, then the bricks table.
// Returns false if the file is missing or empty (no saved game).
// The paddle and ball are not saved, so they start at their default place.
bool loadGame()
{
	ifstream fin("savegame.txt");
	if (!fin)
		return false;

	int s, l;
	if (!(fin >> s >> l))
		return false;

	score = s;
	lives = l;

	for (int r = 0; r < ROWS; r++)
		for (int c = 0; c < COLS; c++)
			fin >> bricks[r][c];

	PaddleX = Paddle_Start_X;
	ballLaunched = false;
	won = false;
	return true;
}

// Opening the file for writing and closing it at once makes it empty,
// so a finished game cannot be continued.
void clearSave()
{
	ofstream fout("savegame.txt");
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
				if (key->code == sf::Keyboard::Key::Down && menuChoice < 2) {
					menuChoice++;
					noSave = false;
				}

				if (key->code == sf::Keyboard::Key::Up && menuChoice > 0) {
					menuChoice--;
					noSave = false;
				}

				if (key->code == sf::Keyboard::Key::Enter)
				{
					if (menuChoice == 0)
					{
						startNewGame();
						gameState = PLAYING;
					}
					else if (menuChoice == 1)
					{
						if (loadGame())
							gameState = PLAYING;
						else
							noSave = true;
					}
					else if (menuChoice == 2)
						window.close();
				}
			}
			else if (gameState == PLAYING)
			{
				if (key->code == sf::Keyboard::Key::Space && !ballLaunched)
				{
					ballLaunched = true;
					dy = -Ball_Speed_Y;
					if (rand() % 2 == 0)
						dx = Ball_Speed_X;
					else
						dx = -Ball_Speed_X;
				}
				if (key->code == sf::Keyboard::Key::Escape)
				{
					saveGame();
					gameState = MENU;
				}
			}
			else if (gameState == GAME_OVER)
			{
				if (key->code == sf::Keyboard::Key::Enter)
					gameState = MENU;
			}
		}
	}

	if (gameState == PLAYING)
	{
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) ||
			sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
			PaddleX = PaddleX - Paddle_Speed;

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) ||
			sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
			PaddleX = PaddleX + Paddle_Speed;

		if (PaddleX < 0)
			PaddleX = 0;

		if (PaddleX + (int)Paddle_W > Window_W)
			PaddleX = Window_W - (int)Paddle_W;
	}
}

// The ball bounces only while moving down (dy > 0) and when its box
// touches the paddle box. The paddle is split into 4 equal parts, and the
// part under the ball's centre decides the new dx: far left parts send the
// ball left, far right parts send it right. This is how the player aims.
void bounceOnPaddle()
{
	bool touching = ballX < PaddleX + (int)Paddle_W &&
		ballX + (int)Ball_Size > PaddleX &&
		ballY < (int)Paddle_Y + (int)Paddle_H &&
		ballY + (int)Ball_Size >(int)Paddle_Y;

	if (touching && dy > 0)
	{
		ballY = Ball_Start_Y;
		dy = -Ball_Speed_Y;

		int centre = ballX + (int)Ball_Size / 2;
		int part = (centre - PaddleX) / ((int)Paddle_W / 4);

		if (part < 0) part = 0;
		if (part > 3) part = 3;

		int speeds[4] = { -6, -3, 3, 6 };
		dx = speeds[part];
	}
}

// Finds the brick under the point (x, y) with one division per direction:
// column = x / brick width, row = (y - top of bricks) / brick height.
// Points outside the brick area are ignored to avoid a wrong row or column.
// The brick is only marked here, not damaged, so one brick under two
// corners of the ball loses only one hit.
void markBrickAt(int x, int y)
{
	if (x < 0 || x >= Window_W || y < (int)Brick_Y || y >= (int)Brick_Y + ROWS * (int)Brick_H)
		return;

	int c = x / (int)Brick_W;
	int r = (y - (int)Brick_Y) / (int)Brick_H;

	if (bricks[r][c] > 0)
		touched[r][c] = true;
}

// Checks the four corners of the ball. Every marked brick loses one hit.
// A hit gives 10 points, and the hit that breaks the brick gives 50.
// Returns true if any brick was hit.
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

int countBricks()
{
	int count = 0;
	for (int r = 0; r < ROWS; r++)
		for (int c = 0; c < COLS; c++)
			if (bricks[r][c] > 0)
				count++;
	return count;
}

void updateGame() {

	if (levelMissing)
		return;
	if (gameState != PLAYING)
		return;

	if (!ballLaunched)
	{
		ballX = PaddleX + Ball_Offset_X;
		ballY = Ball_Start_Y;
	}

	// Move in two steps: first x, then y. After each step, if the ball
	// went into a wall or a brick, undo that step and reverse that speed.
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

		if (ballY > Window_H)
		{
			lives--;
			ballLaunched = false;

			if (lives == 0)
			{
				won = false;
				clearSave();
				gameState = GAME_OVER;
			}
		}

		if (countBricks() == 0)
		{
			won = true;
			clearSave();
			gameState = GAME_OVER;
		}
	}
	
}

void drawGame(sf::RenderWindow& window) {

	window.clear(sf::Color(15, 15, 35));
	if (levelMissing)
	{
		drawText(window, "ERROR: level.txt not found", 80, 320, 28, sf::Color::Red);
		window.display();
		return;
	}

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
		sf::Color accent = won ? sf::Color(60, 220, 100) : sf::Color(230, 60, 60);

		sf::RectangleShape box({ 420.f, 300.f });
		box.setFillColor(sf::Color(30, 30, 70));
		box.setOutlineThickness(3.f);
		box.setOutlineColor(accent);
		box.setPosition({ (Window_W - 420) / 2.f, 200.f });
		window.draw(box);

		drawCenteredText(window, won ? "YOU WIN" : "GAME OVER", 240, 52, accent);
		drawCenteredText(window, "Final Score: " + to_string(score), 340, 30, sf::Color::White);
		drawCenteredText(window, "Press Enter", 420, 24, sf::Color(255, 200, 0));
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
		levelMissing = true;


	while (window.isOpen()) {

		handleInput(window);
		updateGame();
		drawGame(window);
	}

	return 0;
}