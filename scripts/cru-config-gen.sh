#! /bin/bash

CRU=$1

if [ cru.map -nt env-${CRU}.sh ]; then ./cru-init-env.sh $CRU; fi

source env-${CRU}.sh

CRUID=$CRU
if [ x"${CRU_CSUM_ENABLED}" = "x1" ]; then
    CRUID=$((CRUID+256))
fi
CRUIDX=$(echo "obase=16; $CRUID" | bc)

# Set window size in triggered mode
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
RC_OPT="--bypass-fw-check --clock=${CRU_CLKSRC} --datapathmode=STREAMING --gbtmux=ttc --downstreamdata=pattern --cru-id=${CRUIDX} --dyn --trigger-window-size $WIN --pon --onu=${ONUID} --system-id=A --allow-rejection --tf-length ${TF_LENGTH}"

if [ x"${CRU_UL_ENABLED}" = "x1" ]; then
    RC_OPT="${RC_OPT} --user-logic"
fi

if [ x"${CRU_LINKS2_1}" != "x" ]; then
    #RC_OPT2="--id=${CRU_SN}:0 --links=${CRU_LINKS2_1}"
    RC_OPT2="--id=${CRU_SN}:0 --links=0"
else
    RC_OPT2="--id=${CRU_SN}:0 --no-gbt"
fi
#RC_OPT2="--id=${CRU_PCI_ADDR} --links=${CRU_LINKS2_1}"
echo "$RC_OPT $RC_OPT2"

if [ x"${CRU_LINKS2_2}" != "x" ]; then
    #RC_OPT2="--id=${CRU_SN}:1 --links=${CRU_LINKS2_2}"
    RC_OPT2="--id=${CRU_SN}:1 --links=0"
else
    RC_OPT2="--id=${CRU_SN}:1 --no-gbt"
fi
#RC_OPT2="--id=${CRU_PCI_ADDR2} --links=${CRU_LINKS2_2}"
echo "$RC_OPT $RC_OPT2"
