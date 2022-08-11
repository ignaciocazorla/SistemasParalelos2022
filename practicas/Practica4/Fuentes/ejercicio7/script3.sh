#!/bin/bash
#SBATCH -N 2
#SBATCH --exclusive
#SBATCH --tasks-per-node=2
#SBATCH -o Salida/output.txt
#SBATCH -e Salida/errores.txt
mpirun  $1 $2 $3

# $1 --> nombre del programa
# $2 --> tamaño de matriz
# $3 --> cantidad de hilos a usar