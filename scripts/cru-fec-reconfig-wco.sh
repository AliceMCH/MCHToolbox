#! /bin/bash

CRU=$1
shift
LINKS=$*
echo "Will only reconfigure cru ${CRU} and links ${LINKS} keeping CRU ON"
echo -e "\n"

./cru-ul-links-enable.sh $CRU 0; sleep 1

./cru-solar-config.sh $CRU $LINKS && sleep 5  && ./cru-ds-config.sh $CRU $LINKS

NFAILED=$(cat sampa_load_${CRU}.log | grep "FAILED" | wc -l)
NTIMEOUTS=$(cat sampa_load_${CRU}.log | grep "timeout" | wc -l)

echo ""
echo ""
RED='\033[0;31m'
NC='\033[0m' # No Color
if [ $NFAILED -lt 11 -a $NTIMEOUTS -lt 1 ]; then
    echo -n -e "Seen $NFAILED  DS configuration failures and $NTIMEOUTS configuration timeout "

else
    echo -n -e "${RED} Seen $NFAILED  DS configuration failures and $NTIMEOUTS  ${NC}"
    echo -n -e "${RED} Probably too many DS boards with errors ${NC}"
fi
echo ""
echo ""

sleep 1; ./cru-ul-links-enable.sh $CRU 1
