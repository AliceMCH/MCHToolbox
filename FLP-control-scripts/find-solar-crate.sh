#! /bin/bash

CRATE=$1

CRUMAPS=$(ls -1 ../Mapping/cru-*.map)
NCRUMAPS=$(echo "$CRUMAPS" | wc -l)

CRUS=""

function add_CRU
{
    local CRU=$1
    local i=1
    while [ true ]; do
	C=$(echo "$CRUS" | tr " " "\n" | sed -n ${i}p)
	if [ -z "$C" ]; then break; fi
	if [ x"$C" = x"$CRU" ]; then return; fi
	i=$((i+1))
    done
    if [ -z "$CRUS" ]; then
	CRUS=$CRU
    else
	CRUS="$CRUS $CRU"
    fi
}

for I in $(seq 1 $NCRUMAPS); do

    CRUMAP=$(echo "$CRUMAPS" | sed -n ${I}p)
    #echo "CRUMAP: $CRUMAP"

    while IFS= read -r line; do
	SID=$(echo "$line" | cut -f 1)
	if [ -z "$SID" ]; then continue; fi
	CID=$((SID/8))
	#echo "$SID $CID"

	if [ x"$CRATE" != x"$CID" ]; then continue; fi
	
	CRU=$(echo "$line" | cut -f 2)
	if [ -z "$CRU" ]; then continue; fi
	#echo "add_CRU $CRU"
	add_CRU $CRU
    done < "$CRUMAP"

done

echo "$CRUS"
