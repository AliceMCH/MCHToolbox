#! /bin/bash

CRU=$1
LINK=$2

CRUMAP=cru.map

# get link IDs associated to this DE
while read LINE; do

	
    FEEID=$(echo "$LINE" | cut -f 2)
    if [ -z "$FEEID" ]; then continue; fi
    
    if [ "$CRU" = "$FEEID" ]; then
	
	L=$(echo "$LINE" | cut -f 3)
	if [ -z "$L" ]; then continue; fi

	if [ "$LINK" = "$L" ]; then
	
	    LINKID=$(echo "$LINE" | cut -f 1)
	    if [ -z "$LINKID" ]; then continue; fi

	    CRATEID=$(echo "scale=0; $LINKID / 8" | bc)
	    SOLARID=$(echo "scale=0; $LINKID % 8 + 1" | bc)

	    echo "SOLARLINK ID: $LINKID    CRATE ID: $CRATEID   SOLAR ID: $SOLARID"
	    cat fec.map | grep -e "^$LINKID"
	fi
    fi
    
done < "$CRUMAP"


