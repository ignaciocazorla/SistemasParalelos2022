#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>


int max_total = 0, min_total = 101, suma_total = 0, promedio = 0;
int * vector;
sem_t sem;


// Argumentos para la función que recorre el arreglo
struct args
{
    int id;
    int tamanio_particion;
};


//Para calcular tiempo
double dwalltime(){
        double sec;
        struct timeval tv;

        gettimeofday(&tv,NULL);
        sec = tv.tv_sec + tv.tv_usec/1000000.0;
        return sec;
}

void * realizar_calculos(void * arg){
    struct args * a = (struct args *) arg;  
    //printf("elemento:%d id:%d\n", a->elemento, a->id);
    int inicio, fin, min = 101, max = 0, suma = 0;
    inicio = a->tamanio_particion * a->id;
    fin = inicio + a->tamanio_particion;

    for(int i = inicio; i < fin; i++){

        // Calculo el minimo
        if(vector[i] < min){
            min = vector[i];
        }

        if(vector[i] > max){
            max = vector[i];
        }

        suma += vector[i];
    }

    // Actualizar los datos globales a continuacion
    sem_wait(&sem);
    
    if(max_total < max){
        max_total = max;
    }

    if(min_total > min){
        min_total = min;
    }

    suma_total += suma;

    sem_post(&sem);


    pthread_exit(0);

}


int main(int argc, char* argv[]){

    int i, N, T, particion;
    double timetick;

    //Controla los argumentos al programa
    if ((argc != 3) || ((N = atoi(argv[1])) <= 0) || ((T = atoi(argv[2])) <= 0) )
     {
       printf("\nUsar: %s n t\n  n: Tamanio del arreglo\n  t: Cantidad de threads\n", argv[0]);
       exit(1);
     }

    struct args arguments[T];
    pthread_attr_t attr;
    pthread_t threads[T];

    pthread_attr_init(&attr);

    sem_init(&sem,NULL,1);

    vector = (int *) malloc(sizeof(int)*N);

    //Inicializacion del vector con numeros random entre 1 y 100
    for(i = 0; i < N; i++){
       vector[i] = rand() % 100;
       printf("%d\n", vector[i]);
    }

    particion = (N / T);

    timetick = dwalltime();

    //Crea los hilos
    for ( i = 0; i < T; i++)
    {
      //printf("Thread %d", i+ 1);
      arguments[i].id = i;
      arguments[i].tamanio_particion = particion;
      pthread_create(&threads[i], &attr, realizar_calculos, &arguments[i]);
    }

    //Espera la terminacion de todos los hilos
    for(i = 0; i < T; i++){
      pthread_join(threads[i], NULL);
    }

    printf("Calculos en arreglo de tamaño: %d\n", N);

    printf("Promedio: %d\nMinimo: %d\nMaximo: %d\nTiempo en segundos %f\n", suma_total/N, min_total, max_total, dwalltime() - timetick);

    sem_destroy(&sem);

    // Checkeo con una recorrida secuencial
    /*for(i = 0; i<N; i++){
         // TO DO...
    }*/


    return 0;
}