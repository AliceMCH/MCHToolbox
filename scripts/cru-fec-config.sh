#! /bin/bash

CRU=$1
shift
LINKS=$*

./cru-init-env.sh $CRU

./cru-ul-links-enable.sh $CRU 0
./cru-ul-reset.sh $CRU

while [ true ]; do
    if [ -z "$LINKS" ]; then
	./cru-solar-config.sh $CRU && sleep 5 && ./cru-send-hard-reset.sh $CRU && ./cru-config-trigger.sh $CRU && ./cru-ds-config.sh $CRU
    else
	./cru-solar-config.sh $CRU $LINKS && sleep 5 $CRU && ./cru-ds-config.sh $CRU $LINKS
    fi

    NFAILED=$(cat sampa_load_${CRU}.log | grep "FAILED" | wc -l)
    NTIMEOUTS=$(cat sampa_load_${CRU}.log | grep "timeout" | wc -l)
    if [ $NFAILED -lt 11 -a $NTIMEOUTS -lt 1 ]; then
	break
    fi

    echo ""; echo "";
    RED='\033[0;31m'
    NC='\033[0m' # No Color
    echo -n -e "${RED}Too many DS boards with errors, retry? [Y/n]${NC}"
    read dummy
    if [ x"$dummy" = "xn" ]; then
	break
    fi
done

sleep 1
./cru-ul-links-enable.sh $CRU 1
