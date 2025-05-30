#include <iostream>
using namespace std;
struct Nodo
{
    int fila;
    int col;
    Nodo* siguiente;
};
class ListaDisparos {
private:
    Nodo* cabeza;
public:
    ListaDisparos(){
        cabeza = nullptr;
    }
    ~ListaDisparos(){
        Nodo* actual = cabeza;
        while (actual != nullptr) {
            Nodo* temp = actual;
            actual = actual->siguiente;
            delete temp;
        }
    }
    
    bool registrarDisparo(int fila, int col){
        Nodo* actual = cabeza;
        while(actual != nullptr){
            if(actual->fila == fila && actual->col == col){
                return false;
            }
            actual = actual->siguiente;
        }
        
        Nodo* disparo = new Nodo;
        disparo->fila = fila;
        disparo->col = col;
        disparo->siguiente = nullptr;  
       
        if (cabeza == nullptr) {
            cabeza = disparo;
        } else {
            Nodo* temp = cabeza;
            while (temp->siguiente != nullptr) {
                temp = temp->siguiente;
            }
            temp->siguiente = disparo;
        }
        
        return true; 
    }
};