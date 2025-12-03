//Código hecho por Daniel Lobato De La Fuente y Matheus Muiños Kruschewsky

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>
#include <sched.h>   // para sched_yield()

#define NUM_HILOS 100        // número de hilos que voy crear (modificable)
#define TOTAL_PUNTOS 100000000   // número de puntos generados por hilo
#define YIELD_INTERVAL 1000000000000  // frequencia en que los hilos generosos ceden la CPU

void* calcular_pi(void* arg) {
    int id = *(int*)arg; // id del hilo creado (pares = generosos, impares = competitivos)
    unsigned int seed = time(NULL) ^ id;  // semilla diferente por hilo con un usando el id que es unico para cada

    struct timeval inicio, fin;
    gettimeofday(&inicio, NULL); // medir el tiempo de inicio

    unsigned long long dentro = 0; // calculo del montecarlo

    for (unsigned long long i = 0; i < TOTAL_PUNTOS; i++) {
        double x = (double)rand_r(&seed) / RAND_MAX;
        double y = (double)rand_r(&seed) / RAND_MAX;

        if (x*x + y*y <= 1.0){
            dentro++;
        }
        // los hilos pares son generosos y ceden la CPU periódicamente
        if (id % 2 == 0 && i % YIELD_INTERVAL == 0){ // mirar si es hilo generoso (si es par) y si toca ceder
            sched_yield(); // función para ceder la CPU voluntariamente
        }
    }

    double pi = 4.0 * (double)dentro / (double)TOTAL_PUNTOS; // estima pi pelo quotiente dentro/total de puntos * 4

    gettimeofday(&fin, NULL); // medir el tiempo de fin
    double tiempo = (fin.tv_sec - inicio.tv_sec) +(fin.tv_usec - inicio.tv_usec) / 1e6; // poner en segundos y microsegundos la diferencia de tiempo

    if (id % 2 == 0){
        printf("Hilo %d (Generoso): π ≈ %.10f | Tiempo = %.6f s\n", id, pi, tiempo);
    }else{
        printf("Hilo %d (Competitivo): π ≈ %.10f | Tiempo = %.6f s\n", id, pi, tiempo);
    }
    free(arg);  // liberar la memoria reservada en main
    pthread_exit(NULL); // terminar el hilo
}

int main() {
// todos os hilos compartilham o mesmo espaço de endereços (globais/heap), mas cada um tem stack e contexto próprios; o SO coloca cada hilo na fila de prontos.
// como nao ha variaveis globais, cada hilo tem sua propria copia das variaveis locais (stack) e nao precisa do mutex
// Paralelismo acontece quando o número de hilos é menor ou igual ao número de núcleos, permitindo que vários hilos rodem realmente ao mesmo tempo.
// Já concorrência acontece quando há mais hilos do que núcleos, e o sistema operacional precisa alternar entre eles.
    printf("PID de este processo: %d\n", getpid());
    printf("Aproximación de π con %d hilos.\n\n", NUM_HILOS);

    pthread_t hilos[NUM_HILOS]; // arreglo para almacenar los identificadores de los hilos

    for (int i = 0; i < NUM_HILOS; i++) { // crear los hilos
        int* id = malloc(sizeof(int));
        *id = i;
        pthread_create(&hilos[i], NULL, calcular_pi, id); // pasar el id unico a cada hilo y hacer que cada uno ejecute la función calcular_pi con su valor de id
    }

    for (int i = 0; i < NUM_HILOS; i++)
        pthread_join(hilos[i], NULL); // esperar a que todos los hilos terminen

    printf("\nTodos los hilos han terminado.\n");
    getchar(); // esperar a que el usuario presione una tecla antes de salir
    return 0;
}