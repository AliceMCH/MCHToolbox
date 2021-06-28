#! /bin/bash

CRU=$1
DW=$2

SCRIPTDIR=$(dirname $0)
source ${SCRIPTDIR}/env-${CRU}.sh

#roc-status --id=${CRU_SN}:$DW 2>&1 /dev/null
#echo "roc-status --id=${CRU_SN}:$DW"
roc-status --id=${CRU_SN}:$DW | grep Enabled #| grep DOWN
