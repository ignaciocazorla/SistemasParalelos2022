#include<stdio.h>
#include<stdlib.h>
#include <sys/time.h>
#include "mmblk.h"
#define ORDENXFILAS 0
#define ORDENXCOLUMNAS 1

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
void initializeResultados(unsigned long long * resultadosFib){  
  for(int i=0;i < 40;i++){
		resultadosFib[i] = fibIterativo(i + 1);
    //printf("Indice %d = %d\n",i, resultadosFib[i]);
	}
}

int main(int argc, char *argv[]){
  double *A,*B,*C,*D,*AporB, *ABC, *FibF, *DporF, *R;
  int *F;
  double timetick, timeend;
  unsigned long long *resultadosFib;
  int i,j,n,bs,escalar = 0;

  // Validar argumentos al programa
  if ( (argc != 3) || ((n = atoi(argv[1])) <= 0) || ((bs = atoi(argv[2])) <= 0) || ((n % bs) != 0))
  {
    printf("\nError en los parametros. Usage: ./%s N BS (N debe ser multiplo de BS)\n", argv[0]);
    exit(1);
  }

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
      A[i * n +j] = 1;
      B[i + j * n] = 2;
      C[i + j * n] = 3;
      D[i * n +j] = 1;
      F[i * n +j] = rand() % 40 + 1;
  	}
  }

  timetick = dwalltime();

  initializeResultados(resultadosFib);

  //Calcula Fibonacci para F, obtiene min, max y llena la matriz FibF (por columnas)
  int valor, min = 102334156, max = -1, suma = 0;
  for(i=0;i<n;i++){
    for(j=0;j<n;j++){ 
      
      valor = resultadosFib[F[i * n +j] - 1];

      if(valor > max){
        max = valor;
      }
      if(valor < min){
        min = valor;
      }

      suma = suma + valor;

      // Es necesario castear el int a double para que funcione
      FibF[i+j * n] = (double) valor;
  	}
  }

  escalar = (max * min) / (suma/(n*n));
  
  matmulblks(A, B, AporB, n, bs);
  matmulblks(AporB, C, ABC, n, bs);
  matmulblks(D, FibF, DporF, n, bs);


  //Sumar matrices intermedias ABC y DporF y multiplicar por escalar
  int pos;
  for(i=0;i<n;i++){
    for(j=0;j<n;j++){ 
      pos = i*n + j;
      R[pos] = (ABC[pos] + DporF[pos]) * escalar;
    }
  }  
  

  timeend = dwalltime() - timetick;

  /*
  // Tests
  checkMatriz(AporB, n, 2 * n);
  checkMatriz(ABC, n, (2 * n) * 3 * n);
  checkMatriz(DporF, n, n * n);
  checkMatriz(R, n, (((2 * n) * 3 * n) + n * n) * escalar); */
  

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

  return(0);
}
