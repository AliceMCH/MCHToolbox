#! /bin/bash

CRU=$1

./cru-init-env.sh $CRU
./cru-ul-links-enable.sh $CRU 0

while [ true ]; do
    ./cru-solar-config.sh $CRU && sleep 5 && ./cru-ds-config.sh $CRU

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

./cru-ul-reset.sh $CRU
sleep 1
./cru-ul-links-enable.sh $CRU 1
