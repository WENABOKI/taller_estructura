#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>
#include <unordered_map>
#include <cstdlib>
#include <ctime>
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
        for (NodoDecision* hijo : hijos) {
            delete hijo;
        }
    }
};

class JugadorIA : public Jugador {
private:
    int profundidadMinimax = 3;
    int maxCandidatos = 6;
    int probBase = 50;
    int infinito = 999999;
    
    char tableroDisparos[10][10];
    char tableroImpactos[10][10];
    
    vector<vector<int>> probabilidades;
    vector<pair<int, int>> impactosActivos; 
    NodoDecision* raizArbol;
    mutable unordered_map<string, int> cache;
    int nodosEvaluados;
    int podasRealizadas;
    vector<pair<int, int>> direcciones;
    bool inicializado;
    
    int valorAbsoluto(int x) {
        if (x < 0) {
            return -x;
        } else {
            return x;
        }
    }

public:
    JugadorIA() : Jugador(), inicializado(false), raizArbol(nullptr), nodosEvaluados(0), podasRealizadas(0) {
        direcciones = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};
        inicializar();
        construirArbolDecision();
        srand(static_cast<unsigned int>(time(0)));
        inicializado = true;
    }
    
    ~JugadorIA() {
        if (raizArbol) {
            delete raizArbol;
            raizArbol = nullptr;
        }
    }
    
    void inicializar() {
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                tableroDisparos[i][j] = '~'; 
                tableroImpactos[i][j] = '~'; 
            }
        }
        
        probabilidades.clear();
        probabilidades.resize(10, vector<int>(10, probBase));
        impactosActivos.clear();
        calcularProbabilidadesIniciales();
    }
    
    void calcularProbabilidadesIniciales() {
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                probabilidades[i][j] = probBase;
                
                if ((i + j) % 2 == 0) {
                    probabilidades[i][j] += 15;  
                }
                
                int variacion = (rand() % 11) - 5;  
                probabilidades[i][j] += variacion;
                
                probabilidades[i][j] = max(20, min(90, probabilidades[i][j]));
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
                bool horizontal;
                if (rand() % 2 == 1) {
                    horizontal = true;
                } else {
                    horizontal = false;
                }
                
                if (colocarBarco(fila, col, tamano, horizontal)) {
                    colocado = true;
                }
                intentos++;
            }
        }
    }
    
    void construirArbolDecision() {
        if (raizArbol) {
            delete raizArbol;
            raizArbol = nullptr;
        }
        
        raizArbol = new NodoDecision("raiz", "siempre", 100);
        
        NodoDecision* nodoCazar = new NodoDecision("cazar", "sin_impactos_activos", 70);
        NodoDecision* nodoApuntar = new NodoDecision("apuntar", "hay_impactos_aislados", 85);
        NodoDecision* nodoDestruir = new NodoDecision("destruir", "hay_patron_lineal", 95);
        
        raizArbol->hijos.push_back(nodoCazar);
        raizArbol->hijos.push_back(nodoApuntar);
        raizArbol->hijos.push_back(nodoDestruir);
    }
    
    Movimiento decidirDisparo() {
        if (!inicializado) {
            return generarMovimientoSeguro();
        }
        
        nodosEvaluados = 0;
        podasRealizadas = 0;
        
        EstadoJuego estadoActual = crearEstadoActual();
        
        Movimiento mejorMovimiento = aplicarMinimax(estadoActual);
        
        if (mejorMovimiento.fila == -1 || mejorMovimiento.col == -1) {
            return decidirDisparoTradicional();
        }
        
        return mejorMovimiento;
    }
    
    Movimiento decidirDisparoTradicional() {
        vector<pair<int, int>> casillasLibres = obtenerCasillasLibres();
        if (casillasLibres.empty()) {
            return Movimiento(-1, -1, 0); 
        }
        
        string estrategia = seleccionarEstrategia();
        vector<Movimiento> candidatos = generarCandidatos(estrategia);
        
        if (candidatos.empty()) {
            int idx = rand() % casillasLibres.size();
            return Movimiento(casillasLibres[idx].first, casillasLibres[idx].second, probBase);
        }
        
        return seleccionarMejorMovimiento(candidatos);
    }
    
    void procesarResultadoDisparo(int fila, int col, bool impacto, bool hundido) {
        if (fila < 0 || fila >= 10 || col < 0 || col >= 10) {
            return;
        }
        
        if (impacto) {
            if (hundido) {
                tableroDisparos[fila][col] = 'S'; 
                removerImpactoActivo(fila, col);
                limpiarImpactosDeBarcoHundido(fila, col);
            } else {
                tableroDisparos[fila][col] = 'H'; 
                impactosActivos.push_back({fila, col});
            }
        } else {
            tableroDisparos[fila][col] = 'X'; 
        }
        
        actualizarProbabilidades(fila, col, impacto);
    }
    
    void procesarDisparoRecibido(int fila, int col, bool impacto) {
        if (fila >= 0 && fila < 10 && col >= 0 && col < 10) {
            if (impacto) {
                tableroImpactos[fila][col] = 'H';
            } else {
                tableroImpactos[fila][col] = 'X';
            }
        }
    }
    
    char obtenerEstadoCasilla(int fila, int col) const {
        if (fila < 0 || fila >= 10 || col < 0 || col >= 10) {
            return '~';
        }
        return getTablero().obtenerCelda(fila, col);
    }

