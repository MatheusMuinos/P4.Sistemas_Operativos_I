#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

#define N 1000000000ULL   
#define H 14             
#define PI 3.14159265358979323846

static double parcial[H];

typedef struct {
    unsigned long long inicio;
    unsigned long long fin;
    int id;
} Rango;

static Rango rango_de(int id) {
    unsigned long long base = N / H;
    unsigned long long resto = N % H;

    Rango r;
    r.id = id;
    if (id == 0) {
        r.inicio = 1ULL;
        r.fin    = base + resto;
    } else {
        r.inicio = id * base + resto + 1ULL;
        r.fin    = (id + 1) * base + resto;
    }
    return r;
}

static void* hilo_basilea(void* arg) {
    int id = *(int*)arg;
    free(arg);

    Rango r = rango_de(id);

    double s = 0.0;
    for (unsigned long long n = r.inicio; n <= r.fin; n++) {
        // double + pow(), conforme especificacao
        s += 1.0 / pow((double)n, 2.0);
    }
    parcial[id] = s;
    pthread_exit(NULL);
}

int main() {
    printf("Basilea paralelo com H=%d, N=%llu\n\n", H, (unsigned long long)N);

    pthread_t th[H];
    for (int i = 0; i < H; i++) {
        int* pid = (int*)malloc(sizeof(int));
        *pid = i;
        pthread_create(&th[i], NULL, hilo_basilea, pid);
    }

    for (int i = 0; i < H; i++) {
        pthread_join(th[i], NULL);
    }

    // soma total a partir dos parciais
    double soma_paralela = 0.0;
    for (int i = 0; i < H; i++) {
        soma_paralela += parcial[i];
    }

    // conferir que a soma das iterações bate com N
    // {
    //     unsigned long long total_iters = 0ULL;
    //     for (int i = 0; i < H; i++) {
    //         Rango r = rango_de(i);
    //         total_iters += (r.fin - r.inicio + 1ULL);
    //     }
    //     printf("DEBUG total de iteracoes = %llu (esperado %llu)\n",
    //            total_iters, (unsigned long long)N);
    // }

    double soma_sequencial = 0.0;
    for (unsigned long long n = 1ULL; n <= N; n++) {
        soma_sequencial += 1.0 / pow((double)n, 2.0);
    }

    double exato = (PI * PI) / 6.0;

    printf("Resultados:\n");
    printf("Paralelo  : %.15f\n", soma_paralela);
    printf("Sequencial: %.15f\n", soma_sequencial);
    printf("Exato     : %.15f\n\n", exato);

    printf("Diferenças:\n");
    printf("|paralelo - sequencial| = %.15f\n", fabs(soma_paralela - soma_sequencial));
    printf("|paralelo - exato     | = %.15f\n", fabs(exato - soma_paralela));
    printf("|sequencial - exato   | = %.15f\n", fabs(exato - soma_sequencial));

    return 0;
}
