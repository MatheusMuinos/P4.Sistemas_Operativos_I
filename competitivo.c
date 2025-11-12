#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void* worker(void* arg) {
    const char* name = (const char*)arg;
    for (int i = 0; i < 10; i++) {
        printf("%s: %d\n", name, i);
        // Simula trabajo; el planificador puede interrumpir aquí o en cualquier instrucción
        usleep(1000 * 5); // 50 ms
    }
    return NULL;
}

int main(void) {
    pthread_t a, b;
    pthread_create(&a, NULL, worker, "Hilo A");
    pthread_create(&b, NULL, worker, "Hilo B");
    pthread_join(a, NULL);
    pthread_join(b, NULL);
    return 0;
}
