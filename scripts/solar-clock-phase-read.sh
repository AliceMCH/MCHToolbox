#! /bin/bash

CRU=$1
SCRIPTDIR=$(readlink -f $(dirname $0))
source ${SCRIPTDIR}/env-${CRU}.sh

LINKID=$2


GBTCMD=../tools/command-line/build/gbt-config

#$GBTCMD ${CRU_PCI_ADDR} "r" ${LINKID} 390
for reg in $(seq 390 396); do
    #echo "$GBTCMD ${CRU_PCI_ADDR1} ${CRU_PCI_ADDR2} r ${LINKID} $reg"
    $GBTCMD ${CRU_PCI_ADDR1} ${CRU_PCI_ADDR2} "r" ${LINKID} $reg
done
#echo ""; echo "";
PHASES=""
for reg in $(seq 399 418); do
    #echo "$GBTCMD ${CRU_PCI_ADDR1} ${CRU_PCI_ADDR2} r ${LINKID} $reg"
    PHASE=$($GBTCMD ${CRU_PCI_ADDR1} ${CRU_PCI_ADDR2} "r" ${LINKID} $reg | tail -n 1 | cut -d" " -f 4)
    PHASE1=$(echo "$PHASE" | cut -c 4)
    PHASE2=$(echo "$PHASE" | cut -c 3)

    if [ -z "$PHASE1" ]; then PHASE1="0"; fi
    if [ -z "$PHASE2" ]; then PHASE2="0"; fi

    if [ -n "$PHASES" ]; then
	PHASES="$PHASES $PHASE1 $PHASE2"
    else
	PHASES="$PHASE1 $PHASE2"
    fi
done
echo "$PHASES"
