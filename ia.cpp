#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>
#include <unordered_map>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include "estructuras.h"
using namespace std;

struct NodoDecision {
public:
    string estrategia;
    string condicion;
    int prioridad;
    vector<NodoDecision*> hijos;
    
    NodoDecision(const string& est, const string& cond, int prio) 
        : estrategia(est), condicion(cond), prioridad(prio) {}
    
    ~NodoDecision() {
        for (auto hijo : hijos) {
            delete hijo;
        }
    }
};


class JugadorIA : public Jugador {
private:
    static constexpr int PROFUNDIDAD_MINIMAX = 3;
    static constexpr int MAX_CANDIDATOS = 6;
    static constexpr int PROB_BASE = 50;
    
    char tableroDisparos[10][10];
    char tableroImpactos[10][10];
    
    vector<vector<int>> probabilidades;
    vector<pair<int, int>> impactosActivos; 
    NodoDecision* raizArbol;
    mutable unordered_map<string, int> cache;
    int nodosEvaluados;
    int podasRealizadas;
    vector<pair<int, int>> direcciones = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

public:
    JugadorIA() : Jugador() {
        inicializar();
        construirArbolDecision();
        srand(time(0));
        nodosEvaluados = 0;
        podasRealizadas = 0;
    }
    
    ~JugadorIA() {
        delete raizArbol;
    }
    
    void inicializar() {
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                tableroDisparos[i][j] = '~'; 
                tableroImpactos[i][j] = '~'; 
            }
        }
        
        probabilidades.resize(10, vector<int>(10, PROB_BASE));
        calcularProbabilidadesIniciales();
    }
    
    void calcularProbabilidadesIniciales() {
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                probabilidades[i][j] = PROB_BASE;
                
                if ((i + j) % 2 == 0) {
                    probabilidades[i][j] += 25;  
                }
                
                int variacion = (rand() % 11) - 5;  
                probabilidades[i][j] += variacion;
                
                probabilidades[i][j] = max(30, min(100, probabilidades[i][j]));
            }
        }
    }
    
    void colocarBarcosAutomaticamente() {
        vector<int> tamanos = {4, 3, 2, 2};
        
        for (int tamano : tamanos) {
            bool colocado = false;
            int intentos = 0;
            
            while (!colocado && intentos < 100) {
                int fila = rand() % 10;
                int col = rand() % 10;
                bool horizontal = rand() % 2;
                
                if (colocarBarco(fila, col, tamano, horizontal)) {
                    colocado = true;
                }
                intentos++;
            }
        }
    }
    
    void construirArbolDecision() {
        raizArbol = new NodoDecision("ROOT", "siempre", 100);
        
        NodoDecision* nodoHunt = new NodoDecision("HUNT", "sin_impactos_activos", 70);
        NodoDecision* nodoTarget = new NodoDecision("TARGET", "hay_impactos_aislados", 85);
        NodoDecision* nodoDestroy = new NodoDecision("DESTROY", "hay_patron_lineal", 95);
        
        raizArbol->hijos.push_back(nodoHunt);
        raizArbol->hijos.push_back(nodoTarget);
        raizArbol->hijos.push_back(nodoDestroy);
    }
    
    Movimiento decidirDisparo() {
        nodosEvaluados = 0;
        podasRealizadas = 0;
        
        string estrategia = seleccionarEstrategia();
        vector<Movimiento> candidatos = generarCandidatos(estrategia);
        
        if (candidatos.empty()) {
            return Movimiento(-1, -1, 0);
        }
        
        return aplicarMinimax(candidatos);
    }
    
    void procesarResultadoDisparo(int fila, int col, bool impacto, bool hundido) {
        registrarDisparo(fila, col);
        
        if (impacto) {
            tableroDisparos[fila][col] = 'H'; 
            impactosActivos.push_back({fila, col});
            
            if (hundido) {
                procesarBarcoHundido(fila, col);
            }
            
            actualizarProbabilidades(fila, col, true);
        } else {
            tableroDisparos[fila][col] = 'X'; 
            actualizarProbabilidades(fila, col, false);
        }
    }
    
    void procesarDisparoRecibido(int fila, int col, bool impacto) {
        if (impacto) {
            tableroImpactos[fila][col] = 'H';
        } else {
            tableroImpactos[fila][col] = 'X'; 
        }
    }
    
