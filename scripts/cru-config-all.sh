#! /bin/bash

CRU=$1

./cru-init-env.sh $CRU
./cru-config.sh $CRU
sleep 1
while [ true ]; do
    ./cru-solar-config.sh $CRU && sleep 5 && ./cru-solar-config.sh $CRU && sleep 5 && ./cru-ds-config.sh $CRU

    NFAILED=$(cat sampa_load_${CRU}.log | grep "FAILED" | wc -l)
    if [ $NFAILED -lt 10 ]; then
	break
    fi

    echo ""; echo "";
    RED='\033[0;31m'
    NC='\033[0m' # No Color
    echo -n -e "${RED}Too many DS boards with errors, please check and press enter when ready...${NC}"
    read dummy
done

./cru-ul-reset.sh $CRU
