#! /bin/bash

CRATE=$1

MAP=$(./find-solar-crate.sh $CRATE)

FLPS=$(echo "$MAP" | sed -n 1p)
CRUS=$(echo "$MAP" | sed -n 2p)

echo "FLPs: $FLPS"
echo "CRUs: $CRUS"

i=1
while [ true ]; do
    FLP=$(echo "$FLPS" | tr " " "\n" | sed -n ${i}p)
    CRU=$(echo "$CRUS" | tr " " "\n" | sed -n ${i}p)

    if [ -z "$FLP" ]; then break; fi
    if [ -z "$CRU" ]; then break; fi

    echo "Reloading CRU $CRU on FLP $FLP"
    ssh -Y mch@alio2-cr1-flp${FLP}.cern.ch "./cru-fec-config.sh $CRU"
    
    i=$((i+1))
done
