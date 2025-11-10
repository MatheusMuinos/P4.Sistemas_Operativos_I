#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <sched.h>

#define NUM_HILOS 8            // ajustável
#define TOTAL_PUNTOS 1000000000ULL  // 1e9 pontos por hilo (ajustável)
#define YIELD_INTERVAL 100000   // a cada n iterações, hilos generosos cedem a CPU

typedef struct {
    int id;
    int generoso;     // 1 = par (generoso), 0 = ímpar (competitivo)
    double pi;
    double segundos;
} Stat;

static Stat stats[NUM_HILOS];

static inline unsigned int xorshift32(unsigned int *s) {
    unsigned int x = *s;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *s = x;
    return x;
}

void* worker(void* arg) {
    int id = *(int*)arg;
    free(arg);

    unsigned int seed = (unsigned int)time(NULL) ^ (unsigned int)(id * 2654435761u);

    struct timespec t0, t1;
    clock_gettime(CLOCK_MONOTONIC, &t0);

    unsigned long long dentro = 0ULL;
    int generoso = (id % 2 == 0);

    for (unsigned long long i = 0; i < TOTAL_PUNTOS; i++) {
        // PRNG em [0,1)
        double x = (double)xorshift32(&seed) / (double)UINT_MAX;
        double y = (double)xorshift32(&seed) / (double)UINT_MAX;

        if (x*x + y*y <= 1.0) dentro++;

        if (generoso && (i % YIELD_INTERVAL == 0)) {
            sched_yield();
        }
    }

    double pi = 4.0 * (double)dentro / (double)TOTAL_PUNTOS;

    clock_gettime(CLOCK_MONOTONIC, &t1);
    double dt = (t1.tv_sec - t0.tv_sec) + (t1.tv_nsec - t0.tv_nsec) / 1e9;

    stats[id].id = id;
    stats[id].generoso = generoso;
    stats[id].pi = pi;
    stats[id].segundos = dt;

    pthread_exit(NULL);
}

static int cmp_stats(const void* a, const void* b) {
    const Stat* A = (const Stat*)a;
    const Stat* B = (const Stat*)b;
    if (A->segundos < B->segundos) return -1;
    if (A->segundos > B->segundos) return 1;
    return 0;
}

int main() {
    printf("Aproximación de π con %d hilos (TOTAL_PUNTOS por hilo = %llu)\n\n", NUM_HILOS, (unsigned long long)TOTAL_PUNTOS);

    pthread_t th[NUM_HILOS];

    for (int i = 0; i < NUM_HILOS; i++) {
        int* pid = (int*)malloc(sizeof(int));
        *pid = i;
        pthread_create(&th[i], NULL, worker, pid);
    }

    for (int i = 0; i < NUM_HILOS; i++) {
        pthread_join(th[i], NULL);
    }

    // ordenar por tempo crescente
    qsort(stats, NUM_HILOS, sizeof(Stat), cmp_stats);

    printf("Tabla final (ordenada por tiempo creciente):\n");
    printf("Hilo\tTipo\t\tTiempo(s)\tπ_aprox\n");
    for (int i = 0; i < NUM_HILOS; i++) {
        printf("%d\t%s\t%.6f\t%.10f\n",
               stats[i].id,
               stats[i].generoso ? "Generoso" : "Competitivo",
               stats[i].segundos,
               stats[i].pi);
    }

    return 0;
}