private:    
    EstadoJuego crearEstadoActual() {
        EstadoJuego estado;
        
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                estado.disparosIA[i][j] = tableroDisparos[i][j];
                estado.disparosJugador[i][j] = tableroImpactos[i][j];
                estado.tableroIA[i][j] = getTablero().obtenerCelda(i, j);
                estado.tableroJugador[i][j] = '~';
            }
        }
        
        estado.turnoJugador = false; 
        estado.puntajeIA = calcularPuntajeIA();
        estado.puntajeJugador = calcularPuntajeJugador();
        
        return estado;
    }
    
    Movimiento aplicarMinimax(EstadoJuego& estado) {
        vector<Movimiento> movimientosPosibles = generarMovimientosPosibles(estado);
        
        if (movimientosPosibles.empty()) {
            return Movimiento(-1, -1, 0);
        }
        
        Movimiento mejorMovimiento = movimientosPosibles[0];
        int mejorValor = -infinito;
        
        for (Movimiento& movimiento : movimientosPosibles) {
            EstadoJuego nuevoEstado = estado;
            aplicarMovimiento(nuevoEstado, movimiento);
            
            int valor = minimax(nuevoEstado, profundidadMinimax - 1, -infinito, infinito, false);
            
            if (valor > mejorValor) {
                mejorValor = valor;
                mejorMovimiento = movimiento;
            }
        }
        
        mejorMovimiento.valor = mejorValor;
        return mejorMovimiento;
    }
    
    int minimax(EstadoJuego& estado, int profundidad, int alfa, int beta, bool maximizando) {
        nodosEvaluados++;
        
        if (profundidad == 0 || esEstadoTerminal(estado)) {
            return evaluarEstado(estado);
        }
        
        vector<Movimiento> movimientos = generarMovimientosPosibles(estado);
        
        if (maximizando) {
            int maxEval = -infinito;
            for (auto& movimiento : movimientos) {
                EstadoJuego nuevoEstado = estado;
                aplicarMovimiento(nuevoEstado, movimiento);
                
                int eval = minimax(nuevoEstado, profundidad - 1, alfa, beta, false);
                maxEval = max(maxEval, eval);
                alfa = max(alfa, eval);
                
                if (beta <= alfa) {
                    podasRealizadas++;
                    break; 
                }
            }
            return maxEval;
        } else {
            int minEval = infinito;
            for (auto& movimiento : movimientos) {
                EstadoJuego nuevoEstado = estado;
                aplicarMovimiento(nuevoEstado, movimiento);
                
                int eval = minimax(nuevoEstado, profundidad - 1, alfa, beta, true);
                minEval = min(minEval, eval);
                beta = min(beta, eval);
                
                if (beta <= alfa) {
                    podasRealizadas++;
                    break; 
                }
            }
            return minEval;
        }
    }
    
    vector<Movimiento> generarMovimientosPosibles(EstadoJuego& estado) {
        vector<Movimiento> movimientos;
        
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                char casilla;
                if (estado.turnoJugador) {
                    casilla = estado.disparosJugador[i][j];
                } else {
                    casilla = estado.disparosIA[i][j];
                }
                
                if (casilla == '~') {
                    int valor = calcularValorMovimiento(estado, i, j);
                    movimientos.push_back(Movimiento(i, j, valor));
                }
            }
        }
        
        sort(movimientos.begin(), movimientos.end(), 
             [](const Movimiento& a, const Movimiento& b) {
                 return a.valor > b.valor;
             });
        
        if (movimientos.size() > maxCandidatos) {
            movimientos.resize(maxCandidatos);
        }
        
        return movimientos;
    }
    
    void aplicarMovimiento(EstadoJuego& estado, Movimiento& movimiento) {
        if (estado.turnoJugador) {
            estado.disparosJugador[movimiento.fila][movimiento.col] = 'X';
            if (estado.tableroIA[movimiento.fila][movimiento.col] == 'B') {
                estado.disparosJugador[movimiento.fila][movimiento.col] = 'H';
                estado.puntajeJugador += 10;
            }
        } else {
            estado.disparosIA[movimiento.fila][movimiento.col] = 'X';
            if (rand() % 100 < probabilidades[movimiento.fila][movimiento.col]) {
                estado.disparosIA[movimiento.fila][movimiento.col] = 'H';
                estado.puntajeIA += 10;
            }
        }
        
        estado.turnoJugador = !estado.turnoJugador;
    }
    
    int evaluarEstado(EstadoJuego& estado) {
        int puntuacion = 0;
        
        puntuacion += (estado.puntajeIA - estado.puntajeJugador) * 10;
        
        puntuacion += evaluarPatronesImpactos(estado);
        
        puntuacion += evaluarPosicionesEstrategicas(estado);
        
        return puntuacion;
    }
    
    int evaluarPatronesImpactos(EstadoJuego& estado) {
        int puntuacion = 0;
        
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                if (estado.disparosIA[i][j] == 'H') {
                    puntuacion += 20;
                    for (const auto& dir : direcciones) {
                        int ni = i + dir.first;
                        int nj = j + dir.second;
                        if (ni >= 0 && ni < 10 && nj >= 0 && nj < 10 && 
                            estado.disparosIA[ni][nj] == 'H') {
                            puntuacion += 15;
                        }
                    }
                }
            }
        }
        
        return puntuacion;
    }
    
    int evaluarPosicionesEstrategicas(EstadoJuego& estado) {
        int puntuacion = 0;
        
        int disparosRealizados = 0;
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                if (estado.disparosIA[i][j] != '~') {
                    disparosRealizados++;
                }
            }
        }
        
        if (disparosRealizados > 20 && estado.puntajeIA < 50) {
            puntuacion -= 30;
        }
        
        return puntuacion;
    }
    
    int calcularValorMovimiento(EstadoJuego& estado, int fila, int col) {
        int valor = probabilidades[fila][col];
        
        for (const auto& impacto : impactosActivos) {
            int distancia = valorAbsoluto(fila - impacto.first) + valorAbsoluto(col - impacto.second);
            if (distancia == 1) {
                valor += 100;
            } else if (distancia == 2) {
                valor += 20;
            }
        }
        
        return valor;
    }
    
    bool esEstadoTerminal(EstadoJuego& estado) {
        return estado.puntajeIA >= 170 || estado.puntajeJugador >= 170;
    }
    
    int calcularPuntajeIA() {
        int puntaje = 0;
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                if (tableroDisparos[i][j] == 'H' || tableroDisparos[i][j] == 'S') {
                    puntaje += 10;
                }
            }
        }
        return puntaje;
    }
    
    int calcularPuntajeJugador() {
        int puntaje = 0;
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                if (tableroImpactos[i][j] == 'H') {
                    puntaje += 10;
                }
            }
        }
        return puntaje;
    }
    
    Movimiento generarMovimientoSeguro() {
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                if (tableroDisparos[i][j] == '~') {
                    return Movimiento(i, j, probBase);
                }
            }
        }
        return Movimiento(0, 0, 0);
    }
    
    vector<pair<int, int>> obtenerCasillasLibres() {
        vector<pair<int, int>> libres;
        for (int i = 0; i < 10; i++) {
            for (int j = 0; j < 10; j++) {
                if (tableroDisparos[i][j] == '~') {
                    libres.push_back({i, j});
                }
            }
        }
        return libres;
    }
    
    void removerImpactoActivo(int fila, int col) {
        for (size_t i = 0; i < impactosActivos.size(); ++i) {
            if (impactosActivos[i].first == fila && impactosActivos[i].second == col) {
                impactosActivos[i] = impactosActivos.back();
                impactosActivos.pop_back();
                break; 
            }
        }
    }
    
    void limpiarImpactosDeBarcoHundido(int fila, int col) {
        vector<pair<int, int>> porProcesar = {{fila, col}};
        
        while (!porProcesar.empty()) {
            pair<int, int> actual = porProcesar.back();
            porProcesar.pop_back();
            
            for (const auto& dir : direcciones) {
                int nf = actual.first + dir.first;
                int nc = actual.second + dir.second;
                
                if (esValidoYImpacto(nf, nc)) {
                    tableroDisparos[nf][nc] = 'S';
                    removerImpactoActivo(nf, nc);
                    porProcesar.push_back({nf, nc});
                }
            }
        }
    }
    
    string seleccionarEstrategia() {
        if (!raizArbol) {
            return "cazar";
        }
        return evaluarNodo(raizArbol);
    }
    
    string evaluarNodo(NodoDecision* nodo) {
        if (!nodo) {
            return "cazar";
        }
        
        if (nodo->hijos.empty()) {
            return nodo->estrategia;
        }
        
        vector<NodoDecision*> hijosOrdenados = nodo->hijos;
        sort(hijosOrdenados.begin(), hijosOrdenados.end(),
             [](NodoDecision* a, NodoDecision* b) {
                 return a->prioridad > b->prioridad;
             });
        
        for (NodoDecision* hijo : hijosOrdenados) {
            if (evaluarCondicion(hijo->condicion)) {
                return evaluarNodo(hijo);
            }
        }
        
        return hijosOrdenados[0]->estrategia;
    }
    
    bool evaluarCondicion(const string& condicion) {
        if (condicion == "siempre") return true;
        if (condicion == "sin_impactos_activos") return impactosActivos.empty();
        if (condicion == "hay_impactos_aislados") return hayImpactosAislados();
        if (condicion == "hay_patron_lineal") return hayPatronLineal();
        return false;
    }
    
    bool hayImpactosAislados() {
        for (const auto& impacto : impactosActivos) {
            if (!tieneImpactoAdyacente(impacto.first, impacto.second)) {
                return true;
            }
        }
        return false;
    }
    
    bool hayPatronLineal() {
        for (const auto& impacto : impactosActivos) {
            if (tieneImpactoAdyacente(impacto.first, impacto.second)) {
                return true;
            }
        }
        return false;
    }
    
    bool tieneImpactoAdyacente(int fila, int col) {
        for (const auto& dir : direcciones) {
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
        
        if (estrategia == "apuntar" || estrategia == "destruir") {
            candidatos = buscarObjetivosDirectos();
        }
        
        if (candidatos.empty() || estrategia == "cazar") {
            candidatos = buscarCasillasOptimas();
        }
        
        if (candidatos.size() > maxCandidatos) {
            sort(candidatos.begin(), candidatos.end(),
                 [](const Movimiento& a, const Movimiento& b) {
                     return a.valor > b.valor;
                 });
            candidatos.resize(maxCandidatos);
        }
        
        return candidatos;
    }
    
    vector<Movimiento> buscarObjetivosDirectos() {
        vector<Movimiento> objetivos;
        
        for (const auto& impacto : impactosActivos) {
            int fila = impacto.first;
            int col = impacto.second;
            
            for (const auto& dir : direcciones) {
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
    
    Movimiento seleccionarMejorMovimiento(vector<Movimiento>& candidatos) {
        if (candidatos.empty()) {
            return Movimiento(0, 0, 0);
        }
        
        if (candidatos.size() == 1) {
            return candidatos[0];
        }
        
        Movimiento mejor = candidatos[0];
        for (const Movimiento& candidato : candidatos) {
            if (candidato.valor > mejor.valor) {
                mejor = candidato;
            }
        }
        
        return mejor;
    }
    
    void actualizarProbabilidades(int fila, int col, bool impacto) {
        if (impacto) {
            for (const auto& dir : direcciones) {
                int nf = fila + dir.first;
                int nc = col + dir.second;
                if (esValidoYLibre(nf, nc)) {
                    probabilidades[nf][nc] = min(90, probabilidades[nf][nc] + 30);
                }
            }
        } else {
            probabilidades[fila][col] = 0;
        }
    }
    
    int calcularPuntuacion(int fila, int col) {
        int puntuacion = probabilidades[fila][col];
        
        for (const auto& impacto : impactosActivos) {
            int distancia = valorAbsoluto(fila - impacto.first) + valorAbsoluto(col - impacto.second);
            if (distancia == 1) {
                puntuacion += 100;
            } else if (distancia == 2) {
                puntuacion += 20;
            }
        }
        
        return puntuacion;
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