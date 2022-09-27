#!/bin/bash

if [ $# -eq 1 ];then
    chars=$1;
else
    echo "Missing argument of char sequence";
    exit -1;
fi;

mpic++ --prefix /usr/local/share/OpenMPI -o pro pro.cpp

num_of_processors=$(((${#chars}-1)*2));

mpirun --prefix /usr/local/share/OpenMPI -oversubscribe -np $num_of_processors pro $chars

rm -f pro
