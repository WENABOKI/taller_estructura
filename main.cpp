#include <SFML/Graphics.hpp>
#include <iostream>
using namespace std;

// Configuración del tablero
const int CELL_SIZE = 40;
const int BOARD_SIZE = 10;
const int BOARD_OFFSET_X = 100;
const int BOARD_OFFSET_Y = 100;

void dibujarTablero(sf::RenderWindow& window, sf::RectangleShape& celda, sf::Text& texto) {
    // Dibujar letras de columnas (A-J)
    for (int j = 0; j < BOARD_SIZE; j++) {
        texto.setString(string(1, 'A' + j));
        texto.setPosition(sf::Vector2f(BOARD_OFFSET_X + j * CELL_SIZE + CELL_SIZE/2 - 5, BOARD_OFFSET_Y - 30));
        window.draw(texto);
    }
    
    // Dibujar números de filas (1-10)
    for (int i = 0; i < BOARD_SIZE; i++) {
        texto.setString(to_string(i + 1));
        texto.setPosition(sf::Vector2f(BOARD_OFFSET_X - 30, BOARD_OFFSET_Y + i * CELL_SIZE + CELL_SIZE/2 - 8));
        window.draw(texto);
    }
    
    // Dibujar las celdas del tablero
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            celda.setPosition(sf::Vector2f(BOARD_OFFSET_X + j * CELL_SIZE, BOARD_OFFSET_Y + i * CELL_SIZE));
            window.draw(celda);
        }
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode({600u, 600u}), "Battleship - Mi Tablero");
    
    // Configurar la celda
    sf::RectangleShape celda;
    celda.setSize(sf::Vector2f(CELL_SIZE - 2, CELL_SIZE - 2));
    celda.setOutlineThickness(1);
    celda.setOutlineColor(sf::Color::Black);
    celda.setFillColor(sf::Color::Blue);
    
    // Crear fuente y texto
    sf::Font font;
    sf::Text texto(font);
    texto.setCharacterSize(16);
    texto.setFillColor(sf::Color::White);
    
    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }
        
        window.clear(sf::Color::Black);
        
        // Título
        texto.setString("MI TABLERO");
        texto.setPosition(sf::Vector2f(250, 20));
        window.draw(texto);
        
        // Dibujar el tablero
        dibujarTablero(window, celda, texto);
        
        window.display();
    }
    
    return 0;
}