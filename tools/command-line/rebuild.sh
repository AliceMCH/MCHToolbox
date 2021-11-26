#! /bin/bash

mkdir -p build && cd build && rm -rf CMake* && cmake3 .. && make -j
