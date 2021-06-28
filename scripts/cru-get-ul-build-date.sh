#! /bin/bash

CRU=$1

SCRIPTDIR=$(dirname $0)
source ${SCRIPTDIR}/env-${CRU}.sh

echo -n "UL build date for CRU $CRU: "
roc-reg-read --id=${CRU_PCI_ADDR} --channel=2 --address=0xc0008
