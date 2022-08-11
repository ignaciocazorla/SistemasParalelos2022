#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

//Dimension por defecto de las matrices
int N=100;

//Numero de Threads
int T=1;

//Como variables globales para simplificar el pasaje de parámetros
double *A,*B,*C;

//Para calcular tiempo
double dwalltime(){
        double sec;
        struct timeval tv;

        gettimeofday(&tv,NULL);
        sec = tv.tv_sec + tv.tv_usec/1000000.0;
        return sec;
}

//void mul(double *A, double *B, double *C, int index){
void * mul(void * n){
  int * p, index;
  p = (int *) n;
  index = *p;

  int pos = (N / T) + index;

  double elemento;

  printf(" index %d y pos %d\n", index, pos);

   // Todas las mejoras de los ejercicios anteriores. (del punto 8 de la practica 1)
  for(int i=index;i<pos;i++){
   for(int j=0;j<N;j++){
    elemento = 0;
    for(int k=0;k<N;k++){
      //printf("Indices: %d %d\n",(i * N + k), (j + k * N));
      elemento += A[i * N + k] * B[j + k * N];
    }
    C[i * N + j] = elemento;
   }
  }  
  pthread_exit(0);
}

int main(int argc,char*argv[]){
 int i,j;
 int check=1;
 double timetick;

 //Para los threads   ------> CAMBIAR LOS TAMAÑOS DE LOS ARREGLOS PARA QUE SE RESUELVAN EN EJECUCION
 int ids[4], indexes[4];
 pthread_attr_t attr;
 pthread_t threads[4];

 pthread_attr_init(&attr);

 //Controla los argumentos al programa
 if ((argc != 3) || ((N = atoi(argv[1])) <= 0) || ((T = atoi(argv[2])) <= 0) )
  {
    printf("\nUsar: %s n t\n  n: Dimension de la matriz (nxn X nxn)\n  t: Cantidad de threads\n", argv[0]);
    exit(1);
  }

 //Aloca memoria para las matrices
  A=(double*)malloc(sizeof(double)*N*N);
  B=(double*)malloc(sizeof(double)*N*N);
  C=(double*)malloc(sizeof(double)*N*N);

 //Inicializa las matrices A y B en 1, el resultado sera una matriz con todos sus valores en N
  for(i=0;i<N;i++){
   for(j=0;j<N;j++){
      A[i * N + j] = 1;
      B[i + j * N] = 1;
   }
  }   

 //Realiza la multiplicacion

 for(i = 0; i < T; i++){
   indexes[i] = (N / T) * i;
   printf("%d\n",indexes[i]);
 }

  timetick = dwalltime();

  //Crea los hilos
  for ( i = 0; i < T; i++)
  {
    ids[i] = i;
    printf("Thread %d", i+ 1);
    pthread_create(&threads[i], &attr, mul, &indexes[i]);
  }
  //mul(A,B,C,0);

  for(i = 0; i < T; i++){
    pthread_join(threads[i], NULL);
  }

 printf("Multiplicacion de matrices de %dx%d. Tiempo en segundos %f\n",N,N, dwalltime() - timetick);

 //Verifica el resultado
  for(i=0;i<N;i++){
   for(j=0;j<N;j++){
	    check=check&&(C[i * N + j] == N);
   }
  }   

  if(check){
   printf("Multiplicacion de matrices resultado correcto\n");
  }else{
   printf("Multiplicacion de matrices resultado erroneo\n");
  }

 free(A);
 free(B);
 free(C);
 return(0);
}
