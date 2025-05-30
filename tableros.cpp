#include <iostream>
#include <vector>
#include "barcos.cpp"
using namespace std;
class Tablero
{
private:
    char celdas[10][10];
    vector<Barco> barcos;
public:
   
    Tablero() {
        for (int i = 0; i < 10; ++i)
            for (int j = 0; j < 10; ++j)
                celdas[i][j] = '~';
    }
    bool colocarBarco(int fila, int col, int largo, bool horizontal) {
        // MEJORA: Validar límites negativos y largo inválido
        if (fila < 0 || col < 0 || largo <= 0) {
            return false;
        }
        
        int f;
        int c;
       
        for (int i = 0; i < largo; ++i) {
            if (horizontal) {
                f = fila;  
                c = col + i;  
            } else {
                f = fila + i;  
                c = col;
            }
           
            if (f >= 10 || c >= 10 || celdas[f][c] != '~') {
                return false;
            }
        }
       
       
        for (int i = 0; i < largo; ++i) {
            if (horizontal) {
                f = fila;  
                c = col + i;  
            } else {
                f = fila + i;
                c = col;  
            }
            celdas[f][c] = 'B';
        }
       
        barcos.push_back(Barco(fila, col, largo, horizontal));
        return true;
    }
    bool disparar(int fila, int col) {
        if (fila < 0 || fila >= 10 || col < 0 || col >= 10)
            return false;
       
        if (celdas[fila][col] == 'X' || celdas[fila][col] == 'O')
            return false;
           
        if (celdas[fila][col] == 'B') {
            celdas[fila][col] = 'X';  
       
            for (int i = 0; i < barcos.size(); ++i) {
                Barco& b = barcos[i];
                if (b.contiene(fila, col)) {
                    b.recibirImpacto();
                    break;
                }
            }
            return true;
        }
       
        celdas[fila][col] = 'O';
        return false;
    }
    bool todosLosBarcosHundidos() {
        for (int i = 0; i < barcos.size(); ++i) {
            Barco& b = barcos[i];
            if (!b.estaHundido())
                return false;
        }
        return true;
    }
};