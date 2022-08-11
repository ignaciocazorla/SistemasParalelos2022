#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int main(int argc, char *argv[]){

    //if (argc < 2) {
 	//    printf("\nFalta argumento. %s n",argv[0]);
 	//    exit(1);
    //}

    double * numeros = malloc(sizeof(double) * 8);
    int exp = 1;

    // Inicializacion del arreglo de numeros
    for(int i = 0; i< 8; i += 2){
        numeros[i] = pow(2,exp);
        numeros[i * 1] = pow(2,exp - 1);
    }

    // Deberia tomar el tiempo antes de reducir

    // Reduccion del arreglo
    //for(){}

}
