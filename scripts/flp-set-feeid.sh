#! /bin/bash

FLPMAP=flp.map

FORCE=$1

while read LINE; do

    CRU=$(echo "$LINE" | cut -f 1)

    if [ -n "$CRU" ]; then
	xterm -bg black -fg white -geometry 100x45 -T "cru-set-feeid.sh $CRU" -e ./cru-set-feeid.sh $CRU &
    fi

done < "$FLPMAP"

# wait for all CRUs to complete
wait
