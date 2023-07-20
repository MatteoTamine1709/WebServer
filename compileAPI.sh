#!/bin/bash

listSo=$(find ./api -name "*.so")
for so in $listSo; do
    echo "Removing $so"
    rm $so
done
listCpp=$(find ./api -name "*.cpp")
for cpp in $listCpp; do
    echo "Compiling $cpp"
    out=$(echo $cpp | sed 's/\.cpp/\.so/g')
    echo "Output: ${out}"
    g++ $cpp -lxml2 -I/usr/include/libxml2 -lpthread -fno-gnu-unique -shared -fPIC -std=c++20 -I./ -o $out &
done

wait