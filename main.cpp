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
int BOARD_OFFSET_Y = 80;
int SMALL_BOARD_OFFSET_X = 100;
int SMALL_BOARD_OFFSET_Y = 520;
int SHIP_SELECTION_X = 600;
int SHIP_SELECTION_Y = 100;

char disparosRealizados[10][10];

struct BarcoSeleccionable {
    int tamano;
    bool colocado;
    sf::RectangleShape rectangulo;
    sf::Text texto;
    
    BarcoSeleccionable(int t, const sf::Font& font) : tamano(t), colocado(false), texto(font) {
        texto.setCharacterSize(16);
        texto.setFillColor(sf::Color::White);
        rectangulo.setOutlineThickness(2);
    }
};

void dibujarTablero(sf::RenderWindow& window, sf::RectangleShape& celda, sf::Text& texto, 
                   char tablero[10][10], int offsetX, int offsetY, int cellSize, 
                   bool mostrarBarcos = true, char* disparosArray = nullptr) {
    
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
            
            if (disparosArray != nullptr) {
                char* disparos2D = (char*)disparosArray;
                char estadoDisparo = disparos2D[i * 10 + j];
                
                if (estadoDisparo == 'H') {
                    celda.setFillColor(sf::Color::Red);  
                } else if (estadoDisparo == 'X') {
                    celda.setFillColor(sf::Color::Yellow); 
                } else if (tablero[i][j] == 'B' && mostrarBarcos) {
                    celda.setFillColor(sf::Color::Green); 
                } else {
                    celda.setFillColor(sf::Color::Blue);  
                }
            } else {
                if (tablero[i][j] == 'B' && mostrarBarcos) {
                    celda.setFillColor(sf::Color::Green);  
                } else if (tablero[i][j] == 'X') {
                    celda.setFillColor(sf::Color::Red);   
                } else if (tablero[i][j] == 'O') {
                    celda.setFillColor(sf::Color::Yellow); 
                } else {
                    celda.setFillColor(sf::Color::Blue);   
                }
            }
            
            window.draw(celda);
        }
    }
}

void dibujarSeleccionBarcos(sf::RenderWindow& window, vector<BarcoSeleccionable>& barcos, int barcoSeleccionado, const sf::Font& font) {
    sf::Text titulo(font);
    titulo.setCharacterSize(18);
    titulo.setFillColor(sf::Color::White);
    titulo.setString("SELECCIONA BARCO:");
    titulo.setPosition(sf::Vector2f(SHIP_SELECTION_X, SHIP_SELECTION_Y - 30));
    window.draw(titulo);
    
    for (size_t i = 0; i < barcos.size(); i++) {
        int yPos = SHIP_SELECTION_Y + i * 60;
        
        barcos[i].rectangulo.setSize(sf::Vector2f(barcos[i].tamano * 25, 25));
        barcos[i].rectangulo.setPosition(sf::Vector2f(SHIP_SELECTION_X, yPos));
        
        if (barcos[i].colocado) {
            barcos[i].rectangulo.setFillColor(sf::Color(100, 100, 100));
            barcos[i].rectangulo.setOutlineColor(sf::Color::Black);
        } else if (i == static_cast<size_t>(barcoSeleccionado)) {
            barcos[i].rectangulo.setFillColor(sf::Color::Yellow);
            barcos[i].rectangulo.setOutlineColor(sf::Color::White);
        } else {
            barcos[i].rectangulo.setFillColor(sf::Color::Green);
            barcos[i].rectangulo.setOutlineColor(sf::Color::White);
        }
        
        window.draw(barcos[i].rectangulo);
        
        string textoBarco = "Barco " + to_string(barcos[i].tamano) + " casillas";
        
        barcos[i].texto.setString(textoBarco);
        barcos[i].texto.setPosition(sf::Vector2f(SHIP_SELECTION_X, yPos + 30));
        
        if (barcos[i].colocado) {
            barcos[i].texto.setFillColor(sf::Color(150, 150, 150));
        } else {
            barcos[i].texto.setFillColor(sf::Color::White);
        }
        
        window.draw(barcos[i].texto);
    }
}

int verificarClickEnBarco(float mouseX, float mouseY, vector<BarcoSeleccionable>& barcos) {
    for (size_t i = 0; i < barcos.size(); i++) {
        if (!barcos[i].colocado) {
            sf::FloatRect bounds = barcos[i].rectangulo.getGlobalBounds();
            if (bounds.contains({mouseX, mouseY})) {
                return static_cast<int>(i);
            }
        }
    }
    return -1;
}

pair<int, int> obtenerCoordenadas(int mouseX, int mouseY, int offsetX, int offsetY, int cellSize) {
    int col = (mouseX - offsetX) / cellSize;
    int fila = (mouseY - offsetY) / cellSize;
    
    if (col >= 0 && col < BOARD_SIZE && fila >= 0 && fila < BOARD_SIZE) {
        return make_pair(fila, col);
    }
    return make_pair(-1, -1);
}

