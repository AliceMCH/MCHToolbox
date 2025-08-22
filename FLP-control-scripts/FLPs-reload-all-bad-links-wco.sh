#! /bin/bash

# wco  stands for With CRU ON
# i.e. cru link  is not disabled during reconfiguration  

./UpdateBadSolarBoardsFromCCDB.sh

SLIST=$(cat bad_solar_ids.txt)

if [ -z "${SLIST}" ]; then
    echo "List of bad SOLAR links is empty, nothing to do..."
    exit 1
fi

./UpdateBadChannelsFromCCDB.sh

echo ""; echo "=========="; echo ""; 
echo "The following links will be reloaded: $SLIST"
echo ""; echo -n "Do you want to proceed? [y/N] "
read answer
echo ""; echo "=========="; echo ""; 
if [ x"$answer" != "xy" ]; then
    exit 1
fi

echo "./FLPs-reload-links-wco.sh ${SLIST}"
./FLPs-reload-links-wco.sh ${SLIST}
