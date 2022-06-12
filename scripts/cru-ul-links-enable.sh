#! /bin/bash

FORCE_CONFIG=""
if [ x"$1" = "x-f" ]; then FORCE_CONFIG="--force-config"; shift; fi

CRU=$1
ENABLE=$2

if [ cru.map -nt env-${CRU}.sh ]; then ./cru-init-env.sh $CRU; fi

source env-${CRU}.sh

if [ x"$ENABLE" = "x0" ]; then
    LINKS_PATT="0x0FFF0FFF"
else
    LINKS_PATT="0x00000000"
fi
echo "Setting links pattern for CRU $CRU to ${LINKS_PATT}"
echo "roc-reg-write --id=${CRU_PCI_ADDR} --channel=2 --address=0xc80008 --value=\"${LINKS_PATT}\""
roc-reg-write --id=${CRU_PCI_ADDR} --channel=2 --address=0xc80008 --value="${LINKS_PATT}"