private:
    string seleccionarEstrategia() {
        return evaluarNodo(raizArbol);
    }
    
    string evaluarNodo(NodoDecision* nodo) {
        if (nodo->hijos.empty()) {
            return nodo->estrategia;
        }
        
        sort(nodo->hijos.begin(), nodo->hijos.end(),
             [](NodoDecision* a, NodoDecision* b) {
                 return a->prioridad > b->prioridad;
             });
        
        for (auto hijo : nodo->hijos) {
            if (evaluarCondicion(hijo->condicion)) {
                return evaluarNodo(hijo);
            }
        }
        
        return evaluarNodo(nodo->hijos[0]);
    }
    
    bool evaluarCondicion(const string& condicion) {
        if (condicion == "siempre") return true;
        
        if (condicion == "sin_impactos_activos") {
            return impactosActivos.empty();
        }
        
        if (condicion == "hay_impactos_aislados") {
            return hayImpactosAislados();
        }
        
        if (condicion == "hay_patron_lineal") {
            return hayPatronLineal();
        }
        
        return false;
    }
    
    bool hayImpactosAislados() {
        for (auto& impacto : impactosActivos) {
            if (!tieneImpactoAdyacente(impacto.first, impacto.second)) {
                return true;
            }
        }
        return false;
    }
    
    bool hayPatronLineal() {
        for (auto& impacto : impactosActivos) {
            if (tieneImpactoAdyacente(impacto.first, impacto.second)) {
                return true;
            }
        }
        return false;
    }
    
    bool tieneImpactoAdyacente(int fila, int col) {
        for (auto& dir : direcciones) {
            int nf = fila + dir.first;
            int nc = col + dir.second;
            if (esValidoYImpacto(nf, nc)) {
                return true;
            }
        }
        return false;
    }
    
    vector<Movimiento> generarCandidatos(const string& estrategia) {
        vector<Movimiento> candidatos;
        
        if (estrategia == "TARGET" || estrategia == "DESTROY") {
            candidatos = buscarObjetivosDirectos();
        }
        
        if (candidatos.empty() || estrategia == "HUNT") {
            candidatos = buscarCasillasOptimas();
        }
        
        if (candidatos.size() > MAX_CANDIDATOS) {
            sort(candidatos.begin(), candidatos.end(),
                 [](const Movimiento& a, const Movimiento& b) {
                     return a.valor > b.valor;
                 });
            candidatos.resize(MAX_CANDIDATOS);
        }
        
        return candidatos;
    }
    
    vector<Movimiento> buscarObjetivosDirectos() {
        vector<Movimiento> objetivos;
        
        for (auto& impacto : impactosActivos) {
            int fila = impacto.first;
            int col = impacto.second;
            
            for (auto& dir : direcciones) {
                int nf = fila + dir.first;
                int nc = col + dir.second;
                
                if (esValidoYLibre(nf, nc)) {
                    int puntuacion = 150 + probabilidades[nf][nc];
                    objetivos.push_back(Movimiento(nf, nc, puntuacion));
                }
            }
        }
        
        return objetivos;
    }
    
    vector<Movimiento> buscarCasillasOptimas() {
        vector<Movimiento> candidatos;
        
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                if (tableroDisparos[i][j] == '~') {
                    int puntuacion = calcularPuntuacion(i, j);
                    candidatos.push_back(Movimiento(i, j, puntuacion));
                }
            }
        }
        
        return candidatos;
    }
    
    Movimiento aplicarMinimax(vector<Movimiento>& candidatos) {
        if (candidatos.size() == 1) {
            return candidatos[0];
        }
        
        Movimiento mejorMovimiento = candidatos[0];
        int mejorValor = INT_MIN;
        
        for (auto& candidato : candidatos) {
            int valor = evaluarMovimiento(candidato);
            
            if (valor > mejorValor) {
                mejorValor = valor;
                mejorMovimiento = candidato;
            }
        }
        
        return mejorMovimiento;
    }
    
    int evaluarMovimiento(const Movimiento& mov) {
        int probabilidadImpacto = calcularProbabilidadImpacto(mov.fila, mov.col);
        
        int valor = probabilidadImpacto * 10;
        
        for (auto& impacto : impactosActivos) {
            int distancia = abs(mov.fila - impacto.first) + abs(mov.col - impacto.second);
            if (distancia == 1) {
                valor += 200; 
            } else if (distancia == 2) {
                valor += 50; 
            }
        }
        
        return valor;
    }
    
    void procesarBarcoHundido(int fila, int col) {
        vector<pair<int, int>> barcoHundido = encontrarBarcoCompleto(fila, col);
        
        for (auto& casilla : barcoHundido) {
            tableroDisparos[casilla.first][casilla.second] = 'S';
            
            auto it = find(impactosActivos.begin(), impactosActivos.end(), casilla);
            if (it != impactosActivos.end()) {
                impactosActivos.erase(it);
            }
        }
        
        reducirProbabilidadesAlrededor(barcoHundido);
    }
    
    vector<pair<int, int>> encontrarBarcoCompleto(int fila, int col) {
        vector<pair<int, int>> barco;
        vector<vector<bool>> visitado(10, vector<bool>(10, false));
        
        buscarCasillasBarcoRecursivo(fila, col, barco, visitado);
        
        return barco;
    }
    
    void buscarCasillasBarcoRecursivo(int fila, int col, vector<pair<int, int>>& barco, 
                                     vector<vector<bool>>& visitado) {
        if (fila < 0 || fila >= 10 || col < 0 || col >= 10 || 
            visitado[fila][col] || tableroDisparos[fila][col] != 'H') {
            return;
        }
        
        visitado[fila][col] = true;
        barco.push_back({fila, col});
        
        for (auto& dir : direcciones) {
            int nf = fila + dir.first;
            int nc = col + dir.second;
            buscarCasillasBarcoRecursivo(nf, nc, barco, visitado);
        }
    }
    
    void reducirProbabilidadesAlrededor(const vector<pair<int, int>>& barco) {
        for (auto& casilla : barco) {
            int f = casilla.first;
            int c = casilla.second;
            
            for (int di = -1; di <= 1; di++) {
                for (int dj = -1; dj <= 1; dj++) {
                    int nf = f + di;
                    int nc = c + dj;
                    
                    if (nf >= 0 && nf < 10 && nc >= 0 && nc < 10) {
                        probabilidades[nf][nc] = 1; 
                    }
                }
            }
        }
    }
    
    void actualizarProbabilidades(int fila, int col, bool impacto) {
        if (impacto) {
            for (auto& dir : direcciones) {
                int nf = fila + dir.first;
                int nc = col + dir.second;
                if (esValidoYLibre(nf, nc)) {
                    probabilidades[nf][nc] = min(95, probabilidades[nf][nc] + 40);
                }
            }
        } else {
            probabilidades[fila][col] = 0;
        }
    }
    
    int calcularPuntuacion(int fila, int col) {
        int puntuacion = probabilidades[fila][col];
        
        for (auto& impacto : impactosActivos) {
            int distancia = abs(fila - impacto.first) + abs(col - impacto.second);
            if (distancia == 1) {
                puntuacion += 100;
            } else if (distancia == 2) {
                puntuacion += 20;
            }
        }
        
        return puntuacion;
    }
    
    int calcularProbabilidadImpacto(int fila, int col) {
        int probabilidad = probabilidades[fila][col];
        
        for (auto& impacto : impactosActivos) {
            int distancia = abs(fila - impacto.first) + abs(col - impacto.second);
            if (distancia == 1) {
                probabilidad += 30;
            }
        }
        
        return min(95, max(5, probabilidad));
    }
    
    bool esValidoYLibre(int fila, int col) {
        return fila >= 0 && fila < 10 && col >= 0 && col < 10 && 
               tableroDisparos[fila][col] == '~';
    }
    
    bool esValidoYImpacto(int fila, int col) {
        return fila >= 0 && fila < 10 && col >= 0 && col < 10 && 
               tableroDisparos[fila][col] == 'H';
    }
    
    
};