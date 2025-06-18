#ifndef ESTRUCTURAS_H
#define ESTRUCTURAS_H

struct EstadoJuego {
    char tableroJugador[10][10];
    char tableroIA[10][10];
    char disparosJugador[10][10];  
    char disparosIA[10][10];       
    bool turnoJugador;
    int puntajeJugador;
    int puntajeIA;
    
    EstadoJuego() {
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                tableroJugador[i][j] = '~';
                tableroIA[i][j] = '~';
                disparosJugador[i][j] = '~';
                disparosIA[i][j] = '~';
            }
        }
        turnoJugador = true;
        puntajeJugador = 0;
        puntajeIA = 0;
    }
};

struct Movimiento {
    int fila;
    int col;
    int valor;
    
    Movimiento(int f = -1, int c = -1, int v = 0) : fila(f), col(c), valor(v) {}
};

#endif