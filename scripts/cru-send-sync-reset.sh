#! /bin/bash

CRU=$1

SCRIPTDIR=$(readlink -f $(dirname $0))
source ${SCRIPTDIR}/env-${CRU}.sh



if [ x"${CRU_CLKSRC}" = "xlocal" ]; then

    # Send bunch crossing reset signal
    echo "Sending FE reset signal"
    echo "roc-pat-player --id=${CRU_PCI_ADDR} --idle=${PATT_IDLE} --sync=${PATT_SYNC} --sync-length=1 --sync-delay=0 --sync-trigger-select=4 --trigger-sync"
    roc-pat-player --id=${CRU_PCI_ADDR} --idle=${PATT_IDLE} \
	--sync=${PATT_SYNC} \
	--sync-length=1 --sync-delay=0 --sync-trigger-select=4 --trigger-sync

    ./cru-config-trigger.sh $CRU
fi
