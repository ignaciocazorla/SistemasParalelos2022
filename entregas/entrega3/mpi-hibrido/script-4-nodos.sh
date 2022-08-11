#!/bin/bash
#SBATCH -N 4
#SBATCH --exclusive
#SBATCH --tasks-per-node=1
#SBATCH -o Salida/output.txt
#SBATCH -e Salida/errores.txt
mpirun --bind-to none $1 $2 $3 $4

# $1 --> nombre del programa
# $2 --> tamaño de matriz
# $3 --> tamaño del bloque
# $4 --> numero de hilos
