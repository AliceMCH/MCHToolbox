#! /bin/bash

source /opt/rh/gcc-toolset-12/enable
cd ../tools/command-line && rm -rf build && mkdir -p build && cd build && cmake3 .. && make -j
