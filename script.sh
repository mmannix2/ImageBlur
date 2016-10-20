#!/bin/bash

# Builds and runs image_blur with various number of threads

executable="image_blur"
infile="tower.ppm"
outfile="tower_test.ppm"
num_threads=( 2 4 8 16 )

make clean
make
echo "Running with 1 Thread:"
./$executable $infile $outfile

for i in "${num_threads[@]}"
do
    echo ""
    make clean
    make threads THREADS=$i
    echo "Running with $i Threads:"
    ./$executable $infile $outfile
done
