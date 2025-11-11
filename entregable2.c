//Código hecho por Daniel Lobato De La Fuente y Matheus Muiños Kruschewsky

/*Al aumentar el número de hilos por encima del número de núcleos lógicos, el tiempo no empeora significativamente, 
e incluso puede ser ligeramente mejor debido a la forma en que el sistema operativo distribuye las tareas y a posibles optimizaciones 
del planificador de hilos.
En conclusion, el mejor rendimiento se obtiene con un número de hilos mayor del numero de cores pero sin sobrepasar un valor(normalmente alto p.ej. 1000),
a partir del cual el redimiento empeora porque se sobrecarga el planificador.*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#include <math.h>
#include <sched.h>   // para sched_yield()
#define N 1000000000
#define H 100
#define PI 3.14159265358979323846

double sumas[H];//array para guardar los resultados parciales
void* calcular_serie(void* arg){
    int id= *(int*)arg;
    free(arg);
    unsigned long long base= N/H;
    unsigned long long resto =N%H;

    unsigned long long inicio, fin;

    if (id == 0) {
        inicio = 1;
        fin = base + resto;
    } else {
        inicio = id * base + resto + 1;
        fin = (id + 1) * base + resto;
    }

    double suma_local=0.0;
    for(unsigned long long i=inicio;i<=fin;i++){
        suma_local+= 1.0/pow((double)i,2.0);
    }
    sumas[id]=suma_local;
    pthread_exit(NULL); 
}
int main(){
    printf("Calculo de la serie de Basilea con %d hilos\n\n",H);
    pthread_t hilos[H];
    struct timeval inicio, fin;
    double tiempo_paralelo, tiempo_secuencial;
    gettimeofday(&inicio, NULL);
    for(int i=0;i<H;i++){
        int *id=malloc(sizeof(int));
        *id=i;
        pthread_create(&hilos[i],NULL,calcular_serie,id);
    }

    for(int i=0;i<H;i++){
        pthread_join(hilos[i],NULL);
    }
    gettimeofday(&fin, NULL);
    tiempo_paralelo = (fin.tv_sec - inicio.tv_sec) * 1000.0; // segundos → milisegundos
    tiempo_paralelo += (fin.tv_usec - inicio.tv_usec) / 1000.0;

    printf("Sumas parciales calculadas por los hilos.\n");

    //suma total a partir de las contribuciones de  los hilos

    double suma_total=0.0;
    for(int i=0;i<H;i++){
        suma_total+=sumas[i];
    }
    
    //suma secuencial
    gettimeofday(&inicio, NULL);
    double suma_secuencial=0.0;
    for(unsigned long long i=0;i<=N;i++){
        suma_secuencial+=1.0/pow((double)(i+1),2.0);
    }
    gettimeofday(&fin, NULL);
    tiempo_secuencial = (fin.tv_sec - inicio.tv_sec) * 1000.0;
    tiempo_secuencial += (fin.tv_usec - inicio.tv_usec) / 1000.0;

    double basilea=(PI*PI)/6.0;

    printf("Resultados:\n");
    printf("Suma en paralelo:%.15f\n",suma_total);
    printf("Suma secuencial:%.15f\n",suma_secuencial);
    printf("Valor exacto:%.15f\n\n",basilea);

    printf("Tiempo paralelo: %.3f ms\n", tiempo_paralelo);
    printf("Tiempo secuencial: %.3f ms\n\n", tiempo_secuencial);

    printf("Diferencia paralelo-secuencial:%.15f\n",fabs(suma_secuencial-suma_total));
    printf("Diferencia paralelo-exacto:%.15f\n",fabs(basilea-suma_total));
    printf("Diferencia secuencial-exacto:%.15f\n",fabs(basilea-suma_secuencial));
    exit(0);

}