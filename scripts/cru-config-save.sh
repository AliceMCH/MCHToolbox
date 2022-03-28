#! /bin/bash

CRU=$1

if [ cru.map -nt env-${CRU}.sh ]; then ./cru-init-env.sh $CRU; fi

source env-${CRU}.sh

PREFIX="cru-${RUNTYPE}-${CRU_SN}"

CRUCONF=$(./cru-config-gen.sh $CRU)
CRUCONF1=$(echo "$CRUCONF" | sed -n 1p)
CRUCONF2=$(echo "$CRUCONF" | sed -n 2p)

echo "roc-config $CRUCONF1 ${PREFIX}-0.cfg"
roc-config $CRUCONF1 --gen-cfg-file ${PREFIX}-0.cfg

echo "roc-config $CRUCONF2 ${PREFIX}-1.cfg"
roc-config $CRUCONF2 --gen-cfg-file ${PREFIX}-1.cfg
