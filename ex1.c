#include <stdio.h>
#include <pthread.h>

int valor_compartilhado = 0; // variável global compartilhada por todas as threads

void* funcao_thread_0(void* arg) {

    int valor_local_t0 = 10; // variável local: privada desta thread

    printf("valor_compartilhado inicial = %d e valor_local_t0 é: %d\n",
           valor_compartilhado, valor_local_t0);

    valor_compartilhado = 42;     // altera a global (visível às outras threads)
    valor_local_t0 = 777;         // altera apenas o local desta thread

    printf("[T0] mudou: valor_compartilhado para: %d e valor_local_t0 para: %d\n",
           valor_compartilhado, valor_local_t0); //printa os valores alterados mantendo o fluxo do compartilhamento
    return NULL;
}

void* funcao_thread_1(void* arg) {

    int valor_local_t1 = 20; // outra pilha, outro 'local'
    printf("valor_compartilhado vindo de T0 = %d e valor_local_t1 é: %d\n",
           valor_compartilhado, valor_local_t1);

           valor_compartilhado = 100; // altera a global novamente
           valor_local_t1 = 888;       // altera apenas o local desta thread

    printf("[T1] mudou: valor_compartilhado para: %d e valor_local_t1 para: %d\n",
           valor_compartilhado, valor_local_t1); //printa os valores alterados mantendo o fluxo do compartilhamento
    return NULL;
}

int main(void) {
    
    pthread_t thread0, thread1;

    printf("[MAIN] inicio: valor_compartilhado = %d\n", valor_compartilhado);

    // Executa T0 primeiro para alterar a global
    pthread_create(&thread0, NULL, funcao_thread_0, NULL);
    pthread_join(thread0, NULL);

    // Depois T1 lê a global já alterada e altera de novo
    pthread_create(&thread1, NULL, funcao_thread_1, NULL);
    pthread_join(thread1, NULL);

    printf("[MAIN] fim: valor_compartilhado = %d\n", valor_compartilhado);
    return 0;
}