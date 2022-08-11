#!/bin/bash
#SBATCH -N 1
#SBATCH --exclusive
#SBATCH -o Salidas/output.txt
#SBATCH -e Errores/errores.txt
if [ $# -ne 4 ];then
    echo "USAGE: program"
    echo "       N (N es el tamanio de la matriz)"
    echo "       BS (block size)"
    echo "       numThreads"

    exit

fi

$1 $2 $3 $4

# 1 --> ./nombrePrograma
# 2 --> N : Tamaño de la entrada
# 3 --> bs : tamaño del bloque para la multiplicacion
# 4 --> numThreads : numero de hilos