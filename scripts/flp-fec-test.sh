#! /bin/bash

FLPMAP=flp.map

while read LINE; do
    
    CRU=$(echo "$LINE" | cut -f 1)
    
    if [ -n "$CRU" ]; then
	xterm -bg black -fg white -geometry 100x45 -T "cru-fec-test.sh $CRU" -e ./cru-fec-test.sh $CRU &
    fi
    
done < "$FLPMAP"

# wait for all CRUs to complete
wait
