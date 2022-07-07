#! /bin/bash

DES=$*

FECMAP=fec.map
NFECMAPS=$(echo "$FECMAPS" | wc -l)

SIDS=""

function add_solar
{
    local SID=$1
    local i=1
    while [ true ]; do
	S=$(echo "$SIDS" | tr " " "\n" | sed -n ${i}p)
	if [ -z "$S" ]; then break; fi
	if [ x"$S" = x"$SID" ]; then return; fi
	i=$((i+1))
    done
    if [ -z "$SIDS" ]; then
	SIDS=$SID
    else
	SIDS="$SIDS $SID"
    fi
}

while IFS= read -r line; do
    SID=$(echo "$line" | cut -f 1)
    if [ -z "$SID" ]; then continue; fi
    
    D=$(echo "$line" | cut -f 3)
    
    
    d=1
    while [ true ]; do
	DE=$(echo "$DES" | tr " " "\n" | sed -n ${d}p)
	if [ -z "$DE" ]; then break; fi
	d=$((d+1))
	
	if [ x"$DE" != x"$D" ]; then continue; fi
	
	#echo "add_crate $CID"
	add_solar $SID
    done
done < "$FECMAP"

echo "./flp-reconfig-links.sh $SIDS"
./flp-reconfig-links.sh $SIDS
