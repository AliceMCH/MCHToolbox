#! /bin/bash

FLPMAP=flp.map

HOLD="-hold"
if [ x"$1" = "x-e" ]; then
    HOLD=""
fi

while read LINE; do

    CRU=$(echo "$LINE" | cut -f 1)

#    ./cru-solar-clock-phase-save.sh $CRU
    
    if [ -n "$CRU" ]; then
	xterm -bg black -fg white -geometry 100x45 -T "cru-solar-clock-phase-save.sh $CRU" $HOLD -e ./cru-solar-clock-phase-save.sh $CRU &
    fi

done < "$FLPMAP"

# wait for all CRUs to complete
wait
