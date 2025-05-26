#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <optional>
#include <cstdlib>
#include <ctime>
#include "jugador.cpp" 
using namespace std;

int CELL_SIZE = 40;
int SMALL_CELL_SIZE = 25;
int BOARD_SIZE = 10;
int BOARD_OFFSET_X = 100;
int BOARD_OFFSET_Y = 50;
int SMALL_BOARD_OFFSET_X = 100;
int SMALL_BOARD_OFFSET_Y = 500;

void dibujarTablero(sf::RenderWindow& window, sf::RectangleShape& celda, sf::Text& texto, 
                   char tablero[10][10], int offsetX, int offsetY, int cellSize, 
                   bool mostrarBarcos = true, char disparos[10][10] = nullptr) {
    
    for (int j = 0; j < BOARD_SIZE; j++) {
        texto.setString(string(1, 'A' + j));
        texto.setPosition(sf::Vector2f(offsetX + j * cellSize + cellSize/2 - 5, offsetY - 30));
        window.draw(texto);
    }
   
    for (int i = 0; i < BOARD_SIZE; i++) {
        texto.setString(to_string(i + 1));
        texto.setPosition(sf::Vector2f(offsetX - 30, offsetY + i * cellSize + cellSize/2 - 8));
        window.draw(texto);
    }
   
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            celda.setSize(sf::Vector2f(cellSize - 2, cellSize - 2));
            celda.setPosition(sf::Vector2f(offsetX + j * cellSize, offsetY + i * cellSize));
            
            if (disparos != nullptr && disparos[i][j] == 'X') {
                celda.setFillColor(sf::Color::Red);
            } else if (disparos != nullptr && disparos[i][j] == 'H') {
                celda.setFillColor(sf::Color::Yellow);
            } else if (tablero[i][j] == '~') {
                celda.setFillColor(sf::Color::Blue);  
            } else if (tablero[i][j] == 'B' && mostrarBarcos) {
                celda.setFillColor(sf::Color::Green); 
            } else {
                celda.setFillColor(sf::Color::Blue);  
            }
            
            window.draw(celda);
        }
    }
}

pair<int, int> obtenerCoordenadas(int mouseX, int mouseY, int offsetX, int offsetY, int cellSize) {
    int col = (mouseX - offsetX) / cellSize;
    int fila = (mouseY - offsetY) / cellSize;
    
    if (col >= 0 && col < BOARD_SIZE && fila >= 0 && fila < BOARD_SIZE) {
        return make_pair(fila, col);
    }
    return make_pair(-1, -1);
}

bool colocarBarcoAleatorio(char tablero[10][10], int tamano) {
    srand(time(0) + rand()); 
    
    for (int intentos = 0; intentos < 100; intentos++) {
        int fila = rand() % BOARD_SIZE;
        int col = rand() % BOARD_SIZE;
        bool horizontal = rand() % 2;
        
        bool puedeColocar = true;
        
        for (int i = 0; i < tamano; i++) {
            int f = horizontal ? fila : fila + i;
            int c = horizontal ? col + i : col;
            
            if (f >= BOARD_SIZE || c >= BOARD_SIZE || tablero[f][c] != '~') {
                puedeColocar = false;
                break;
            }
        }
        
        if (puedeColocar) {
            for (int i = 0; i < tamano; i++) {
                int f = horizontal ? fila : fila + i;
                int c = horizontal ? col + i : col;
                tablero[f][c] = 'B';
            }
            return true;
        }
    }
    return false;
}

