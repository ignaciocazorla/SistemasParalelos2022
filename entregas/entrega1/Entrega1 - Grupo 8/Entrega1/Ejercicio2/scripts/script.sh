#!/bin/bash
#SBATCH -N 1
#SBATCH --exclusive
#SBATCH -o Salidas/output.txt
#SBATCH -e Errores/errores.txt
$1 $2 $3
