#! /bin/bash

SID=$1

CRUMAP=cru.map

CRU=""

while IFS= read -r line; do
    SID2=$(echo "$line" | cut -f 1)
    if [ -z "$SID2" ]; then continue; fi
    
    if [ x"$SID2" != x"$SID" ]; then continue; fi
    
    CRU=$(echo "$line" | cut -f 2)
    if [ -z "$CRU" ]; then continue; fi
    LINK=$(echo "$line" | cut -f 3)
done < "$CRUMAP"

echo "$CRU $LINK"
