#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <optional>
#include <cstdlib>
#include <ctime>
#include "jugador.cpp"
#include "ia.cpp"
#include "estructuras.h"

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

char disparosJugador[10][10];  
char disparosIA[10][10];      

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

struct BotonMenu {
    sf::RectangleShape rectangulo;
    sf::Text texto;
    
    BotonMenu(const sf::Font& font, const string& textoBoton, float x, float y, float ancho, float alto) : texto(font) {
        rectangulo.setSize(sf::Vector2f(ancho, alto));
        rectangulo.setPosition(sf::Vector2f(x, y));
        rectangulo.setFillColor(sf::Color(100, 100, 100));
        rectangulo.setOutlineThickness(3);
        rectangulo.setOutlineColor(sf::Color::White);
        
        texto.setString(textoBoton);
        texto.setCharacterSize(24);
        texto.setFillColor(sf::Color::White);
        
        sf::FloatRect textBounds = texto.getLocalBounds();
        texto.setPosition(sf::Vector2f(
            x + (ancho - textBounds.size.x) / 2,
            y + (alto - textBounds.size.y) / 2 - 5
        ));
    }
    
    bool contienePunto(float x, float y) {
        return rectangulo.getGlobalBounds().contains(sf::Vector2f(x, y));
    }
    
    void setResaltado(bool resaltado) {
        if (resaltado) {
            rectangulo.setFillColor(sf::Color(150, 150, 150));
            rectangulo.setOutlineColor(sf::Color::Yellow);
        } else {
            rectangulo.setFillColor(sf::Color(100, 100, 100));
            rectangulo.setOutlineColor(sf::Color::White);
        }
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

EstadoJuego crearEstadoParaIA(char tableroJugador[10][10], char tableroEnemigo[10][10], 
                              char disparosJugador[10][10], char disparosIA[10][10], bool turnoJugador) {
    EstadoJuego estado;
    
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            estado.tableroJugador[i][j] = tableroJugador[i][j];
            estado.tableroIA[i][j] = tableroEnemigo[i][j];
            estado.disparosJugador[i][j] = disparosJugador[i][j];
            estado.disparosIA[i][j] = disparosIA[i][j];
        }
    }
    
    estado.turnoJugador = turnoJugador;
    estado.puntajeJugador = 0;  
    estado.puntajeIA = 0;       
    return estado;
}

