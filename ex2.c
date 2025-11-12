#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void* crear_hilo(void* arg) {
    long id = (long)arg;
    printf("[crear_Hilo %ld] iniciado (PID=%d)\n", id, getpid());

    if (id == 3) {
        printf("[Hilo %ld] haciendo fork()\n", id);
        pid_t pid = fork();
        if (pid == 0) {
            printf("[Hijo] PID=%d, PPID=%d (creado por Hilo %ld)\n", getpid(), getppid(), id);
            _exit(0);
        } else {
            int status = 0;
            waitpid(pid, &status, 0);
            printf("[Hilo %ld] hijo %d terminó (status=%d)\n", id, (int)pid, status);
        }
    }
    return NULL;
}

int main(void) {
    pthread_t t1, t2, t3;

    pthread_create(&t1, NULL, crear_hilo, (void*)1L);
    pthread_create(&t2, NULL, crear_hilo, (void*)2L);
    pthread_create(&t3, NULL, crear_hilo, (void*)3L);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    pthread_join(t3, NULL);

    printf("[Main] fin (PID=%d)\n", getpid());
    return 0;
}