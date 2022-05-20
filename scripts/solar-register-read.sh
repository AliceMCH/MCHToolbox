#! /bin/bash

CRU=$1
SCRIPTDIR=$(readlink -f $(dirname $0))
source ${SCRIPTDIR}/env-${CRU}.sh

LINKID=$2

REG=$3


GBTCMD=../tools/command-line/build/gbt-config

echo "$GBTCMD  ${CRU_PCI_ADDR1} ${CRU_PCI_ADDR2} \"r\" ${LINKID} $REG"
$GBTCMD  ${CRU_PCI_ADDR1} ${CRU_PCI_ADDR2} "r" ${LINKID} $REG
