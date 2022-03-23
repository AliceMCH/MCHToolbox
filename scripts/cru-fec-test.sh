#! /bin/bash

CRU=$1
SCRIPTDIR=$(readlink -f $(dirname $0))
source ${SCRIPTDIR}/env-${CRU}.sh

while [ true ]; do
    bash ./cru-check-link-status.sh $CRU
    if [ $? -eq 0 ]; then
        break
    fi
    echo ""; echo "";
    RED='\033[0;31m'
    NC='\033[0m' # No Color
    echo -n -e "${RED}Some SOLAR boards are DOWN, please try to power-cycle the corresponding crates and press enter...${NC}"
    read dummy
done

GBTCMD=../tools/command-line/build/gbt-config

NLINKS=$(echo "${CRU_LINKS1}" | tr "," "\n" | wc -l)
for L in $(seq 1 $NLINKS); do
    LINKID=$(echo "${CRU_LINKS1}" | tr "," "\n" | sed -n ${L}p)
    #echo "LINKID: $LINKID"
    if [ -z "$LINKID" ]; then continue; fi
    FEC=$($GBTCMD "${CRU_PCI_ADDR1}" "${CRU_PCI_ADDR1}" "r" ${LINKID} 435 | cut -d' ' -f 4)
    #echo "${LINKID} $FEC"
    FECL1="${FECL1} ${FEC}"
done

FECL1=""
FECL2=""

for L in $(seq 1 $NLINKS); do
    LINKID=$(echo "${CRU_LINKS1}" | tr "," "\n" | sed -n ${L}p)
    #echo "LINKID: $LINKID"
    if [ -z "$LINKID" ]; then continue; fi
    FEC=$($GBTCMD "${CRU_PCI_ADDR1}" "${CRU_PCI_ADDR1}" "r" ${LINKID} 435 | cut -d' ' -f 4)
    #echo "${LINKID} $FEC"
    FECL1="${FECL1} ${FEC}"
done

echo "${FECL1}"

TIMEOUT=600
while [ $TIMEOUT -gt 0 ]; do
    sleep 10
    TIMEOUT=$((TIMEOUT-10))
    echo -n "...${TIMEOUT} "
done
echo ""

for L in $(seq 1 $NLINKS); do
    LINKID=$(echo "${CRU_LINKS1}" | tr "," "\n" | sed -n ${L}p)
    #echo "LINKID: $LINKID"
    if [ -z "$LINKID" ]; then continue; fi
    FEC=$($GBTCMD "${CRU_PCI_ADDR1}" "${CRU_PCI_ADDR1}" "r" ${LINKID} 435 | cut -d' ' -f 4)
    #echo "${LINKID} $FEC"
    FECL2="${FECL2} ${FEC}"
done

echo "${FECL2}"

FEC_OK=1
for L in $(seq 1 $NLINKS); do

    LINKID=$(echo "${CRU_LINKS1}" | tr "," "\n" | sed -n ${L}p)
    DP=$((LINKID/12))
    LINKID2=${LINKID}
    if [ $DP -gt 0 ]; then
	LINKID2=$((LINKID-12))
    fi
    
    FEC1=$(echo "${FECL1}" | cut -d' ' -f $((L+2)))
    FEC2=$(echo "${FECL2}" | cut -d' ' -f $((L+2)))
    FEC_OK=1
    if [ "x${FEC2}" != "x${FEC1}" ]; then
	RED='\033[0;31m'
	NC='\033[0m' # No Color
	echo -e "${RED}Link $LINKID ($DP/$LINKID2) FEC counter mismatch${NC}"
        echo "$FEC1 $FEC2"
 	FEC_OK=0
	#echo "link $LINKID FEC_OK=${FEC_OK}"
    fi
    #echo "link $LINKID FEC_OK=${FEC_OK}"
    #echo ""
done

if [ x"${FEC_OK}" != "x1" ]; then
    echo -n "Some links are unstable. Press enter key to close terminal..."
    read dummy
fi
