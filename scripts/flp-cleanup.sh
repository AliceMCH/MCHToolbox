#! /bin/bash

FLPMAP=flp.map

FORCE=$1

while read LINE; do

    CRU=$(echo "$LINE" | cut -f 1)

    ./cru-cleanup.sh $CRU

done < "$FLPMAP"
