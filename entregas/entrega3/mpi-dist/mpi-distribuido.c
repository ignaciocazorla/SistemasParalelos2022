#include <stdio.h>
#include <stdlib.h>
//#include <sys/time.h>
#include <mpi.h>

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
  for(int i=0;i < 40;i++){
		resultadosFib[i] = fibIterativo(i + 1);
	}
}

void matrizFibonacci(unsigned long long * resultadosFib, int * F, double ** FibonacciF, int n, int * minF, int * maxF, int * sumaF) {
  int valor, i, j, *max, *min, *suma;
  double * FibF;
  FibF = *FibonacciF;
  max = maxF;
  min = minF;
  suma = sumaF;
  for(i=0;i<n;i++){
    for(j=0;j<n;j++){ 
      
      valor = resultadosFib[F[i * n + j] - 1];

      if(valor > *max){
        *max = valor;
      }
      if(valor < *min){
        *min = valor;
      }

      *suma = *suma + valor;

      // Es necesario castear el int a double para que funcione
      FibF[i+j * n] = (double) valor;
  	}
  }
}

void multiplicacion_parcial(double ** m1, double **m2, double **m3, int n, int stripSize){
    double * matriz1 = (double *) * m1;
    double * matriz2 = (double *) * m2;
    double * matriz3 = (double *) * m3;

    /* computar multiplicacion parcial */
    #pragma omp for private(i,j,k) nowait
    for (int i=0; i<stripSize; i++) {
        for (int j=0; j<n ;j++ ) {
            matriz3[i*n+j]=0;
            for (int k=0; k<n ;k++ ) { 
                matriz3[i*n+j] += (matriz1[i*n+k]*matriz2[j*n+k]); 
            }
        }
    }
}

void suma_parcial(double ** m1, double **m2, double **m3, int n, int stripSize, int escalar){
  double * matriz1 = (double *) * m1;
  double * matriz2 = (double *) * m2;
  double * matriz3 = (double *) * m3;

  for (int i=0; i<stripSize; i++) {
      for (int j=0; j<n ;j++ ) {
          matriz3[i*n+j] = (matriz1[i*n+j] + matriz2[i*n+j]) * escalar; 
      }
  }
}

