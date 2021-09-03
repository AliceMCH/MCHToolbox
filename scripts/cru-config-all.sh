#! /bin/bash

CRU=$1

./cru-config.sh $CRU && ./cru-solar-config.sh $CRU && sleep 5 && ./cru-ds-config.sh $CRU
