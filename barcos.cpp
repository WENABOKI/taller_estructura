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
            int f = filaInicial;
            for(int i = 0; i < largo; i++) {
                int c = colInicial+i;
                if (f == fila && c == col){
                    return true;
                }
            }
        } 
    
        else {
            int c = colInicial;
            for(int i = 0; i < largo; i++) {
                int f = filaInicial+i;
                if (f == fila && c == col){
                    return true;
                }
            }
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