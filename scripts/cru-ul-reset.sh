#! /bin/bash

CRU=$1

SCRIPTDIR=$(readlink -f $(dirname $0))
source ${SCRIPTDIR}/env-${CRU}.sh

echo "roc-reg-write --id=${CRU_PCI_ADDR} --channel=2 --address=0xc80000 --value=\"0x3\""
roc-reg-write --id=${CRU_PCI_ADDR} --channel=2 --address=0xc80000 --value="0x3"

sleep 1

echo "roc-reg-write --id=${CRU_PCI_ADDR} --channel=2 --address=0xc80000 --value=\"0x0\""
roc-reg-write --id=${CRU_PCI_ADDR} --channel=2 --address=0xc80000 --value="0x0"