void inicializarTableroEnemigo(char tablero[10][10], Jugador& enemigo) {
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            tablero[i][j] = '~';
        }
    }
    
    vector<int> tamanosBarcos = {4, 3, 2, 1};
    for (int tamano : tamanosBarcos) {
        bool colocado = false;
        for (int intentos = 0; intentos < 100 && !colocado; intentos++) {
            int fila = rand() % BOARD_SIZE;
            int col = rand() % BOARD_SIZE;
            bool horizontal = rand() % 2;
            
            if (enemigo.colocarBarco(fila, col, tamano, horizontal)) {
                for (int i = 0; i < tamano; i++) {
                    int f = horizontal ? fila : fila + i;
                    int c = horizontal ? col + i : col;
                    tablero[f][c] = 'B';
                }
                colocado = true;
            }
        }
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode({800u, 800u}), "Battleship - Juego Completo");
   
    sf::RectangleShape celda;
    celda.setOutlineThickness(1);
    celda.setOutlineColor(sf::Color::Black);
    
    sf::RectangleShape celdaPequena;
    celdaPequena.setOutlineThickness(1);
    celdaPequena.setOutlineColor(sf::Color::Black);
   
    sf::Font font;
    sf::Text texto(font);
    texto.setCharacterSize(16);
    texto.setFillColor(sf::Color::White);
    
    sf::Text textoTitulo(font);
    textoTitulo.setCharacterSize(20);
    textoTitulo.setFillColor(sf::Color::White);
    
    sf::Text textoVictoria(font);
    textoVictoria.setCharacterSize(30);
    textoVictoria.setFillColor(sf::Color::Green);
    
    Jugador jugador;
    Jugador enemigo;
    char tableroJugador[10][10];
    char tableroEnemigo[10][10];
    char disparosEnemigo[10][10]; 
    
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            tableroJugador[i][j] = '~';
            disparosEnemigo[i][j] = '~';
        }
    }
    
    srand(time(0));
    inicializarTableroEnemigo(tableroEnemigo, enemigo);
    
    vector<int> tamanosBarcos = {4, 3, 2, 1};
    int barcoActual = 0;
    bool horizontal = true;
    bool todosColocados = false;
    bool juegoTerminado = false;
    string mensaje = "";
    
    enum EstadoJuego { COLOCANDO_BARCOS, JUGANDO, JUEGO_TERMINADO };
    EstadoJuego estado = COLOCANDO_BARCOS;
   
    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            
            if (estado == COLOCANDO_BARCOS) {
                if (event->is<sf::Event::KeyPressed>() && !todosColocados) {
                    const auto& keyEvent = event->getIf<sf::Event::KeyPressed>();
                    if (keyEvent && keyEvent->code == sf::Keyboard::Key::Space) {
                        horizontal = !horizontal;
                    }
                }
                
                if (event->is<sf::Event::MouseButtonPressed>() && !todosColocados) {
                    const auto& mouseEvent = event->getIf<sf::Event::MouseButtonPressed>();
                    if (mouseEvent && mouseEvent->button == sf::Mouse::Button::Left) {
                        int mouseX = mouseEvent->position.x;
                        int mouseY = mouseEvent->position.y;
                        
                        pair<int, int> coords = obtenerCoordenadas(mouseX, mouseY, BOARD_OFFSET_X, BOARD_OFFSET_Y, CELL_SIZE);
                        if (coords.first != -1 && coords.second != -1) {
                            int fila = coords.first;
                            int col = coords.second;
                            
                            if (jugador.colocarBarco(fila, col, tamanosBarcos[barcoActual], horizontal)) {
                                for (int i = 0; i < tamanosBarcos[barcoActual]; i++) {
                                    int f = horizontal ? fila : fila + i;
                                    int c = horizontal ? col + i : col;
                                    tableroJugador[f][c] = 'B';
                                }
                                
                                barcoActual++;
                                if (barcoActual >= tamanosBarcos.size()) {
                                    todosColocados = true;
                                    estado = JUGANDO;
                                    mensaje = "¡Comienza el juego! Dispara al tablero enemigo";
                                } else {
                                    mensaje = "";
                                }
                            } else {
                                mensaje = "Posicion invalida!";
                            }
                        }
                    }
                }
            } else if (estado == JUGANDO) {
                if (event->is<sf::Event::MouseButtonPressed>()) {
                    const auto& mouseEvent = event->getIf<sf::Event::MouseButtonPressed>();
                    if (mouseEvent && mouseEvent->button == sf::Mouse::Button::Left) {
                        int mouseX = mouseEvent->position.x;
                        int mouseY = mouseEvent->position.y;
                        
                        pair<int, int> coords = obtenerCoordenadas(mouseX, mouseY, BOARD_OFFSET_X, BOARD_OFFSET_Y, CELL_SIZE);
                        if (coords.first != -1 && coords.second != -1) {
                            int fila = coords.first;
                            int col = coords.second;
                            
                            if (disparosEnemigo[fila][col] == '~') {
                                if (jugador.registrarDisparo(fila, col)) {
                                    bool impacto = enemigo.recibirDisparo(fila, col);
                                    
                                    if (impacto) {
                                        disparosEnemigo[fila][col] = 'H'; 
                                        mensaje = "¡Impacto! (" + string(1, 'A' + col) + to_string(fila + 1) + ")";
                                        
                                        if (enemigo.todosLosBarcosHundidos()) {
                                            estado = JUEGO_TERMINADO;
                                            juegoTerminado = true;
                                            mensaje = "¡FELICIDADES! ¡HAS GANADO! Todos los barcos enemigos han sido hundidos.";
                                        }
                                    } else {
                                        disparosEnemigo[fila][col] = 'X'; 
                                        mensaje = "Agua. (" + string(1, 'A' + col) + to_string(fila + 1) + ")";
                                    }
                                }
                            } else {
                                mensaje = "Ya disparaste ahi!";
                            }
                        }
                    }
                }
            } else if (estado == JUEGO_TERMINADO) {
                if (event->is<sf::Event::KeyPressed>()) {
                    const auto& keyEvent = event->getIf<sf::Event::KeyPressed>();
                    if (keyEvent && keyEvent->code == sf::Keyboard::Key::Escape) {
                        window.close();
                    }
                }
            }
        }
       
        window.clear(sf::Color::Black);
       
        if (estado == COLOCANDO_BARCOS) {
            
            textoTitulo.setString("COLOCA TUS BARCOS");
            textoTitulo.setPosition(sf::Vector2f(250, 10));
            window.draw(textoTitulo);
            
            if (!todosColocados) {
                string instruccion = "Coloca barco de " + to_string(tamanosBarcos[barcoActual]) + " casillas";
                texto.setString(instruccion);
                texto.setPosition(sf::Vector2f(50, 450));
                window.draw(texto);
                
                string orientacion = horizontal ? "Horizontal" : "Vertical";
                texto.setString("Orientacion: " + orientacion + " (SPACE para cambiar)");
                texto.setPosition(sf::Vector2f(50, 470));
                window.draw(texto);
            }
            
            dibujarTablero(window, celda, texto, tableroJugador, BOARD_OFFSET_X, BOARD_OFFSET_Y, CELL_SIZE);
            
        } else if (estado == JUGANDO) {
            textoTitulo.setString("TABLERO ENEMIGO");
            textoTitulo.setPosition(sf::Vector2f(200, 10));
            window.draw(textoTitulo);
            
            textoTitulo.setString("TU TABLERO");
            textoTitulo.setPosition(sf::Vector2f(150, 460));
            window.draw(textoTitulo);
            
            dibujarTablero(window, celda, texto, tableroEnemigo, BOARD_OFFSET_X, BOARD_OFFSET_Y, CELL_SIZE, false, disparosEnemigo);
            
            dibujarTablero(window, celdaPequena, texto, tableroJugador, SMALL_BOARD_OFFSET_X, SMALL_BOARD_OFFSET_Y, SMALL_CELL_SIZE);
            
        } else if (estado == JUEGO_TERMINADO) {
            textoVictoria.setString("¡VICTORIA!");
            textoVictoria.setPosition(sf::Vector2f(300, 200));
            window.draw(textoVictoria);
            
            texto.setString("Has hundido todos los barcos enemigos");
            texto.setPosition(sf::Vector2f(250, 250));
            window.draw(texto);
            
            texto.setString("Presiona ESC para salir");
            texto.setPosition(sf::Vector2f(300, 300));
            window.draw(texto);
            
            textoTitulo.setString("TABLERO ENEMIGO COMPLETO");
            textoTitulo.setPosition(sf::Vector2f(180, 350));
            window.draw(textoTitulo);
            
            dibujarTablero(window, celdaPequena, texto, tableroEnemigo, SMALL_BOARD_OFFSET_X, 380, SMALL_CELL_SIZE, true, disparosEnemigo);
        }
        
        if (!mensaje.empty() && estado != JUEGO_TERMINADO) {
            texto.setString(mensaje);
            texto.setPosition(sf::Vector2f(50, 750));
            window.draw(texto);
        }
       
        window.display();
    }
   
    return 0;
}