int main() {
    sf::RenderWindow window(sf::VideoMode({900u, 850u}), "Battleship - vs IA");
   
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
    textoTitulo.setCharacterSize(36);
    textoTitulo.setFillColor(sf::Color::Cyan);
    
    sf::Text textoTurno(font);
    textoTurno.setCharacterSize(18);
    textoTurno.setFillColor(sf::Color::Cyan);
    
    sf::Text textoVictoria(font);
    textoVictoria.setCharacterSize(30);
    textoVictoria.setFillColor(sf::Color::Green);
    
    BotonMenu botonIniciar(font, "INICIAR JUEGO", 300, 350, 300, 60);
    BotonMenu botonSalir(font, "SALIR", 300, 450, 300, 60);
    
    Jugador jugador;
    Jugador enemigo;
    IABattleship ia; 
    
    char tableroJugador[10][10];
    char tableroEnemigo[10][10];
    
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            tableroJugador[i][j] = '~';
            tableroEnemigo[i][j] = '~';
            disparosJugador[i][j] = '~';
            disparosIA[i][j] = '~';
        }
    }
    
    vector<int> tamanosBarcos = {4, 3, 2, 2};
    vector<BarcoSeleccionable> barcosSeleccionables;
    for (int tamano : tamanosBarcos) {
        barcosSeleccionables.emplace_back(tamano, font);
    }
    
    int barcoSeleccionado = 0;
    bool horizontal = true;
    bool todosColocados = false;
    bool juegoTerminado = false;
    bool turnoJugador = true;
    bool esperandoTurnoIA = false;
    sf::Clock clockIA;
    
    string mensaje = "";
    string ganador = "";
    
    enum EstadoDelJuego { MENU_PRINCIPAL, COLOCANDO_BARCOS, JUGANDO, JUEGO_TERMINADO };
    EstadoDelJuego estado = MENU_PRINCIPAL;
   
    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            
            if (estado == MENU_PRINCIPAL) {
                if (event->is<sf::Event::MouseButtonPressed>()) {
                    const auto& mouseEvent = event->getIf<sf::Event::MouseButtonPressed>();
                    if (mouseEvent && mouseEvent->button == sf::Mouse::Button::Left) {
                        float mouseX = static_cast<float>(mouseEvent->position.x);
                        float mouseY = static_cast<float>(mouseEvent->position.y);
                        
                        if (botonIniciar.contienePunto(mouseX, mouseY)) {
                            jugador = Jugador();
                            enemigo = Jugador();
                            
                            for (int i = 0; i < 10; i++) {
                                for (int j = 0; j < 10; j++) {
                                    tableroJugador[i][j] = '~';
                                    tableroEnemigo[i][j] = '~';
                                    disparosJugador[i][j] = '~';
                                    disparosIA[i][j] = '~';
                                }
                            }
                            
                            inicializarTableroEnemigo(enemigo, tableroEnemigo);
                            
                            for (auto& barco : barcosSeleccionables) {
                                barco.colocado = false;
                            }
                            
                            barcoSeleccionado = 0;
                            horizontal = true;
                            todosColocados = false;
                            juegoTerminado = false;
                            turnoJugador = true;
                            esperandoTurnoIA = false;
                            mensaje = "";
                            ganador = "";
                            
                            estado = COLOCANDO_BARCOS;
                        } else if (botonSalir.contienePunto(mouseX, mouseY)) {
                            window.close();
                        }
                    }
                }
                
                // Resaltar botones al pasar el mouse
                if (event->is<sf::Event::MouseMoved>()) {
                    const auto& mouseEvent = event->getIf<sf::Event::MouseMoved>();
                    if (mouseEvent) {
                        float mouseX = static_cast<float>(mouseEvent->position.x);
                        float mouseY = static_cast<float>(mouseEvent->position.y);
                        
                        botonIniciar.setResaltado(botonIniciar.contienePunto(mouseX, mouseY));
                        botonSalir.setResaltado(botonSalir.contienePunto(mouseX, mouseY));
                    }
                }
                
            } else if (estado == COLOCANDO_BARCOS) {
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
                                            turnoJugador = true;
                                            mensaje = "¡Comenzando batalla! Es tu turno";
                                        } else {
                                            for (size_t i = 0; i < barcosSeleccionables.size(); i++) {
                                                if (!barcosSeleccionables[i].colocado) {
                                                    barcoSeleccionado = static_cast<int>(i);
                                                    break;
                                                }
                                            }
                                        }
                                    } else {
                                        mensaje = "No se puede colocar el barco ahí";
                                    }
                                }
                            }
                        }
                    }
                }
                
            } else if (estado == JUGANDO && turnoJugador && !esperandoTurnoIA) {
                if (event->is<sf::Event::MouseButtonPressed>()) {
                    const auto& mouseEvent = event->getIf<sf::Event::MouseButtonPressed>();
                    if (mouseEvent && mouseEvent->button == sf::Mouse::Button::Left) {
                        int mouseX = mouseEvent->position.x;
                        int mouseY = mouseEvent->position.y;
                        
                        pair<int, int> coords = obtenerCoordenadas(mouseX, mouseY, BOARD_OFFSET_X, BOARD_OFFSET_Y, CELL_SIZE);
                        if (coords.first != -1 && coords.second != -1) {
                            int fila = coords.first;
                            int col = coords.second;
                            
                            if (disparosJugador[fila][col] == '~') {
                                if (jugador.registrarDisparo(fila, col)) {
                                    bool impacto = enemigo.recibirDisparo(fila, col);
                                    
                                    if (impacto) {
                                        disparosJugador[fila][col] = 'H';
                                        
                                        if (enemigo.todosLosBarcosHundidos()) {
                                            estado = JUEGO_TERMINADO;
                                            ganador = "JUGADOR";
                                            juegoTerminado = true;
                                        }
                                    } else {
                                        disparosJugador[fila][col] = 'X';
                                        turnoJugador = false;
                                        esperandoTurnoIA = true;
                                        clockIA.restart();
                                    }
                                }
                            }
                        }
                    }
                }
            } else if (estado == JUEGO_TERMINADO) {
                if (event->is<sf::Event::KeyPressed>()) {
                    const auto& keyEvent = event->getIf<sf::Event::KeyPressed>();
                    if (keyEvent && keyEvent->code == sf::Keyboard::Key::Escape) {
                        estado = MENU_PRINCIPAL;
                    }
                }
            }
        }
        
        if (estado == JUGANDO && !turnoJugador && esperandoTurnoIA && !juegoTerminado) {
            if (clockIA.getElapsedTime().asSeconds() > 1.0f) { 
                EstadoJuego estadoIA = crearEstadoParaIA(tableroJugador, tableroEnemigo, disparosJugador, disparosIA, false);
                Movimiento movIA = ia.decidirMovimiento(estadoIA);
                
                if (movIA.fila != -1 && movIA.col != -1) {
                    bool impacto = jugador.recibirDisparo(movIA.fila, movIA.col);
                    
                    if (impacto) {
                        disparosIA[movIA.fila][movIA.col] = 'H';
                        
                        if (jugador.todosLosBarcosHundidos()) {
                            estado = JUEGO_TERMINADO;
                            ganador = "IA";
                            juegoTerminado = true;
                        } else {
                            esperandoTurnoIA = true;
                            clockIA.restart();
                        }
                    } else {
                        disparosIA[movIA.fila][movIA.col] = 'X';
                        turnoJugador = true;
                        esperandoTurnoIA = false;
                    }
                } else {
                    int fila, col;
                    do {
                        fila = rand() % 10;
                        col = rand() % 10;
                    } while (disparosIA[fila][col] != '~');
                    
                    bool impacto = jugador.recibirDisparo(fila, col);
                    
                    if (impacto) {
                        disparosIA[fila][col] = 'H';
                        
                        if (jugador.todosLosBarcosHundidos()) {
                            estado = JUEGO_TERMINADO;
                            ganador = "IA";
                            juegoTerminado = true;
                        } else {
                            esperandoTurnoIA = true;
                            clockIA.restart();
                        }
                    } else {
                        disparosIA[fila][col] = 'X';
                        turnoJugador = true;
                        esperandoTurnoIA = false;
                    }
                }
            }
        }
       
        window.clear(sf::Color::Black);
       
        if (estado == MENU_PRINCIPAL) {
            textoTitulo.setString("BATALLA NAVAL");
            textoTitulo.setPosition(sf::Vector2f(280, 200));
            window.draw(textoTitulo);

            window.draw(botonIniciar.rectangulo);
            window.draw(botonIniciar.texto);
            window.draw(botonSalir.rectangulo);
            window.draw(botonSalir.texto);
            
        } else if (estado == COLOCANDO_BARCOS) {
            textoTitulo.setString("COLOCA TUS BARCOS");
            textoTitulo.setCharacterSize(20);
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
            textoTitulo.setString("BATALLA NAVAL vs IA");
            textoTitulo.setCharacterSize(20);
            textoTitulo.setPosition(sf::Vector2f(280, 15));
            window.draw(textoTitulo);
            
            if (turnoJugador && !esperandoTurnoIA) {
                textoTurno.setString("TU TURNO - Dispara al tablero enemigo");
                textoTurno.setFillColor(sf::Color::Green);
            } else {
                textoTurno.setString("TURNO DE LA IA - Esperando...");
                textoTurno.setFillColor(sf::Color::Red);
            }
            textoTurno.setPosition(sf::Vector2f(180, 45));
            window.draw(textoTurno);
            
            dibujarTablero(window, celda, texto, tableroEnemigo, BOARD_OFFSET_X, BOARD_OFFSET_Y, CELL_SIZE, false, (char*)disparosJugador);
            
            texto.setPosition(sf::Vector2f(SMALL_BOARD_OFFSET_X, SMALL_BOARD_OFFSET_Y - 25));
            window.draw(texto);
            
            dibujarTablero(window, celdaPequena, texto, tableroJugador, SMALL_BOARD_OFFSET_X, SMALL_BOARD_OFFSET_Y, SMALL_CELL_SIZE, true, (char*)disparosIA);
            
        } else if (estado == JUEGO_TERMINADO) {
            if (ganador == "JUGADOR") {
                textoVictoria.setString("VICTORIA");
                textoVictoria.setFillColor(sf::Color::Green);
                texto.setString("Has derrotado a la IA");
            } else {
                textoVictoria.setString("DERROTA");
                textoVictoria.setFillColor(sf::Color::Red);
                texto.setString("La IA ha hundido todos tus barcos");
            }
            
            textoVictoria.setPosition(sf::Vector2f(300, 200));
            window.draw(textoVictoria);
            
            texto.setPosition(sf::Vector2f(280, 250));
            window.draw(texto);
            
            texto.setString("Presiona ESC para volver al menu");
            texto.setPosition(sf::Vector2f(280, 300));
            window.draw(texto);
        }
        
        if (!mensaje.empty() && estado != JUEGO_TERMINADO && estado != MENU_PRINCIPAL) {
            texto.setString(mensaje);
            texto.setPosition(sf::Vector2f(50, 780));
            window.draw(texto);
        }
       
        window.display();
    }
   
    return 0;
}