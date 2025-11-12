#include <stdio.h>
#include <stdbool.h>

/* ====== Infra mínima de "green threads" ====== */
typedef struct {
    int i;
    int limit;
    int pc;    // "program counter" cooperativo (estado)
    bool done;
    const char* name;
} Task;

/* Macro súper simple para simular puntos de reentrada */
#define BEGIN switch(t->pc){ case 0:
#define YIELD do { t->pc=__LINE__; return; case __LINE__:; } while(0)
#define END } t->done = true; return

/* Dos tareas que imprimen números y ceden voluntariamente */
void taskA(Task* t) {
    BEGIN;
    for (t->i = 0; t->i < t->limit; t->i++) {
        printf("%s: %d\n", t->name, t->i);
        YIELD;  // Cede al "scheduler"
    }
    END;
}

void taskB(Task* t) {
    BEGIN;
    for (t->i = 0; t->i < t->limit; t->i++) {
        printf("%s: %d\n", t->name, t->i);
        YIELD;  // Cede al "scheduler"
    }
    END;
}

/* Scheduler round-robin cooperativo */
int main(void) {
    Task A = {.limit = 10, .pc = 0, .done = false, .name = "Tarea A"};
    Task B = {.limit = 10, .pc = 0, .done = false, .name = "Tarea B"};

    while (!A.done || !B.done) {
        if (!A.done) taskA(&A);  // corre hasta que ceda
        if (!B.done) taskB(&B);  // corre hasta que ceda
    }
    return 0;
}
