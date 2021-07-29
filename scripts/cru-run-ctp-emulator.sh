#! /bin/bash

CRU=$1

SCRIPTDIR=$(readlink -f $(dirname $0))
source ${SCRIPTDIR}/env-${CRU}.sh


echo ""
echo -n "Press Enter key to start the CTP emulator..."
read dummy

TRIGMODE=periodic
#TRIGMODE=continuous

TRIGRATE=4000000
#TRIGRATE=8000000
#TRIGRATE=700000

HBKEEP=256
HBDROP=$((256-HBKEEP))

echo "roc-ctp-emulator --id=${CRU_PCI_ADDR} --trigger-mode=${TRIGMODE} --trigger-freq=$TRIGRATE"
roc-ctp-emulator --id=${CRU_PCI_ADDR} --trigger-mode=${TRIGMODE} --trigger-freq=$TRIGRATE --hbmax=255 #--hbkeep=$HBKEEP --hbdrop=$HBDROP


echo ""
echo -n "Press Enter key to stop the CTP emulator..."
read dummy

echo "roc-ctp-emulator --id=${CRU_PCI_ADDR} --eox"
roc-ctp-emulator --id=${CRU_PCI_ADDR} --eox
