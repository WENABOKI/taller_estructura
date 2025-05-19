#include <SFML/Graphics.hpp>
#include "Jugador.cpp"

int tamCelda = 40;
int filas = 10;
int columnas = 10;

int main(){

    sf::RenderWindow ventana(sf::VideoMode(800, 600), "Coloca tus barcos");
        while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color::Black);
        window.display();
    }

    return 0;

}