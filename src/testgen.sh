#!/bin/bash

make bin/generator
mkdir -p tests
for i in $(seq 1 10)
do
    ./bin/generator 10000 >tests/test_10000_$i.in
done
for i in $(seq 1 15)
do
    ./bin/generator 200000 >tests/test_200000_$i.in
done
for i in $(seq 1 15)
do
    ./bin/generator 2000000 >tests/test_2000000_$i.in
done
for i in $(seq 1 15)
do
    ./bin/generator 10000000 >tests/test_10000000_$i.in
done

echo OK
