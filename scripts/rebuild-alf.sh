#! /bin/bash

cd ../tools/command-line/ALF
if [ ! -e build ]; then
    mkdir build && cd build && cmake3 .. && cd .. || exit 1
fi
cd build && make -j