void inicializarTableroEnemigo(Jugador& enemigo, char tablero[10][10]) {
    srand(time(0));
    
    vector<int> tamanosBarcos = {4, 3, 2, 2};
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
    sf::RenderWindow window(sf::VideoMode({900u, 850u}), "Battleship - Juego Completo");
   
    sf::RectangleShape celda;
    celda.setOutlineThickness(1);
    celda.setOutlineColor(sf::Color::Black);
    
    sf::RectangleShape celdaPequena;
    celdaPequena.setOutlineThickness(1);
    celdaPequena.setOutlineColor(sf::Color::Black);
   
    sf::Font font;
    if (!font.openFromFile("ARIAL.TTF")) {
        cout << "Error: No se pudo cargar la fuente ARIAL.TTF" << endl;
        return -1; 
    }
    
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
    
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            tableroJugador[i][j] = '~';
            tableroEnemigo[i][j] = '~';
            disparosRealizados[i][j] = '~';
        }
    }
    
    inicializarTableroEnemigo(enemigo, tableroEnemigo);
    
    vector<int> tamanosBarcos = {4, 3, 2, 2};
    vector<BarcoSeleccionable> barcosSeleccionables;
    for (int tamano : tamanosBarcos) {
        barcosSeleccionables.emplace_back(tamano, font);
    }
    
    int barcoSeleccionado = 0;
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
                        float mouseX = static_cast<float>(mouseEvent->position.x);
                        float mouseY = static_cast<float>(mouseEvent->position.y);
                        
                        int barcoClicado = verificarClickEnBarco(mouseX, mouseY, barcosSeleccionables);
                        if (barcoClicado != -1) {
                            barcoSeleccionado = barcoClicado;
                            mensaje = "Barco de " + to_string(barcosSeleccionables[barcoSeleccionado].tamano) + " casillas seleccionado";
                        } else {
                            pair<int, int> coords = obtenerCoordenadas(mouseEvent->position.x, mouseEvent->position.y, BOARD_OFFSET_X, BOARD_OFFSET_Y, CELL_SIZE);
                            if (coords.first != -1 && coords.second != -1) {
                                int fila = coords.first;
                                int col = coords.second;
                                
                                if (!barcosSeleccionables[barcoSeleccionado].colocado) {
                                    int tamanoBarco = barcosSeleccionables[barcoSeleccionado].tamano;
                                    
                                    if (jugador.colocarBarco(fila, col, tamanoBarco, horizontal)) {
                                        for (int i = 0; i < tamanoBarco; i++) {
                                            int f = horizontal ? fila : fila + i;
                                            int c = horizontal ? col + i : col;
                                            tableroJugador[f][c] = 'B';
                                        }
                                        
                                        barcosSeleccionables[barcoSeleccionado].colocado = true;
                                        
                                        bool todosBarcosColocados = true;
                                        for (const auto& barco : barcosSeleccionables) {
                                            if (!barco.colocado) {
                                                todosBarcosColocados = false;
                                                break;
                                            }
                                        }
                                        
                                        if (todosBarcosColocados) {
                                            todosColocados = true;
                                            estado = JUGANDO;
                                        } else {
                                            for (size_t i = 0; i < barcosSeleccionables.size(); i++) {
                                                if (!barcosSeleccionables[i].colocado) {
                                                    barcoSeleccionado = static_cast<int>(i);
                                                    break;
                                                }
                                            }
                                        }
                                    }
                                }
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
                            
                            if (disparosRealizados[fila][col] == '~') {
                                if (jugador.registrarDisparo(fila, col)) {
                                    bool impacto = enemigo.recibirDisparo(fila, col);
                                    
                                    if (impacto) {
                                        disparosRealizados[fila][col] = 'H'; 
                                        
                                        if (enemigo.todosLosBarcosHundidos()) {
                                            estado = JUEGO_TERMINADO;
                                            juegoTerminado = true;
                                        }
                                    } else {
                                        disparosRealizados[fila][col] = 'X'; 
                                    }
                                }
                            } else {
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
            textoTitulo.setPosition(sf::Vector2f(250, 15));
            window.draw(textoTitulo);
            
            dibujarTablero(window, celda, texto, tableroJugador, BOARD_OFFSET_X, BOARD_OFFSET_Y, CELL_SIZE, true);
            
            dibujarSeleccionBarcos(window, barcosSeleccionables, barcoSeleccionado, font);
            
            if (!todosColocados) {
                string orientacion = horizontal ? "Horizontal" : "Vertical";
                texto.setString("Orientacion: " + orientacion + " (SPACE para cambiar)");
                texto.setPosition(sf::Vector2f(100, 490));
                window.draw(texto);
            }
            
        } else if (estado == JUGANDO) {
            textoTitulo.setString("TABLERO ENEMIGO - HAZ TUS DISPAROS");
            textoTitulo.setPosition(sf::Vector2f(150, 15));
            window.draw(textoTitulo);
            
            dibujarTablero(window, celda, texto, tableroEnemigo, BOARD_OFFSET_X, BOARD_OFFSET_Y, CELL_SIZE, false, (char*)disparosRealizados);
            
            dibujarTablero(window, celdaPequena, texto, tableroJugador, SMALL_BOARD_OFFSET_X, SMALL_BOARD_OFFSET_Y, SMALL_CELL_SIZE, true);
            
        } else if (estado == JUEGO_TERMINADO) {
            textoVictoria.setString("VICTORIA");
            textoVictoria.setPosition(sf::Vector2f(300, 200));
            window.draw(textoVictoria);
            
            texto.setString("Has hundido todos los barcos enemigos");
            texto.setPosition(sf::Vector2f(250, 250));
            window.draw(texto);
            
            texto.setString("Presiona ESC para salir");
            texto.setPosition(sf::Vector2f(300, 300));
            window.draw(texto);
        }
        
        if (!mensaje.empty() && estado != JUEGO_TERMINADO) {
            texto.setString(mensaje);
            texto.setPosition(sf::Vector2f(50, 780));
            window.draw(texto);
        }
       
        window.display();
    }
   
    return 0;
    
}