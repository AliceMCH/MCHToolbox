#! /bin/bash

MODE=$1

if [ x"$MODE" = "x" ]; then
    echo "Usage: ./set-bc-mode.sh [1,2,3]"
    exit 1
fi

echo "BC_MODE=$MODE"
echo "BC_MODE=$MODE" > bcmode.sh
