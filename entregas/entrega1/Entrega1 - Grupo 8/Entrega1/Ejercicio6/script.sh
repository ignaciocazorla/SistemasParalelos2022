#!/bin/bash
#SBATCH -N 1
#SBATCH --exclusive
#SBATCH -o Salidas/output1.txt
#SBATCH -e Errores/errores1.txt
./quadratic1