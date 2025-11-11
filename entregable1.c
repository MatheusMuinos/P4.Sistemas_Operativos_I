//Código hecho por Daniel Lobato De La Fuente y Matheus Muiños Kruschewsky

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>
#include <sched.h>   // para sched_yield()

#define NUM_HILOS 100          // número de hilos (modificable)
#define TOTAL_PUNTOS 100000000   // número de puntos generados por hilo
#define YIELD_INTERVAL 1000   // cada cuántas iteraciones los hilos generosos ceden la CPU

void* calcular_pi(void* arg) {
    int id = *(int*)arg;
    unsigned int seed = time(NULL) ^ id;  // semilla diferente por hilo

    struct timeval inicio, fin;
    gettimeofday(&inicio, NULL);

    unsigned long long dentro = 0;

    for (unsigned long long i = 0; i < TOTAL_PUNTOS; i++) {
        double x = (double)rand_r(&seed) / RAND_MAX;
        double y = (double)rand_r(&seed) / RAND_MAX;

        if (x*x + y*y <= 1.0){
            dentro++;
        }
        // Los hilos pares son "generosos" y ceden la CPU periódicamente
        if (id % 2 == 0 && i % YIELD_INTERVAL == 0){
            sched_yield();
        }
    }

    double pi = 4.0 * (double)dentro / (double)TOTAL_PUNTOS;

    gettimeofday(&fin, NULL);
    double tiempo = (fin.tv_sec - inicio.tv_sec) +(fin.tv_usec - inicio.tv_usec) / 1e6;

    if (id % 2 == 0){
        printf("Hilo %d (Generoso): π ≈ %.10f | Tiempo = %.6f s\n", id, pi, tiempo);
    }else{
        printf("Hilo %d (Competitivo): π ≈ %.10f | Tiempo = %.6f s\n", id, pi, tiempo);
    }
    free(arg);  // liberar la memoria reservada en main
    pthread_exit(NULL);
}

int main() {
    printf("Aproximación de π con %d hilos.\n\n", NUM_HILOS);

    pthread_t hilos[NUM_HILOS];

    for (int i = 0; i < NUM_HILOS; i++) {
        int* id = malloc(sizeof(int));
        *id = i;
        pthread_create(&hilos[i], NULL, calcular_pi, id);
    }

    for (int i = 0; i < NUM_HILOS; i++)
        pthread_join(hilos[i], NULL);

    printf("\nTodos los hilos han terminado.\n");
    return 0;
}