#! /bin/bash

CRU=$1

./cru-init-env.sh $CRU
./cru-config.sh $CRU || exit 1

./cru-fec-config.sh $CRU || exit 1
