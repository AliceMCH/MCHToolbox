#! /bin/bash

CRU=$1

SCRIPTDIR=$(readlink -f $(dirname $0))
source ${SCRIPTDIR}/env-${CRU}.sh

FEEID1=$(echo "$CRU * 2" | bc)
FEEID2=$(echo "($CRU * 2) + 1" | bc)
if [ x"${CRU_CSUM_ENABLED}" = "x1" ]; then
    FEEID1=$((FEEID1+256))
    FEEID2=$((FEEID2+256))
fi
FEEIDX1=$(printf '%04x\n' $FEEID1)
FEEIDX2=$(printf '%04x\n' $FEEID2)

echo "Setting FEEIDs for CRU $CRU to 0x${FEEIDX2}${FEEIDX1}"
echo "roc-reg-write --id=${CRU_PCI_ADDR} --channel=2 --address=0xc80004 --value=\"0x${FEEIDX2}${FEEIDX1}\""
roc-reg-write --id=${CRU_PCI_ADDR} --channel=2 --address=0xc80004 --value="0x${FEEIDX2}${FEEIDX1}"
