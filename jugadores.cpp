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
   
    Tablero& getTablero() {
        return tablero;
    }
    
    bool todosBarcosHundidos() const {
        return tablero.todosBarcosHundidos();
    }
    
    bool recibirDisparo(int fila, int col) {
        if (!tablero.coordenadasValidas(fila, col)) {
            return false;
        }
        return tablero.disparar(fila, col);
    }
   
    bool disparar(Jugador& enemigo, int fila, int col) {
        if (!tablero.coordenadasValidas(fila, col)) {
            cout << "Coordenadas fuera de rango." << endl;
            return false;
        }
        
        if (disparos.contiene(fila, col)) {
            cout << "Ya disparaste en (" << fila << ", " << col << "). Intenta con otras coordenadas." << endl;
            return false;
        }
        
    
        disparos.agregar(fila, col);
        
    
        bool acierto = enemigo.recibirDisparo(fila, col);
        
        
        if (acierto) {
            cout << "¡Disparo acertado en (" << fila << ", " << col << ")!" << endl;
            
            
            if (enemigo.todosBarcosHundidos()) {
                cout << "¡Has hundido todos los barcos enemigos!" << endl;
            }
        } else {
            cout << "Disparo fallido en (" << fila << ", " << col << ")." << endl;
        }
        
        return true;
    }
    
    
    void mostrarTablero(bool mostrarBarcos = true) {
        tablero.mostrar(mostrarBarcos);
    }
};