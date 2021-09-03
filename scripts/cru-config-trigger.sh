#! /bin/bash

CRU=$1

SCRIPTDIR=$(readlink -f $(dirname $0))
source ${SCRIPTDIR}/env-${CRU}.sh



if [ x"${CRU_CLKSRC}" = "xlocal" ]; then
    roc-ctp-emulator --id=${CRU_PCI_ADDR} --eox

    # Send bunch crossing reset signal
    roc-pat-player --id=${CRU_PCI_ADDR} --idle=${PATT_IDLE} \
	--sync=${PATT_SYNC} \
	--sync-length=1 --sync-delay=0 --sync-trigger-select=4 --trigger-sync
fi


if [ x"$RUNTYPE" = "xpedestals" ]; then
    echo "roc-pat-player --id=${CRU_PCI_ADDR} --idle=${PATT_IDLE} --sync=${PATT_PHYS} --reset=${PATT_SYNC} --sync-length=1 --sync-delay=0 --sync-trigger-select=4 --reset-trigger-select=12"
    roc-pat-player --id=${CRU_PCI_ADDR} --idle=${PATT_IDLE} \
	--sync=${PATT_PHYS} \
        --reset=${PATT_SYNC} \
	--sync-length=1 --sync-delay=0 --sync-trigger-select=4 --reset-trigger-select=12
    exit
fi

if [ x"$RUNTYPE" = "xpedestals_ul" ]; then
    echo "roc-pat-player --id=${CRU_PCI_ADDR} --idle=${PATT_IDLE} --sync=${PATT_PHYS} --reset=${PATT_SYNC} --sync-length=1 --sync-delay=0 --sync-trigger-select=11 --reset-trigger-select=12"
    roc-pat-player --id=${CRU_PCI_ADDR} --idle=${PATT_IDLE} \
	--sync=${PATT_PHYS} \
        --reset=${PATT_SYNC} \
	--sync-length=1 --sync-delay=0 --sync-trigger-select=11 --reset-trigger-select=12
    exit
fi

if [ x"$RUNTYPE" = "xber_check" ]; then
    roc-pat-player --id=${CRU_PCI_ADDR} --idle=${PATT_IDLE} \
	--sync=${PATT_IDLE} \
        --reset=${PATT_IDLE} \
	--sync-length=1 --sync-delay=0 --sync-trigger-select=4 --reset-trigger-select=12
    exit
fi

if [ x"$RUNTYPE" = "xhb_check" ]; then
    roc-pat-player --id=${CRU_PCI_ADDR} --idle=${PATT_IDLE} \
	--sync=${PATT_HBTR} \
        --reset=${PATT_SYNC} \
	--sync-length=1 --sync-delay=0 --sync-trigger-select=4 --reset-trigger-select=12
    exit
fi

SEND_HBT=1
if [ x"$RUNTYPE" = "xpedestals" -o x"$RUNTYPE" = "xpedestals_ul" -o x"$RUNTYPE" = "xber_check" ]; then
    SEND_HBT=0
fi

echo "SEND_HBT: ${SEND_HBT}"

CONTINUOUS=0
if [ x"$RUNTYPE" = "xphysics_continuous_ul" -o x"$RUNTYPE" = "xphysics_continuous_ul_csum" ]; then
    CONTINUOUS=1
fi

echo "CONTINUOUS ${CONTINUOUS}"

if [ x"${CONTINUOUS}" = "x1" ]; then
    echo "roc-pat-player --id=${CRU_PCI_ADDR} --idle=${PATT_IDLE} --sync=${PATT_HBTR} --reset=${PATT_SYNC} --sync-length=1 --sync-delay=0 --sync-trigger-select=11 --reset-trigger-select=12"
    roc-pat-player --id=${CRU_PCI_ADDR} --idle=${PATT_IDLE} \
	--sync=${PATT_HBTR} \
	--reset=${PATT_SYNC} \
	--sync-length=1 --sync-delay=0 --sync-trigger-select=11 --reset-trigger-select=12

    exit
fi


if [ x"${SEND_HBT}" = "x1" ]; then
    echo "roc-pat-player --id=${CRU_PCI_ADDR} --idle=${PATT_IDLE} --sync=${PATT_PHYS} --reset=${PATT_HBTR} --sync-length=1 --sync-delay=0 --sync-trigger-select=4 --reset-trigger-select=11"
    roc-pat-player --id=${CRU_PCI_ADDR} --idle=${PATT_IDLE} \
	--sync=${PATT_PHYS} \
	--reset=${PATT_IDLE} \ #${PATT_HBTR} \
	--sync-length=1 --sync-delay=0 --sync-trigger-select=4 --reset-trigger-select=11
else
    roc-pat-player --id=${CRU_PCI_ADDR} --idle=${PATT_IDLE} \
	--sync=${PATT_PHYS} \
        --reset=${PATT_SYNC} \
	--sync-length=1 --sync-delay=0 --sync-trigger-select=4 --reset-trigger-select=12
fi
