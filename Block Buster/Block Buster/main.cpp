#include <SFML/Graphics.hpp>

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
	// Draw game objects here
	window.display();
}

int main() {

	sf::RenderWindow window(sf::VideoMode({ 600, 700 }), "Block Buster");
	window.setFramerateLimit(60);

	while (window.isOpen()) {

		handleInput(window);
		updateGame();
		drawGame(window);
	}

	return 0;
}