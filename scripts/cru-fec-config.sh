#! /bin/bash

CRU=$1

./cru-init-env.sh $CRU
./cru-solar-config.sh $CRU && sleep 5 && ./cru-ds-config.sh $CRU
./cru-ul-reset.sh $CRU
