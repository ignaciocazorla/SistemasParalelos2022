#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "mmblk_pthread.h"
#include <pthread.h>

double *A,*B,*C,*D,*AporB, *ABC, *FibF, *DporF, *R;
int *F;
double timetick, timeend;
unsigned long long *resultadosFib;
int n = 0, bs = 0, numThreads = 0, escalar = 0;
int min_total = 102334156, max_total = -1, suma_total = 0;
pthread_barrier_t barrera;
pthread_mutex_t mutex_fibF;

/**********Para calcular tiempo*************************************/
double dwalltime()
{
  double sec;
  struct timeval tv;

  gettimeofday(&tv,NULL);
  sec = tv.tv_sec + tv.tv_usec/1000000.0;
  return sec;
}
/****************************************************************/

// Para revisar matrices con decimales, ordenadas por filas
void checkMatriz(double * matriz, int n, double num){
  int i,j;
  for (i = 0; i < n; i++)
  {
    for (j = 0; j < n; j++)
    {
      if (matriz[i*n + j] != num)
      {
        printf("Error at %d, %d, value: %f\n", i, j, matriz[i*n + j]);
      }
    }
  }
}

unsigned long long fibIterativo(int n){
  unsigned long long j = 0;
  unsigned long long i = 1;
  unsigned long long k;
  unsigned long long t;

	for(k=1;k<=n;k++){
		t = i + j;
		i = j;
		j = t;
	}
 	return j;
}

// Inicializa los valores de los resultados de Fibonacci para los numeros entre 1-40
void initializeResultados(int inicio, int fin){ 

  for(int i=inicio;i < fin;i++){
    resultadosFib[i] = fibIterativo(i + 1);
    //printf("Indice %d = %lld\n",i, resultadosFib[i]);
	}
}

//Calcula Fibonacci para F, obtiene min, max y llena la matriz FibF (por columnas)
void cargar_FibF(int inicio, int fin){
  int i, j, min_local = 102334156, max_local = -1, valor, suma_local = 0;

    for(i=inicio;i<fin;i++){
      for(j=0;j<n;j++){ 
        valor = resultadosFib[F[i * n +j] - 1];

        if(valor > max_local){
          max_local = valor;
        }
        if(valor < min_local){
          min_local = valor;
        }

        suma_local = suma_local + valor;

        // Es necesario castear el int a double para que funcione
        FibF[i+j * n] = (double) valor;
      }
    }

      // Actualizar maximo, minimo y suma totales

      pthread_mutex_lock(&mutex_fibF);
      if(max_local > max_total){
        max_total = max_local;
      }
      if(min_local < min_total){
        min_total = min_local;
      }

      suma_total = suma_total + suma_local;
      pthread_mutex_unlock(&mutex_fibF);
      
}

//Sumar matrices intermedias ABC y DporF y multiplicar por escalar
void calcular_matriz_R(int inicio, int fin){
   int pos;
   for(int i= inicio; i<fin ;i++){
     for(int j=0;j<n;j++){ 
       pos = i*n + j;
       R[pos] = (ABC[pos] + DporF[pos]) * escalar;
     }
   }
} 


void * realizar_calculo(void * p){

  int id;
  id = *((int *) p);

  int inicio, fin, particion = (40 / numThreads);
  inicio = id * particion;
  fin = inicio + particion;

  initializeResultados(inicio, fin);

  particion = (n / numThreads);
  inicio = id * particion;
  fin = inicio + particion;

  matmulblks(A, B, AporB, n, bs, inicio, fin);
  matmulblks(AporB, C, ABC, n, bs, inicio, fin);

  pthread_barrier_wait(&barrera);

  cargar_FibF(inicio, fin);

  pthread_barrier_wait(&barrera);

  matmulblks(D, FibF, DporF, n, bs, inicio, fin);

  if(id == 0){
    escalar = (max_total * min_total) / (suma_total/(n*n));
    //printf("Escalar = %d\n", escalar);
  }

  pthread_barrier_wait(&barrera);

  calcular_matriz_R(inicio, fin);

  pthread_exit(NULL);

}


int main(int argc, char *argv[]){

  int i,j;

  // Validar argumentos al programa
  if ( (argc != 4) || ((n = atoi(argv[1])) <= 0) || ((bs = atoi(argv[2])) <= 0) || ((n % bs) != 0) || ((numThreads = atoi(argv[3])) <= 0)  || ((numThreads % 2) != 0))
  {
    printf("\nError en los parametros. Usage: %s N BS numThreads (N debe ser multiplo de BS)\n", argv[0]);
    exit(1);
  }

  // ----------------------- Inicializacion de contexto de para los hilos
  pthread_attr_t attr;
  pthread_t threads[numThreads];

  pthread_attr_init(&attr);
  pthread_barrier_init(&barrera, NULL, numThreads);
  pthread_mutex_init(&mutex_fibF, NULL);


  A=(double*)malloc(sizeof(double)*n*n);
  B=(double*)malloc(sizeof(double)*n*n);
  C=(double*)malloc(sizeof(double)*n*n);
  D=(double*)malloc(sizeof(double)*n*n);
  F=(int*)malloc(sizeof(int)*n*n);
  resultadosFib = (unsigned long long*)malloc(40 * sizeof(unsigned long long));
  AporB=(double*)malloc(sizeof(double)*n*n);
  ABC=(double*)malloc(sizeof(double)*n*n);
  FibF=(double*)malloc(sizeof(double)*n*n);
  DporF=(double*)malloc(sizeof(double)*n*n);
  R = (double*)malloc(sizeof(double)*n*n);

  //Inicializa las matrices A, B, C, D y F
	for(i=0;i<n;i++){
    for(j=0;j<n;j++){ 
      A[i * n + j] = 1;
      B[i + j * n] = 2;
      C[i + j * n] = 3;
      D[i * n + j] = 1;
      F[i * n + j] = rand() % 40 + 1;
      //F[i * n + j] = 7;     //--> resultadosFib[7 - 1] = 13
  	}
  }

  int * ids = (int *) malloc(sizeof(int) * numThreads);

  timetick = dwalltime();

  for ( i = 0; i < numThreads; i++){
    ids[i] = i;
    //printf("Thread %d", i+ 1);
    pthread_create(&threads[i], &attr, realizar_calculo, &ids[i]);
  }

  //Espera la terminacion de todos los hilos
  for(i = 0; i < numThreads; i++){
    pthread_join(threads[i], NULL);
  }

  timeend = dwalltime() - timetick;

  /*
  // Tests
  // Es necesario setear la matriz F, de la siguiente manera: F[i * n + j] = 7;     //--> resultadosFib[7 - 1] = 13
  checkMatriz(AporB, n, (2 * n));
  checkMatriz(ABC, n, (2 * n) * (3 * n));
  checkMatriz(DporF, n, (13 * n));
  checkMatriz(R, n, ((((2 * n) * (3 * n)) + (13 * n)) * escalar)); */
  

  // Imprime tiempo total requerido por el calculo
  printf("Tiempo requerido para la operacion: %f\n", timeend);

  //Liberar toda la memoria reservada
  free(A);
  free(B);
  free(C);
  free(D);
  free(F);
  free(AporB);
  free(resultadosFib);
  free(FibF);
  free(DporF);
  free(ABC);
  free(R);
  free(ids);

  // Destruir la barrera
  pthread_barrier_destroy(&barrera);

  return(0);
}