int main(int argc, char *argv[]){
  double *A,*B,*C,*D,*AporB, *ABC, *FibF, *DporF, *R;
  int *F, min = 102334156, max = -1, suma = 0;
  int myrank, size, porcion;
  double timetick, timeend;
  unsigned long long *resultadosFib;
  int i,j,n,escalar = 0;
  double commTimes[6], maxCommTimes[6], minCommTimes[6], commTime, totalTime;

  // Validar argumentos al programa
  if ( (argc != 2) || ((n = atoi(argv[1])) <= 0))
  {
    printf("\nError en los parametros. Usage: ./%s N: tamanio de la matriz\n", argv[0]);
    exit(1);
  }

  /* MPI_Init returns once it has started up processes */
  MPI_Init( &argc, &argv );

  /* size and rank will become ubiquitous */ 
  MPI_Comm_size( MPI_COMM_WORLD, &size );
  MPI_Comm_rank( MPI_COMM_WORLD, &myrank );

  // Calcular porcion de matriz que se reparte a cada proceso
  porcion = n / size;

  // Reservar memoria
  if (myrank == COORDINATOR)
  {
    A = (double *) malloc(n * n * sizeof(double));
    D = (double *) malloc(n * n * sizeof(double));
    AporB = (double *) malloc(n * n * sizeof(double));
    ABC = (double *) malloc(n * n * sizeof(double));
    DporF = (double *) malloc(n * n * sizeof(double));
    R = (double *) malloc(n * n * sizeof(double));
  } else {
    A=(double*)malloc(sizeof(double)*n*porcion);
    D=(double*)malloc(sizeof(double)*n*porcion);
    AporB=(double*)malloc(sizeof(double)*n*porcion);
    ABC=(double*)malloc(sizeof(double)*n*porcion);
    DporF=(double*)malloc(sizeof(double)*n*porcion);
    R = (double*)malloc(sizeof(double)*n*porcion);
  }
   B = (double *) malloc(n * n * sizeof(double));
   C = (double *) malloc(n * n * sizeof(double));
   F = (int *) malloc(n * n * sizeof(int));
   FibF=(double*)malloc(sizeof(double)*n*n);
   resultadosFib = (unsigned long long*)malloc(40 * sizeof(unsigned long long));

  //Inicializa las matrices A, B, C, D y F
  if (myrank == COORDINATOR) {
    for(i=0;i<n;i++){
      for(j=0;j<n;j++){ 
        A[i * n +j] = 1;
        B[i + j * n] = 2;
        C[i + j * n] = 3;
        D[i * n + j] = 1;
        //F[i * n +j] = rand() % 40 + 1;
        F[i * n + j] = 7;     //--> resultadosFib[7 - 1] = 13
      }
    }
  }

  //Comunicaciones 1
  commTimes[0] = MPI_Wtime();
  MPI_Bcast(F, n * n, MPI_INT, COORDINATOR, MPI_COMM_WORLD);
  MPI_Scatter(A, n * porcion, MPI_DOUBLE, A, n * porcion, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);
  MPI_Bcast(B, n * n, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);
  MPI_Bcast(C, n * n, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);
  MPI_Scatter(D, n * porcion, MPI_DOUBLE, D, n * porcion, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);
  commTimes[1] = MPI_Wtime();
  //Fin comunicaciones 1

  initializeResultados(resultadosFib);

  matrizFibonacci(resultadosFib, F, &FibF, n, &min, &max, &suma);

  multiplicacion_parcial(&A, &B, &AporB, n, porcion);
  multiplicacion_parcial(&AporB, &C, &ABC, n, porcion);
  multiplicacion_parcial(&D, &FibF, &DporF, n, porcion);

  //Comunicaciones 2
  commTimes[2] = MPI_Wtime();
  MPI_Allreduce(&max, &max, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
  MPI_Allreduce(&min, &min, 1, MPI_INT, MPI_MIN, MPI_COMM_WORLD);
  MPI_Allreduce(&suma, &suma, 1, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
  commTimes[3] = MPI_Wtime();
  //Fin comunicaciones 2

  escalar = (max * min) / (suma/(n*n));
  
  suma_parcial(&ABC, &DporF, &R, n, porcion, escalar);

  //Comunicaciones 3 
  commTimes[4] = MPI_Wtime(); 
  MPI_Gather(R, n * porcion, MPI_DOUBLE, R, n * porcion, MPI_DOUBLE, COORDINATOR, MPI_COMM_WORLD);
  commTimes[5] = MPI_Wtime();
  //Fin comunicaciones 3

  //MPI_Reduce(commTimes, minCommTimes, 6, MPI_DOUBLE, MPI_MIN, COORDINATOR, MPI_COMM_WORLD);
	//MPI_Reduce(commTimes, maxCommTimes, 6, MPI_DOUBLE, MPI_MAX, COORDINATOR, MPI_COMM_WORLD);
  
  MPI_Finalize();

  if(myrank == COORDINATOR){
    checkMatriz(R, n, ((((2 * n) * (3 * n)) + (13 * n)) * escalar));
    //imprimir_matriz(R, n, n);

    // Imprime tiempo total requerido por el calculo con calculo de las comunicaciones
  totalTime = maxCommTimes[5] - minCommTimes[0];
	commTime = (maxCommTimes[1] - minCommTimes[0]) + (maxCommTimes[3] - minCommTimes[2]) + (maxCommTimes[5] - minCommTimes[4]);		
  printf("Multiplicacion de matrices (N=%d)\tTiempo total=%lf\tTiempo comunicacion=%lf\n",n,totalTime,commTime);
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
