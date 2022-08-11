#!/bin/bash
#SBATCH -N 1
#SBATCH --exclusive
#SBATCH -o Salidas/output.txt
#SBATCH -e Errores/errores.txt

if [ $# -ne 2 ];then
    echo "USAGE: program N (N es el tamanio de la matriz)"
else
    n=$2
    bs=2
    while [ $bs -le $n ]
    do
        echo "Ejecucion N=$n BS=$bs" >> output_bs.txt
        $1 $n $bs >> output_bs.txt
        pid=$!

        wait $pid
        bs=$(expr $bs \* 2)
    done
fi
