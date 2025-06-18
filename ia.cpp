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

class NodoDecision {
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

class IABattleship {
private:
    static constexpr int PROFUNDIDAD_MINIMAX = 3;
    static constexpr int MAX_CANDIDATOS = 6;
    static constexpr int PROB_BASE = 50;
    
    vector<vector<int>> probabilidades;
    vector<vector<pair<int, int>>> barcosHundidos;
    vector<int> barcosRestantes;
    NodoDecision* raizArbol;
    
    mutable unordered_map<string, int> cache;
    int totalDisparos;
    int nodosEvaluados;
    int podasRealizadas;
    vector<pair<int, int>> direcciones = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

public:
    IABattleship() {
        inicializar();
        construirArbolDecision();
        srand(time(0));
        nodosEvaluados = 0;
        podasRealizadas = 0;
    }
    
    ~IABattleship() {
        delete raizArbol;
    }
    
    void inicializar() {
        probabilidades.resize(10, vector<int>(10, PROB_BASE));
        barcosRestantes = {4, 3, 2, 2};
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
    
    void construirArbolDecision() {
        raizArbol = new NodoDecision("ROOT", "siempre", 100);
        
        NodoDecision* nodoHunt = new NodoDecision("HUNT", "sin_impactos_activos", 70);
        NodoDecision* nodoTarget = new NodoDecision("TARGET", "hay_impactos_aislados", 85);
        NodoDecision* nodoDestroy = new NodoDecision("DESTROY", "hay_patron_lineal", 95);
        
        raizArbol->hijos.push_back(nodoHunt);
        raizArbol->hijos.push_back(nodoTarget);
        raizArbol->hijos.push_back(nodoDestroy);
    }
    
    bool evaluarCondicion(const string& condicion, const char disparos[10][10]) {
        if (condicion == "siempre") return true;
        
        if (condicion == "sin_impactos_activos") {
            return !hayImpactosSinHundir(disparos);
        }
        
        if (condicion == "hay_impactos_aislados") {
            return hayImpactosAislados(disparos);
        }
        
        if (condicion == "hay_patron_lineal") {
            return hayPatronLineal(disparos);
        }
        
        return false;
    }
    
    string seleccionarEstrategia(const char disparos[10][10]) {
        return evaluarNodo(raizArbol, disparos);
    }
    
    string evaluarNodo(NodoDecision* nodo, const char disparos[10][10]) {
        if (nodo->hijos.empty()) {
            return nodo->estrategia;
        }
        
        sort(nodo->hijos.begin(), nodo->hijos.end(),
             [](NodoDecision* a, NodoDecision* b) {
                 return a->prioridad > b->prioridad;
             });
        
        for (auto hijo : nodo->hijos) {
            if (evaluarCondicion(hijo->condicion, disparos)) {
                return evaluarNodo(hijo, disparos);
            }
        }
        
        return evaluarNodo(nodo->hijos[0], disparos);
    }
    
    Movimiento decidirMovimiento(const EstadoJuego& estado) {
        nodosEvaluados = 0;
        podasRealizadas = 0;
        
        string estrategia = seleccionarEstrategia(estado.disparosIA);
        vector<Movimiento> candidatos = generarCandidatos(estado, estrategia);
        
        if (candidatos.empty()) {
            return Movimiento(-1, -1, 0);
        }
        
        return aplicarMinimax(estado, candidatos);
    }
    
    vector<Movimiento> generarCandidatos(const EstadoJuego& estado, const string& estrategia) {
        vector<Movimiento> candidatos;
        
        if (estrategia == "TARGET" || estrategia == "DESTROY") {
            candidatos = buscarObjetivosDirectos(estado.disparosIA);
        }
        
        if (candidatos.empty() || estrategia == "HUNT") {
            candidatos = buscarCasillasOptimas(estado.disparosIA);
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
    
    vector<Movimiento> buscarObjetivosDirectos(const char disparos[10][10]) {
        vector<Movimiento> objetivos;
        
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                if (disparos[i][j] == 'H' && !estaHundido(i, j)) {
                    for (auto& dir : direcciones) {
                        int ni = i + dir.first;
                        int nj = j + dir.second;
                        
                        if (esValidaYLibre(ni, nj, disparos)) {
                            int puntuacion = 150 + probabilidades[ni][nj];
                            objetivos.push_back(Movimiento(ni, nj, puntuacion));
                        }
                    }
                }
            }
        }
        
        return objetivos;
    }
    
    vector<Movimiento> buscarCasillasOptimas(const char disparos[10][10]) {
        vector<Movimiento> candidatos;
        
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                if (disparos[i][j] == '~') {
                    int puntuacion = calcularPuntuacion(i, j, disparos);
                    candidatos.push_back(Movimiento(i, j, puntuacion));
                }
            }
        }
        
