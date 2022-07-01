#! /bin/bash

FLPMAP=flp.map

HOLD="-hold"
if [ x"$1" = "x-e" ]; then
    HOLD=""
fi

while read LINE; do

    CRU=$(echo "$LINE" | cut -f 1)

    if [ -n "$CRU" ]; then
	xterm -bg black -fg white -geometry 100x45 -T "cru-check-link-status.sh $CRU" $HOLD -e ./cru-check-link-status.sh $CRU &
    fi

done < "$FLPMAP"
