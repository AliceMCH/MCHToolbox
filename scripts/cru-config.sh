#! /bin/bash

FORCE_CONFIG=""
if [ x"$1" = "x-f" ]; then FORCE_CONFIG="--force-config"; shift; fi

CRU=$1

source env-${CRU}.sh

CRUID=$CRU
if [ x"${CRU_CSUM_ENABLED}" = "x1" ]; then
    CRUID=$((CRUID+256))
fi
CRUIDX=$(echo "obase=16; $CRUID" | bc)

# Set window size in triggered mode
#WIN=0x1f40 # 0x1f40 = 8000
#WIN=0xfa0  # 0xfaa0 = 4000
#WIN=4000
WIN=2000
if [ x"${RUNTYPE}" = "xpedestals" ]; then
    WIN=4200
fi
if [ x"${RUNTYPE}" = "xhb_check" ]; then
    WIN=500
fi
#WIN=9500
WINX=$(echo "obase=16; $WIN" | bc)


ONUID=$CRU
RC_OPT="${FORCE_CONFIG} --bypass-fw-check --clock=${CRU_CLKSRC} --datapathmode=STREAMING --gbtmux=ttc --downstreamdata=pattern --cru-id=${CRUIDX} --dyn --trigger-window-size $WIN --pon --onu=${ONUID} --system-id=A --allow-rejection "

if [ x"${CRU_UL_ENABLED}" = "x1" ]; then
    RC_OPT="${RC_OPT} --user-logic"
fi

RC_OPT2="--id=${CRU_SN}:0 --links=${CRU_LINKS2_1}"
#RC_OPT2="--id=${CRU_PCI_ADDR} --links=${CRU_LINKS2_1}"
echo "roc-config $RC_OPT $RC_OPT2"
if [ -n "${CRU_LINKS2_1}" ]; then
    roc-config $RC_OPT $RC_OPT2
fi

RC_OPT2="--id=${CRU_SN}:1 --links=${CRU_LINKS2_2}"
#RC_OPT2="--id=${CRU_PCI_ADDR2} --links=${CRU_LINKS2_2}"
echo "roc-config $RC_OPT $RC_OPT2"
if [ -n "${CRU_LINKS2_2}" ]; then
    roc-config $RC_OPT $RC_OPT2
fi



# EP 0
roc-reg-read --id=${CRU_PCI_ADDR} --channel=2 --address=0x600034
# EP 1
roc-reg-read --id=${CRU_PCI_ADDR} --channel=2 --address=0x700034

if [ x"${CRU_UL_ENABLED}" = "x1" ]; then
    bash ./cru-set-feeid.sh $CRU
fi

# set pat-player in default mode
#echo "roc-pat-player --id=${CRU_PCI_ADDR} --idle=0x0C0300C0300C0300C030 --sync=0x3C0F03C0F03C0F03C0F0 --sync-length=1 --sync-delay=0 --sync-trigger-select=4"
roc-pat-player --id=${CRU_PCI_ADDR} --idle=0x0C0300C0300C0300C030 \
    --sync=0x3C0F03C0F03C0F03C0F0 \
    --sync-length=1 --sync-delay=0 --sync-trigger-select=4
