#! /bin/bash

CRU=$1

SCRIPTDIR=$(dirname $0)
source ${SCRIPTDIR}/env-${CRU}.sh

roc-pat-player --id=${CRU_PCI_ADDR} --idle=${PATT_IDLE} --reset 0x0 --reset-length=3000  --trigger-reset
