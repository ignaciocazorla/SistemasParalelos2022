#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>


int ocurrencias = 0;
int * vector;
pthread_mutex_t ocurrencias_lock;


// Argumentos para la función que recorre el arreglo
struct args
{
    int elemento;
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

void * recolectar_ocurrencias(void * arg){
    struct args * a = (struct args *) arg;  
    //printf("elemento:%d id:%d\n", a->elemento, a->id);
    int inicio, fin, ocurrencias_tmp = 0;
    inicio = a->tamanio_particion * a->id;
    fin = inicio + a->tamanio_particion;

    for(int i = inicio; i < fin; i++){
        if(vector[i] == a->elemento){
            ocurrencias_tmp += 1;
        }
    }
    if(ocurrencias_tmp > 0){
        pthread_mutex_lock(&ocurrencias_lock);
        ocurrencias += ocurrencias_tmp;
        pthread_mutex_unlock(&ocurrencias_lock);
    }

    //printf("Ocurrencias encontradas por el thread %d : %d\n", a->id, ocurrencias_tmp);

    pthread_exit(0);

}


int main(int argc, char* argv[]){

    int i, N, T, elemento, particion, ocurrencias_tmp = 0;
    double timetick;

    //Controla los argumentos al programa
    if ((argc != 4) || ((N = atoi(argv[1])) <= 0) || ((T = atoi(argv[3])) <= 0) )
     {
       printf("\nUsar: %s n x t\n  n: Tamanio del arreglo\n  x: Elemento a buscar\n  t: Cantidad de threads\n", argv[0]);
       exit(1);
     }

    //int indexes[T];
    struct args arguments[T];
    pthread_attr_t attr;
    pthread_t threads[T];

    // No considera que el numero este entre 1 y 100
    elemento = atoi(argv[2]);

    pthread_attr_init(&attr);
    pthread_mutex_init(&ocurrencias_lock, NULL);

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
      //indexes[i] = (N / T) * i;
      arguments[i].elemento = elemento;
      arguments[i].id = i;
      arguments[i].tamanio_particion = particion;
      pthread_create(&threads[i], &attr, recolectar_ocurrencias, &arguments[i]);
    }

    //Espera la terminacion de todos los hilos
    for(i = 0; i < T; i++){
      pthread_join(threads[i], NULL);
    }

    printf("Contador de ocurrencias en arreglo de tamanio %d. Tiempo en segundos %f\n",N, dwalltime() - timetick);

    printf("Ocurrencias del numero %d : %d\n", elemento, ocurrencias);

    // Checkeo de que la cantidad de ocurrencias es correcta.
    for(i = 0; i<N; i++){
         if(vector[i] == elemento){
            ocurrencias_tmp += 1;
        }
    }

    if(ocurrencias_tmp == ocurrencias){
        printf("Conteo de ocurrencias correcto.\n");
    }else{
        printf("Conteo de ocurrencias incorrecto.\n");
    }

    return 0;
}