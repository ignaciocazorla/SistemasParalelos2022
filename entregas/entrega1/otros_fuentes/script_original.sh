#!/bin/bash

n=512

while [ $n -le 512 ]
do
    bs=2
    while [ $bs -le $n ]
    do
        echo "Ejecucion N=$n BS=$bs"
        ./entrega1 $n $bs
        pid=$!

        wait $pid
        bs=$(expr $bs \* 2)
    done
    n=$(expr $n \* 2)
done