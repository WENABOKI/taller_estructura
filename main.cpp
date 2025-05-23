#include <SFML/Graphics.hpp>
#include <optional>

int main() {
    sf::RenderWindow window(sf::VideoMode(800, 600), "Battleship");

    while (window.isOpen()) {
        std::optional<sf::Event> event;
        while ((event = window.pollEvent())) {
            if (event->type == sf::Event::Closed) {
                window.close();
            }
        }

        window.clear();
        // Aquí puedes dibujar cosas
        window.display();
    }

    return 0;
}
