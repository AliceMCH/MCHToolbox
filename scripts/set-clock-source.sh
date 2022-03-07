#! /bin/bash

CLKSRC=$1

if [ x"$CLKSRC" = "x" ]; then
    echo "Usage: ./set-clock-source.sh [local,ttc]"
    exit 1
fi

echo "CRU_CLKSRC=$CLKSRC"
echo "CRU_CLKSRC=$CLKSRC" > clksource.sh

#./flp-init-env.sh
#./flp-config.sh -f
