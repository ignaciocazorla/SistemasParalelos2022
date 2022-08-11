#include <stdio.h>
#include <stdlib.h>
#include "mmblk.h"
#include <mpi.h>
#include <omp.h>

#define COORDINATOR 0

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

void imprimir_matriz(double * matriz, int n, int cant_filas){
  int i,j;

  for (i = 0; i < cant_filas; i++)
  {
    for (j = 0; j < n; j++)
    {
      printf("Posicion %d, %d, value: %f\n", i, j, matriz[i*n + j]);
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
void initializeResultados(unsigned long long * resultadosFib){  
  int i;
  #pragma omp for private(i) schedule(static)
  for(i=0;i < 40;i++){
		resultadosFib[i] = fibIterativo(i + 1);
	}
}

void suma_parcial(double ** m1, double **m2, double **m3, int n, int stripSize, int escalar){
  double * matriz1 = (double *) * m1;
  double * matriz2 = (double *) * m2;
  double * matriz3 = (double *) * m3;
  int i,j;

  #pragma omp for private(i,j) schedule(static) nowait
  for (i=0; i<stripSize; i++) {
      for (j=0; j<n ;j++ ) {
          matriz3[i*n+j] = (matriz1[i*n+j] + matriz2[i*n+j]) * escalar; 
      }
  }
}

int main(int argc, char *argv[]){
  double *A,*B,*C,*D,*AporB, *ABC, *FibF, *DporF, *R;
  int *F, min = 102334156, max = -1, suma = 0, valor;
  int myrank, size, porcion;
  unsigned long long *resultadosFib;
  int i,j,n,bs,escalar = 0;
  double commTimes[6], commTime, totalTime;
  int provided;

  // Validar argumentos al programa
  if ( (argc != 3) || ((n = atoi(argv[1])) <= 0) || ((bs = atoi(argv[2])) <= 0) || ((n % bs) != 0))
  {
    printf("\nError en los parametros. Usage: ./%s N BS (N debe ser multiplo de BS)\n", argv[0]);
    exit(1);
  }

  /* MPI_Init returns once it has started up processes */
  MPI_Init_thread(&argc,&argv, MPI_THREAD_MULTIPLE, &provided);

  /* size and rank will become ubiquitous */ 
  MPI_Comm_size( MPI_COMM_WORLD, &size );
  MPI_Comm_rank( MPI_COMM_WORLD, &myrank );

  // Calcular porcion de matriz que se reparte a cada proceso
  porcion = n / size;

  // Utilizar bs mas chico en caso de que la porcion que se reparte sea menor que bs
  if(bs > porcion){
    bs = porcion;
  }

// Reservar memoria
  if (myrank == COORDINATOR)
  {
    A = (double *) malloc(n * n * sizeof(double));
    D = (double *) malloc(n * n * sizeof(double));
    AporB = (double *) malloc(n * n * sizeof(double));
    ABC = (double *) malloc(n * n * sizeof(double));
    DporF = (double *) malloc(n * n * sizeof(double));
    F = (int *) malloc(sizeof(int)*n*n);
    R = (double *) malloc(n * n * sizeof(double));
  } else {
    A=(double*)malloc(sizeof(double)*n*porcion);
    D=(double*)malloc(sizeof(double)*n*porcion);
    AporB=(double*)malloc(sizeof(double)*n*porcion);
    ABC=(double*)malloc(sizeof(double)*n*porcion);
    DporF=(double*)malloc(sizeof(double)*n*porcion);
    F = (int *) malloc(sizeof(int)*n*porcion);
    R = (double*)malloc(sizeof(double)*n*porcion);
  }
   B = (double *) malloc(n * n * sizeof(double));
   C = (double *) malloc(n * n * sizeof(double));
   FibF=(double*)malloc(sizeof(double)*n*n);
   resultadosFib = (unsigned long long*)malloc(40 * sizeof(unsigned long long));

  //Inicializa las matrices A, D por filas y C, D y F por columnas
  if (myrank == COORDINATOR) {
    for(i=0;i<n;i++){
      for(j=0;j<n;j++){ 
        A[i * n +j] = 1;
        B[i + j * n] = 2;
        C[i + j * n] = 3;
        D[i * n + j] = 1;
        //F[i +j * n] = rand() % 40 + 1;
        F[i +j * n] = 7;     //--> resultadosFib[7 - 1] = 13
      }
    }
  }

  //Comunicaciones 1
  commTimes[0] = MPI_Wtime();
  MPI_Scatter(A, n * porcion, MPI_DOUBLE, A, n * porcion, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);
  MPI_Bcast(B, n * n, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);
  MPI_Bcast(C, n * n, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);
  MPI_Scatter(D, n * porcion, MPI_DOUBLE, D, n * porcion, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);
  MPI_Scatter(F, n * porcion, MPI_INT, F, n * porcion, MPI_INT, COORDINATOR, MPI_COMM_WORLD);
  commTimes[1] = MPI_Wtime();
  //Fin comunicaciones 1

  #pragma omp parallel num_threads(8)
  {

    initializeResultados(resultadosFib);

    #pragma omp for private(i, j, valor) schedule(static) reduction(+:suma) reduction(max:max) reduction(min:min)
    for(i=0;i<porcion;i++){
      for(j=0;j<n;j++){ 
      
        valor = resultadosFib[F[i * n + j] - 1];

        if(valor > max){
          max = valor;
        }
        if(valor < min){
          min = valor;
        }

        suma = suma + valor;

        // Es necesario castear el int a double para que funcione
        FibF[i * n+j] = (double) valor;
  	  }
    }

    //Comunicaciones 2
    commTimes[2] = MPI_Wtime();
    if(omp_get_thread_num() == COORDINATOR){
      MPI_Allgather(FibF, n * porcion, MPI_DOUBLE, FibF, n * porcion, MPI_DOUBLE, MPI_COMM_WORLD);
      MPI_Allreduce(&max, &max, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
      MPI_Allreduce(&min, &min, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD);
      MPI_Allreduce(&suma, &suma, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
      escalar = (max * min) / (suma/(n*n));
    }
    commTimes[3] = MPI_Wtime();
    //Fin comunicaciones 2

    #pragma omp barrier

    matmulblks(A,B,AporB,n,bs,porcion);
    matmulblks(D,FibF,DporF,n,bs,porcion);
    #pragma omp barrier
    matmulblks(AporB,C,ABC,n,bs,porcion);

    #pragma omp barrier
  
    suma_parcial(&ABC, &DporF, &R, n, porcion, escalar);
  }

  //Comunicaciones 3 
  commTimes[4] = MPI_Wtime(); 
  MPI_Gather(R, n * porcion, MPI_DOUBLE, R, n * porcion, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);
  commTimes[5] = MPI_Wtime();
  //Fin comunicaciones 3
  
  MPI_Finalize();

  if(myrank == COORDINATOR){
    checkMatriz(R, n, ((((2 * n) * (3 * n)) + (13 * n)) * escalar));
    //imprimir_matriz(R, n, n);

    // Imprime tiempo total requerido por el calculo y el tiempo de las comunicaciones
    totalTime = commTimes[5] - commTimes[0];
	  commTime = (commTimes[1] - commTimes[0]) + (commTimes[3] - commTimes[2]) + (commTimes[5] - commTimes[4]);		
    printf("Multiplicacion de matrices (N=%d)/(BS=%d)\tTiempo total=%lf\tTiempo comunicacion=%lf\n",n,bs,totalTime,commTime);
  }


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

  return(0);
}