        return candidatos;
    }
    
    
    Movimiento aplicarMinimax(const EstadoJuego& estado, vector<Movimiento>& candidatos) {
        if (candidatos.size() == 1) {
            return candidatos[0];
        }
        
        Movimiento mejorMovimiento = candidatos[0];
        int mejorValor = INT_MIN;
        
        for (auto& candidato : candidatos) {
            string cacheKey = generarClave(estado, candidato);
            
            int valor;
            if (cache.find(cacheKey) != cache.end()) {
                valor = cache[cacheKey];
            } else {
                int probImpacto = calcularProbabilidadImpacto(candidato.fila, candidato.col, estado.disparosIA);
                
                EstadoJuego escenarioImpacto = estado;
                escenarioImpacto.disparosIA[candidato.fila][candidato.col] = 'H';
                int valorImpacto = minimax(escenarioImpacto, PROFUNDIDAD_MINIMAX - 1, false, INT_MIN, INT_MAX);
                
                EstadoJuego escenarioFallo = estado;
                escenarioFallo.disparosIA[candidato.fila][candidato.col] = 'X';
                int valorFallo = minimax(escenarioFallo, PROFUNDIDAD_MINIMAX - 1, false, INT_MIN, INT_MAX);
                
                valor = (probImpacto * valorImpacto + (100 - probImpacto) * valorFallo) / 100;
                cache[cacheKey] = valor;
            }
            
            if (valor > mejorValor) {
                mejorValor = valor;
                mejorMovimiento = candidato;
            }
        }
        
        return mejorMovimiento;
    }
    
