#! /bin/bash

CRATES=$*

c=1
while [ true ]; do
    CRATE=$(echo "$CRATES" | tr " " "\n" | sed -n ${c}p)
    if [ -z "$CRATE" ]; then break; fi
    c=$((c+1))

    MAP=$(./find-solar-crate.sh $CRATE)
    
    FLPS=$(echo "$MAP" | sed -n 1p)
    CRUS=$(echo "$MAP" | sed -n 2p)
    
    echo "FLPs: $FLPS"
    echo "CRUs: $CRUS"
    
    i=1
    while [ true ]; do
	FLP=$(echo "$FLPS" | tr " " "\n" | sed -n ${i}p)
	if [ -z "$FLP" ]; then break; fi
	CRU=$(echo "$CRUS" | tr " " "\n" | sed -n ${i}p)
	if [ -z "$CRU" ]; then break; fi    
	i=$((i+1))
	
	echo "Reloading CRU $CRU on FLP $FLP"
	ssh -Y mch@alio2-cr1-flp${FLP}.cern.ch "xterm -hold -bg black -fg white -geometry 100x45 -T \"FLP$FLP $CRU\" -e ./cru-fec-config.sh $CRU" &
	
    done
done
