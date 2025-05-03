#! /bin/bash

OFFS=$1

if [ x"$OFFS" = "x" ]; then
    echo "Usage: ./set-zs-offset.sh OFFSET"
    exit 1
fi

echo "ZS_OFFSET=$OFFS"
echo "ZS_OFFSET=$OFFS" > zsoffset.sh
