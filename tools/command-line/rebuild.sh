#! /bin/bash

SCRIPTDIR=$(dirname $0)

cd "$SCRIPTDIR" && mkdir -p build && cd build && rm -rf CMake* && cmake3 .. && make -j
