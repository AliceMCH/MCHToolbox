#! /bin/bash

CRU=$1

./cru-ul-links-enable.sh $CRU 0

./cru-solar-config.sh $CRU && sleep 5 && ./cru-ds-config.sh $CRU

sleep 1
./cru-ul-links-enable.sh $CRU 1

