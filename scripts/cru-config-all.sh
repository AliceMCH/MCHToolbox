#! /bin/bash

CRU=$1

./cru-config.sh $CRU && ./cru-solar-config.sh $CRU && ./cru-ds-config.sh $CRU
