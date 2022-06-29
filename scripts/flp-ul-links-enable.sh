#! /bin/bash

FLPMAP=flp.map

while read LINE; do

    CRU=$(echo "$LINE" | cut -f 1)

    if [ -n "$CRU" ]; then
	./cru-ul-links-enable.sh $CRU $1
    fi

done < "$FLPMAP"
