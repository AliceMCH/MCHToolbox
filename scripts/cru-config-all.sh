#! /bin/bash

CRU=$1

./cru-init-env.sh $CRU
./cru-config.sh $CRU

./cru-fec-config.sh $CRU
