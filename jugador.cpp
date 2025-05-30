#include <iostream>
#include "tableros.cpp"
#include "disparos.cpp"
using namespace std;
class Jugador {
private:
    Tablero tablero;
    ListaDisparos disparos;
   
public:
    Jugador() {
    }    
   
    bool colocarBarco(int fila, int col, int largo, bool horizontal) {
        return tablero.colocarBarco(fila, col, largo, horizontal);
    }
    bool recibirDisparo(int fila, int col) {
        return tablero.disparar(fila, col);
    }
    
    bool registrarDisparo(int fila, int col) {
        return disparos.registrarDisparo(fila, col);
    }
    
    bool todosLosBarcosHundidos() {
        return tablero.todosLosBarcosHundidos();
    }
   
};