#! /bin/bash

FLPMAP=flp.map
FILE=$1

if [ -z "$FILE" ]; then
    FILE=$HOME/Data/data-flp.raw
fi

while read LINE; do
    
    CRU=$(echo "$LINE" | cut -f 1)
    
    if [ -n "$CRU" ]; then
	#./cru_get_links.sh $CRU
	./cru-check-data.sh $CRU "$FILE" >& log-${CRU}.txt
	echo "log-${CRU}.txt created"
    fi
    
done < "$FLPMAP"
