#!/bin/bash
#SBATCH -N 1
#SBATCH --exclusive
#SBATCH -o Salidas/output2.txt
#SBATCH -e Errores/errores2.txt
./quadratic2