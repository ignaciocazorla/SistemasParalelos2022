#include<stdio.h>
#include<stdlib.h>
#include <sys/time.h>
#include "mmblk.h"
#define N 4
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

//Establece el valor de la matriz en la posicion fila y columna segun el orden que este ordenada
void setValor(double *matriz,int fila,int columna,int orden,double valor){
  if(orden==ORDENXFILAS){
    matriz[fila*N+columna]=valor;
  }else{
    matriz[fila+columna*N]=valor;
  }
}


//Establece el valor de la matriz en la posicion fila y columna segun el orden que este ordenada
void setValorF(int *matriz,int fila,int columna,int orden,int valor){
  if(orden==ORDENXFILAS){
    matriz[fila*N+columna]=valor;
  }else{
    matriz[fila+columna*N]=valor;
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
  int *F, escalar;
  double timetick, timeend;
  unsigned long long *resultadosFib;
  int i,j,n,bs;

  // Validar argumentos al programa
  if ( (argc != 3) || ((n = atoi(argv[1])) <= 0) || ((bs = atoi(argv[2])) <= 0) || ((n % bs) != 0))
  {
    printf("\nError en los parametros. Usage: ./%s N BS (N debe ser multiplo de BS)\n", argv[0]);
    exit(1);
  }

  A=(double*)malloc(sizeof(double)*N*N);
  B=(double*)malloc(sizeof(double)*N*N);
  C=(double*)malloc(sizeof(double)*N*N);
  D=(double*)malloc(sizeof(double)*N*N);
  F=(int*)malloc(sizeof(int)*N*N);
  resultadosFib = (unsigned long long*)malloc(40 * sizeof(unsigned long long));
  AporB=(double*)malloc(sizeof(double)*N*N);
  ABC=(double*)malloc(sizeof(double)*N*N);
  FibF=(double*)malloc(sizeof(double)*N*N);
  DporF=(double*)malloc(sizeof(double)*N*N);
  R = (double*)malloc(sizeof(double)*N*N);


  for(i=0;i<N;i++){
    for(j=0;j<N;j++){
	    setValorF(F,i,j,ORDENXFILAS,5);
	    setValor(A,i,j,ORDENXFILAS,1);
	    setValor(B,i,j,ORDENXCOLUMNAS,2);
	    setValor(C,i,j,ORDENXCOLUMNAS,2);
	    setValor(D,i,j,ORDENXFILAS,1);
      setValor(DporF,i,j,ORDENXFILAS,0);
      setValor(FibF,i,j,ORDENXCOLUMNAS,0);
    }
  }

  initializeResultados(resultadosFib);

  //Inicializa la matriz F con valores random entre 1-40 (por filas)
	for(i=0;i<N;i++){
    for(j=0;j<N;j++){ 
      F[i * N +j] = rand() % 40 + 1;
      //printf("Indice %d %d %d\n", i , j, F[i+j]);
  	}
  }

  // Matriz de prueba para comprobar que la cuenta da correcta !!!
  /*double contador = 1;

  for(i=0;i<N;i++){
    for(j=0;j<N;j++){ 
      FibF[i+N * j] = contador;
      printf("Indice %d %d %f\n", i , j, FibF[i+N *j]);
      contador++;
  	}
  matmulblks(D, FibF, DporF, N, 4);
  resultado = 
                28 32 36 40
                28 32 36 40
                28 32 36 40
                28 32 36 40

  }*/

  timetick = dwalltime();

  //Calcula Fibonacci para F, obtiene min, max y llena la matriz FibF (por columnas)
  int valor, min = 102334156, max = -1, suma = 0;
  for(i=0;i<N;i++){
    for(j=0;j<N;j++){ 
      // Es necesario castear el int a double para que funcione
      valor = resultadosFib[F[i * N +j] - 1];

      if(valor > max){
        max = valor;
      }
      if(valor < min){
        min = valor;
      }

      suma = suma + valor;

      FibF[i+j * N] = (double) valor;
      //printf("Indice %d %d %f\n", i , j, FibF[i+j * N]);
  	}
  }

  //printf("\n\nMIN: %d, MAX: %d.\n\n", min, max);

  //printf("\n\nPROMEDIO: %d\n\n", suma/(N*N));

  escalar = (max * min) / (suma/(N*N));
  
  matmulblks(A, B, AporB, N, 2);
  matmulblks(AporB, C, ABC, N, 2);
  matmulblks(D, FibF, DporF, N, 2);



  //Sumar matrices intermedias ABC y DporF y multiplicar por escalar
  for(i=0;i<N;i++){
    for(j=0;j<N;j++){ 
      R[i*N + j] = ABC[i*N + j] + DporF[i *N + j] * escalar;
    }
  }  
  

  timeend = dwalltime() - timetick;

  //Imprime C
  /*printf("\n\n\nC:\n\n");
  for(i=0;i<N;i++){
    for(j=0;j<N;j++){
    printf("Indice %d %d %f\n", i , j, C[i+j *N]);
    }
  }*/

  // Imprime AporB
  /*printf("\n\n\nAporB:\n\n");
  for(i=0;i<N;i++){
    for(j=0;j<N;j++){
      printf("Indice %d %d %f\n", i , j, AporB[i*N+j]);
    }
  }*/

  // Imprime ABC
  /*printf("\n\n\nABC:\n\n");
  for(i=0;i<N;i++){
    for(j=0;j<N;j++){
      printf("Indice %d %d %f\n", i , j, ABC[i * N+j]);
    }
  }*/

  /*printf("\n\n\nD:\n\n");
  for(i=0;i<N;i++){
    for(j=0;j<N;j++){
      printf("Indice %d %d %f\n", i , j, D[i * N+j]);
    }
  }*/

  /*printf("\n\n\nFibF:\n\n");
  for(i=0;i<N;i++){
    for(j=0;j<N;j++){
      printf("Indice %d %d %f\n", i , j, FibF[i+j * N]);
    }
  }*/

  /*printf("\n\n\nDporF:\n\n");
  for(i=0;i<N;i++){
    for(j=0;j<N;j++){
      printf("Indice %d %d %f\n", i , j, DporF[i * N+j]);
    }
  }*/


  // Imprime tiempo total requerido por el calculo
  printf("Tiempo en segundos: %f.\n", ((double)2*N*N*N)/(timeend*1000000000));

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
