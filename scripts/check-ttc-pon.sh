#! /bin/bash

for ADDR in 3b 3c af b0 d8 d9; do
#for ADDR in d8 d9; do

    rm ttc-log-${ADDR}.txt

    for n in $(seq 1 10); do
	roc-status --id ${ADDR}:0.0 --onu | grep "quality:" >> ttc-log-${ADDR}.txt
	sleep 10
    done

done
