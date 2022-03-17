#! /bin/bash

CHECK=0
if [ "$1" = "-c" ]; then
    CHECK=1
    shift
fi

CRU=$1
FILE="$2"
if [ x"$FILE" = "x" ]; then
    mkdir -p "$HOME/Data"
    FILE="$HOME/Data/data-${CRU}.raw"
fi

SCRIPTDIR=$(readlink -f $(dirname $0))
source ${SCRIPTDIR}/env-${CRU}.sh


#./cru-config-trigger.sh $CRU

#xterm -bg black -fg white -geometry 100x15+0+780 -e watch -n 0.9 ${SCRIPT_DIR}/check-packets-loss.sh  &

ROC1_EN=0
if [ -n "${CRU_PCI_ADDR1}" ]; then
    ROC1_EN=1
fi

ROC2_EN=0
if [ -n "${CRU_PCI_ADDR2}" -a "${CRU_PCI_ADDR2}" != "${CRU_PCI_ADDR1}" ]; then
    ROC2_EN=1
fi

rm -f "$FILE"
cat readout-file.cfg | sed "s|%OUTFILE%|$FILE|g" | sed "s|%CRU_PCI_ADDR1%|${CRU_PCI_ADDR1}|g" | sed "s|%CRU_PCI_ADDR2%|${CRU_PCI_ADDR2}|g" | sed "s|%ROC1_EN%|${ROC1_EN}|g" | sed "s|%ROC2_EN%|${ROC2_EN}|g" > readout.cfg
xterm -bg black -fg white -geometry 100x45+0+100 -e ./start-readout.sh 0 &

if [ x"${CRU_CLKSRC}" = "xlocal" ]; then
    xterm -bg black -fg white -geometry 100x5+0+720 -e ./cru-run-ctp-emulator.sh $CRU &
fi

sleep 1
while [ ! -e "$FILE" ]; do
    sleep 1
done

sleep 2

while [ ! -e /tmp/readout.done ]; do
    sleep 1
    clear
    #ls -lht ${SCRIPT_DIR}/*.raw | head -n 10
    ls -lht ${FILE} | head -n 10
done


if [ x"${CRU_CLKSRC}" = "xlocal" ]; then
    roc-ctp-emulator --id=${CRU_PCI_ADDR} --eox
fi

if [ x"$CHECK" = "x1" ]; then
    ./cru-check-data.sh $CRU
fi
