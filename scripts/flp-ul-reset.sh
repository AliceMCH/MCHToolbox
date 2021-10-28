#! /bin/bash

FLPMAP=flp.map

while read LINE; do

    CRU=$(echo "$LINE" | cut -f 1)

    if [ -n "$CRU" ]; then
	./cru-ul-reset.sh $CRU
    fi

done < "$FLPMAP"
