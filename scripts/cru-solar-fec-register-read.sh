#! /bin/bash

CRU=$1
SCRIPTDIR=$(readlink -f $(dirname $0))
source ${SCRIPTDIR}/env-${CRU}.sh

GBTCMD=../tools/command-line/build/gbt-config

#echo "${CRU_LINKS1}"
echo "CRU $CRU"

FECL1=""
NLINKS=$(echo "${CRU_LINKS1}" | tr "," "\n" | wc -l)
for L in $(seq 1 $NLINKS); do
    LINKID=$(echo "${CRU_LINKS1}" | tr "," "\n" | sed -n ${L}p)
    #echo "LINKID: $LINKID"
    if [ -z "$LINKID" ]; then continue; fi
    #$GBTCMD "${CRU_PCI_ADDR1}" "${CRU_PCI_ADDR1}" "r" ${LINKID} 435
    FEC=$($GBTCMD "${CRU_PCI_ADDR1}" "${CRU_PCI_ADDR1}" "r" ${LINKID} 435 | cut -d' ' -f 3)
    LOCK=$($GBTCMD "${CRU_PCI_ADDR1}" "${CRU_PCI_ADDR1}" "r" ${LINKID} 434 | cut -d' ' -f 3)
    #echo "  ${LINKID}"'\t'" FEC=$FEC"'\t'" LOCK=$LOCK"
    printf "  CRU %2d link %2d\t FEC: %4d\t LOCK: %4d\n" ${CRU} ${LINKID} $FEC $LOCK
    #FECL1="${FECL1}\nL${LINKID}=${FEC}"
done

#echo "$FECL1"
