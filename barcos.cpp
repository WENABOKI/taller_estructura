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
        if(horizontal){
            if(fila != filaInicial) return false;
            return (col >= colInicial && col < colInicial + largo);
        }
        else {
            if(col != colInicial) return false;
            return (fila >= filaInicial && fila < filaInicial + largo);
        }
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