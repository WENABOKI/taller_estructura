#include <iostream>
#include "Tablero.cpp"
#include "ListaDisparos.cpp"
using namespace std;


class Jugador
{
private:
    Tablero tablero;
    ListaDisparos disparos;
    
public:
    Jugador() {}

    
    Tablero& getTablero() {
        return tablero;
    }

    
    bool disparar(Jugador& enemigo, int fila, int col) {
        if (disparos.contiene(fila, col)) {
            std::cout << "Ya disparaste en (" << fila << ", " << col << ")" << std::endl;
            return false;
        }

        disparos.agregar(fila, col);
        bool acierto = enemigo.getTablero().disparar(fila, col);

        if (acierto)
            std::cout << "¡Disparo acertado en (" << fila << ", " << col << ")!" << std::endl;
        else
            std::cout << "Disparo fallido en (" << fila << ", " << col << ")." << std::endl;

        return true;
    }
}
