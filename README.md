Manuel Morales 20.824.563-5 ITI


Interacción entre Clases
El juego la interacción parte entre las clases Jugador, Tablero y Barco.
Cuando el jugador solicita colocar un barco, el Tablero valida la posición y el espacio disponible. Si es válido, se crea un objeto Barco, se añade al vector de barcos del tablero y se actualiza la matriz celdas[10][10], marcando las posiciones ocupadas con “B”.
Además, el Jugador interactúa con la clase ListaDisparos, que mantiene una lista enlazada de disparos. Al registrar un nuevo disparo, ListaDisparos recorre la lista para evitar duplicados. Si el disparo no existe, crea un nuevo nodo y lo enlaza al final.
Todos estos comportamientos también son heredados por la clase JugadorIA, que extiende las funcionalidades básicas con la herramienta de minimax y el algoritmo minimax.


Atributos Principales
Tablero::celdas[10][10] : El estado fundamental del juego 
Barco::impactos : Determina si un barco está hundido 
JugadorIA::impactosActivos :Controla la estrategia de la IA 
JugadorIA::tableroDisparos : Memoria de la IA sobre sus acciones 
Jugador::tablero :Conexión entre jugador y su tablero



Estructura del Proyecto
estructuras.h: Header compartido que define:
EstadoJuego: Representa el estado completo de la partida.
Movimiento: Representa un disparo y su valor estratégico para la IA.

barcos.cpp: Define la clase Barco, que maneja:
Posición del barco.
Impactos recibidos.
Estado (activo o hundido).

tableros.cpp: Define la clase Tablero, que gestiona:
Tamaño.
Colocación de barcos.
Procesamiento de disparos.
Validación de condiciones de victoria.

disparos.cpp: Define la clase ListaDisparos  y el nodo que mantiene:
Una lista enlazada de disparos.
Verificación de duplicados.

jugadores.cpp: Clase base Jugador que integra:
Tablero propio y tablero de disparo.
Operaciones básicas del juego.

ia.cpp: Hereda de Jugador e implementa:
Algoritmo minimax con poda alfa-beta.
Árbol de decisiones para seleccionar estrategias.
Matriz de probabilidades.
Estrategias múltiples.

Main.cpp: Archivo principal que implementa: 
Interfaz gráfica usando SFML.
Máquina de estados del juego 
Gestión de eventos de usuario 
Bucle principal que coordina lógica y presentación.
Componentes visuales



Funciones Clave 
Tablero::colocarBarco()
Valida y coloca los barcos en la grilla

Tablero::disparar()
Procesa los disparos recibidos, actualiza el estado del tablero y determina si hubo impacto o hundimiento.

Tablero::todosLosBarcosHundidos()
Evalúa si todos los barcos han sido destruidos, determinando el fin del juego.

Barco::contiene()
Verifica si una coordenada pertenece al barco

JugadorIA::decidirDisparo()
Función principal de la IA. Utiliza todos los recursos estratégicos para decidir la próxima jugada.

JugadorIA::aplicarMinimax()
Implementa el algoritmo Minimax y evalúa posibles jugadas futuras optimizando los movimientos.

JugadorIA::procesarResultadoDisparo()
Actualiza el conocimiento de la IA tras cada disparo

JugadorIA::seleccionarEstrategia()
Utiliza un árbol de decisiones para escoger entre las estrategias

JugadorIA::generarCandidatos()
Genera una lista de movimientos válidos según la estrategia actual.

JugadorIA::actualizarProbabilidades()
Ajusta la matriz de probabilidades en función de los resultados anteriores.

JugadorIA::buscarObjetivosDirectos()
Detecta casillas adyacentes a impactos previos para finalizar el hundimiento de barcos.

ListaDisparos::registrarDisparo()
Registra un disparo nuevo, asegurando que no se repitan coordenadas.




Análisis IA:
La IA implementa un árbol de decisiones para la selección de estrategias y el algoritmo minimax para la evaluación de movimientos. El árbol de decisiones organiza las estrategias. El nodo raíz analiza el estado del juego y elige entre tres estrategias según prioridad y cada estrategia se activa según condiciones específicas. Esto permite decisiones adaptativas basadas en el estado del tablero.
Por su parte, minimax es un algoritmo que recorre un árbol de jugadas posibles, evaluando escenarios futuros con una profundidad de tres niveles. Alterna entre niveles que maximizan y minimizan, simulando turnos alternos. La poda alfa-beta optimiza este proceso, descartando ramas que no pueden mejorar el resultado. Se utiliza un sistema de puntuación para evaluar la calidad de los movimientos.

