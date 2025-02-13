#! /bin/bash

cd ../tools/command-line && rm -rf build && mkdir -p build && cd build && cmake3 .. && make -j
