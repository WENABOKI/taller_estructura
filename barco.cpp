#include <iostream>
using namespace std;

class Barco {
private:
    int filaInicial;
    int colInicial;
    int largo;
    bool horizontal;
    int impactos;

public:
    Barco(int fila, int col, int largo, bool horizontal){
        this->filaInicial = fila;
        this->colInicial = col;
        this->largo = largo;
        this->horizontal = horizontal;
        impactos = 0;
    }


    bool contiene(int fila, int col) {
        int f;
        int c;
        if(horizontal){
        f = filaInicial;
        c = colInicial + i;
    } else {
        f = filaInicial + i;
        c = colInicial;
    }

    if (f == fila && c == col){
        return true;
}
return false;
}
     
    

    void recibirImpacto() {
        impactos++;
    }

    bool estaHundido() {
        return impactos >= largo;
    }

    int getFila() { return filaInicial; }
    int getCol() { return colInicial; }
    int getLargo() { return largo; }
    bool esHorizontal() { return horizontal; }

};