    int minimax(const EstadoJuego& estado, int profundidad, bool maximizando, int alpha, int beta) {
        nodosEvaluados++;
        
        if (profundidad == 0 || juegoTerminado(estado)) {
            return evaluarEstado(estado);
        }
        
        vector<Movimiento> movimientos = generarMovimientosRapidos(estado, maximizando);
        
        if (movimientos.empty()) {
            return evaluarEstado(estado);
        }
        
        if (maximizando) {
            int maxEval = INT_MIN;
            
            for (auto& mov : movimientos) {
                EstadoJuego nuevoEstado = estado;
                
                if (esMovimientoPrometedor(mov, estado)) {
                    nuevoEstado.disparosIA[mov.fila][mov.col] = 'H';
                } else {
                    nuevoEstado.disparosIA[mov.fila][mov.col] = 'X';
                }
                
                int eval = minimax(nuevoEstado, profundidad - 1, false, alpha, beta);
                maxEval = max(maxEval, eval);
                alpha = max(alpha, eval);
                
                if (beta <= alpha) {
                    podasRealizadas++;
                    break;
                }
            }
            return maxEval;
            
        } else {
            int minEval = INT_MAX;
            
            for (auto& mov : movimientos) {
                EstadoJuego nuevoEstado = estado;
                
                if (mov.fila < 10 && mov.col < 10 && estado.disparosJugador[mov.fila][mov.col] == '~') {
                    nuevoEstado.disparosJugador[mov.fila][mov.col] = 'H';
                }
                
                int eval = minimax(nuevoEstado, profundidad - 1, true, alpha, beta);
                minEval = min(minEval, eval);
                beta = min(beta, eval);
                
                if (beta <= alpha) {
                    podasRealizadas++;
                    break;
                }
            }
            return minEval;
        }
    }
    
    
    bool juegoTerminado(const EstadoJuego& estado) {
        int impactosIA = 0;
        int impactosJugador = 0;
        
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                if (estado.disparosIA[i][j] == 'H') impactosIA++;
                if (estado.disparosJugador[i][j] == 'H') impactosJugador++;
            }
        }
        
        return impactosIA >= 17 || impactosJugador >= 17; 
    }
    
    bool esMovimientoPrometedor(const Movimiento& mov, const EstadoJuego& estado) {
        int probabilidad = calcularProbabilidadImpacto(mov.fila, mov.col, estado.disparosIA);
        
        bool tieneImpactosCerca = false;
        for (auto& dir : direcciones) {
            int ni = mov.fila + dir.first;
            int nj = mov.col + dir.second;
            if (esValidaYImpacto(ni, nj, estado.disparosIA) && !estaHundido(ni, nj)) {
                tieneImpactosCerca = true;
                break;
            }
        }
        
        return probabilidad > 60 || tieneImpactosCerca;
    }
    
    vector<Movimiento> generarMovimientosRapidos(const EstadoJuego& estado, bool paraIA) {
        vector<Movimiento> movimientos;
        
        if (paraIA) {
            for (int i = 0; i < 10; i++) {
                for (int j = 0; j < 10; j++) {
                    if (estado.disparosIA[i][j] == '~') {
                        int puntuacion = calcularPuntuacion(i, j, estado.disparosIA);
                        movimientos.push_back(Movimiento(i, j, puntuacion));
                    }
                }
            }
        } else {
            for (int i = 0; i < 10; i++) {
                for (int j = 0; j < 10; j++) {
                    if (estado.disparosJugador[i][j] == '~') {
                        int puntuacion = calcularPuntuacionDefensiva(i, j, estado);
                        movimientos.push_back(Movimiento(i, j, puntuacion));
                    }
                }
            }
        }
        
        sort(movimientos.begin(), movimientos.end(),
             [](const Movimiento& a, const Movimiento& b) {
                 return a.valor > b.valor;
             });
        
        if (movimientos.size() > MAX_CANDIDATOS) {
            movimientos.resize(MAX_CANDIDATOS);
        }
        
        return movimientos;
    }
    
    int calcularPuntuacionDefensiva(int fila, int col, const EstadoJuego& estado) {
        int puntuacion = 50;
        
        for (auto& dir : direcciones) {
            int ni = fila + dir.first;
            int nj = col + dir.second;
            if (ni >= 0 && ni < 10 && nj >= 0 && nj < 10) {
                if (estado.disparosJugador[ni][nj] == 'H') {
                    puntuacion += 80;
                }
            }
        }
        
        return puntuacion;
    }
    
    
    int evaluarEstado(const EstadoJuego& estado) {
        int puntuacion = 0;
        
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                if (estado.disparosIA[i][j] == 'H') {
                    puntuacion += 100;
                    if (tieneImpactoAdyacente(estado.disparosIA, i, j)) {
                        puntuacion += 30;
                    }
                } else if (estado.disparosIA[i][j] == 'X') {
                    puntuacion -= 5;
                }
                
                if (estado.disparosJugador[i][j] == 'H') {
                    puntuacion -= 80;
                }
            }
        }
        
        puntuacion += barcosHundidos.size() * 200;
        
        return puntuacion;
    }
    
    int calcularProbabilidadImpacto(int fila, int col, const char disparos[10][10]) {
        int probabilidad = probabilidades[fila][col];
        
        for (auto& dir : direcciones) {
            int ni = fila + dir.first;
            int nj = col + dir.second;
            if (esValidaYImpacto(ni, nj, disparos) && !estaHundido(ni, nj)) {
                probabilidad += 30;
            }
        }
        
        return min(95, max(5, probabilidad * 100 / 200));
    }
    
    
    bool hayImpactosSinHundir(const char disparos[10][10]) {
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                if (disparos[i][j] == 'H' && !estaHundido(i, j)) {
                    return true;
                }
            }
        }
        return false;
    }
    
    bool hayImpactosAislados(const char disparos[10][10]) {
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                if (disparos[i][j] == 'H' && !estaHundido(i, j)) {
                    if (!tieneImpactoAdyacente(disparos, i, j)) {
                        return true;
                    }
                }
            }
        }
        return false;
    }
    
    bool hayPatronLineal(const char disparos[10][10]) {
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                if (disparos[i][j] == 'H' && !estaHundido(i, j)) {
                    if (tieneImpactoAdyacente(disparos, i, j)) {
                        return true;
                    }
                }
            }
        }
        return false;
    }
    
    bool tieneImpactoAdyacente(const char disparos[10][10], int fila, int col) {
        for (auto& dir : direcciones) {
            int ni = fila + dir.first;
            int nj = col + dir.second;
            if (esValidaYImpacto(ni, nj, disparos)) {
                return true;
            }
        }
        return false;
    }
    
    bool estaHundido(int fila, int col) {
        for (auto& barco : barcosHundidos) {
            for (auto& casilla : barco) {
                if (casilla.first == fila && casilla.second == col) {
                    return true;
                }
            }
        }
        return false;
    }
    
    bool esValidaYLibre(int fila, int col, const char disparos[10][10]) {
        return fila >= 0 && fila < 10 && col >= 0 && col < 10 && 
               disparos[fila][col] == '~';
    }
    
    bool esValidaYImpacto(int fila, int col, const char disparos[10][10]) {
        return fila >= 0 && fila < 10 && col >= 0 && col < 10 && 
               disparos[fila][col] == 'H';
    }
    
    int calcularPuntuacion(int fila, int col, const char disparos[10][10]) {
        int puntuacion = probabilidades[fila][col];
        
        for (auto& dir : direcciones) {
            int ni = fila + dir.first;
            int nj = col + dir.second;
            if (esValidaYImpacto(ni, nj, disparos) && !estaHundido(ni, nj)) {
                puntuacion += 100;
            }
        }
        
        return puntuacion;
    }
    
    
    string generarClave(const EstadoJuego& estado, const Movimiento& mov) {
        string clave = to_string(mov.fila) + "," + to_string(mov.col) + ",";
        
        int hash = 0;
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                if (estado.disparosIA[i][j] == 'H') {
                    hash += i * 10 + j;
                }
            }
        }
        
        clave += to_string(hash);
        return clave;
    }
    
    void actualizarProbabilidadesPorTiempo(const char disparos[10][10]) {
        totalDisparos = 0;
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                if (disparos[i][j] != '~') {
                    totalDisparos++;
                }
            }
        }
        
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                if (disparos[i][j] == '~') {
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
    }
    void procesarBarcoHundido(const char tablero[10][10], char disparos[10][10], int fila, int col) {
        vector<pair<int, int>> casillasBarco = encontrarBarcoCompleto(disparos, fila, col);
        
        if (!casillasBarco.empty()) {
            barcosHundidos.push_back(casillasBarco);
            marcarAguaAdyacente(disparos, casillasBarco);
            actualizarBarcosRestantes(casillasBarco.size());
            cache.clear();
        }
    }
    
    vector<pair<int, int>> encontrarBarcoCompleto(const char disparos[10][10], int fila, int col) {
        vector<pair<int, int>> barco;
        vector<vector<bool>> visitado(10, vector<bool>(10, false));
        
        buscarCasillasBarcoRecursivo(disparos, fila, col, barco, visitado);
        
        return barco;
    }
    
    void buscarCasillasBarcoRecursivo(const char disparos[10][10], int fila, int col,
                                    vector<pair<int, int>>& barco, vector<vector<bool>>& visitado) {
        if (fila < 0 || fila >= 10 || col < 0 || col >= 10 || 
            visitado[fila][col] || disparos[fila][col] != 'H') {
            return;
        }
        
        visitado[fila][col] = true;
        barco.push_back({fila, col});
        
        for (auto& dir : direcciones) {
            int ni = fila + dir.first;
            int nj = col + dir.second;
            buscarCasillasBarcoRecursivo(disparos, ni, nj, barco, visitado);
        }
    }
    
    void marcarAguaAdyacente(char disparos[10][10], const vector<pair<int, int>>& barco) {
        for (auto& casilla : barco) {
            int f = casilla.first;
            int c = casilla.second;
            
            for (int di = -1; di <= 1; di++) {
                for (int dj = -1; dj <= 1; dj++) {
                    int nf = f + di;
                    int nc = c + dj;
                    
                    if (nf >= 0 && nf < 10 && nc >= 0 && nc < 10 && 
                        disparos[nf][nc] == '~') {
                        disparos[nf][nc] = 'N'; 
                    }
                }
            }
        }
    }
    
    void actualizarBarcosRestantes(int tamano) {
        auto it = find(barcosRestantes.begin(), barcosRestantes.end(), tamano);
        if (it != barcosRestantes.end()) {
            barcosRestantes.erase(it);
        }
    }
    
    
    vector<pair<pair<int, int>, pair<int, bool>>> colocarBarcosIA() {
        vector<pair<pair<int, int>, pair<int, bool>>> posiciones;
        vector<int> tamanos = {4, 3, 2, 2};
        char tableroTemp[10][10];
        
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                tableroTemp[i][j] = '~';
            }
        }
        
        for (int tamano : tamanos) {
            bool colocado = false;
            int intentos = 0;
            
            while (!colocado && intentos < 100) {
                int fila = rand() % 10;
                int col = rand() % 10;
                bool horizontal = rand() % 2;
                
                if (puedeColocarBarco(tableroTemp, fila, col, tamano, horizontal)) {
                    for (int i = 0; i < tamano; i++) {
                        int f = horizontal ? fila : fila + i;
                        int c = horizontal ? col + i : col;
                        tableroTemp[f][c] = 'B';
                    }
                    
                    posiciones.push_back({{fila, col}, {tamano, horizontal}});
                    colocado = true;
                }
                
                intentos++;
            }
        }
        
        return posiciones;
    }
    
private:
    bool puedeColocarBarco(char tablero[10][10], int fila, int col, int tamano, bool horizontal) {
        if (horizontal && col + tamano > 10) return false;
        if (!horizontal && fila + tamano > 10) return false;
        
        for (int i = 0; i < tamano; i++) {
            int f = horizontal ? fila : fila + i;
            int c = horizontal ? col + i : col;
            
            if (tablero[f][c] != '~') return false;
            
            for (int di = -1; di <= 1; di++) {
                for (int dj = -1; dj <= 1; dj++) {
                    int nf = f + di;
                    int nc = c + dj;
                    
                    if (nf >= 0 && nf < 10 && nc >= 0 && nc < 10 && 
                        tablero[nf][nc] == 'B') {
                        return false;
                    }
                }
            }
        }
        
        return true;
    }
};