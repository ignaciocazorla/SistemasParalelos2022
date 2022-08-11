#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>


int N,M, size_vector_resultado = 0;
int * vector1, *vector2, *vector_resultado;
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

void * interseccion(void * arg){
    struct args * a = (struct args *) arg;  
    int inicio, fin, min = 101, max = 0, suma = 0;
    inicio = a->tamanio_particion * a->id;
    fin = inicio + a->tamanio_particion;
    int size_vector_tmp = 0;
    int * vector_tmp = NULL;

    for(int i = inicio; i < fin; i++){
		for (int j = 0; j < M; j++){
			if(vector1[i] == vector2[j]){
			  sem_wait(&sem);
				size_vector_resultado += 1;
				vector_resultado = (int *) realloc(vector_tmp, sizeof(int)* size_vector_resultado);
				vector_resultado[size_vector_resultado - 1] = vector2[j];
				sem_post(&sem);				
			}
		}
    }

    pthread_exit(NULL);

}


int main(int argc, char* argv[]){

    int i, T, particion;
    double timetick;

    //Controla los argumentos al programa
    if ((argc != 4) || ((N = atoi(argv[1])) <= 0) || ((M = atoi(argv[2])) <= 0) || ((T = atoi(argv[3])) <= 0) )
     {
       printf("\nUsar: %s n m t\n  n: Tamanio del arreglo 1\n  m: Tamanio del arreglo 2\n  t: Cantidad de threads\n", argv[0]);
       exit(1);
     }

    struct args arguments[T];
    pthread_attr_t attr;
    pthread_t threads[T];

    pthread_attr_init(&attr);

    sem_init(&sem,NULL,1);

    vector1 = (int *) malloc(sizeof(int)*N);
    vector2 = (int *) malloc(sizeof(int)*M);

    //Inicializacion de los vectores con numeros random entre 1 y 100
    for(i = 0; i < N; i++){
       vector1[i] = rand() % 100;
       //printf("%d\n", vector1[i]);
    }

    for(i = 0; i < N; i++){
       vector1[i] = rand() % 100;
    }

    particion = (N / T);

    timetick = dwalltime();

    //Crea los hilos
    for ( i = 0; i < T; i++)
    {
      //printf("Thread %d", i+ 1);
      arguments[i].id = i;
      arguments[i].tamanio_particion = particion;
      pthread_create(&threads[i], &attr, interseccion, &arguments[i]);
    }

    //Espera la terminacion de todos los hilos
    for(i = 0; i < T; i++){
      pthread_join(threads[i], NULL);
    }

    printf("Interseccion de vectores.\n");

    printf("Tiempo en segundos %f\n", dwalltime() - timetick);

    sem_destroy(&sem);

    // Checkeo con una recorrida secuencial
    /*for(i = 0; i<N; i++){
         // TO DO...
    }*/


    return 0;
}
