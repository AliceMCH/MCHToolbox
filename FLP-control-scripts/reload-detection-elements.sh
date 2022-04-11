#! /bin/bash

DES=$*

FECMAPS=$(ls -1 ../Mapping/fec-*.map)
NFECMAPS=$(echo "$FECMAPS" | wc -l)

CRATES=""

function add_crate
{
    local CRATE=$1
    local i=1
    while [ true ]; do
	C=$(echo "$CRATES" | tr " " "\n" | sed -n ${i}p)
	if [ -z "$C" ]; then break; fi
	if [ x"$C" = x"$CRATE" ]; then return; fi
	i=$((i+1))
    done
    if [ -z "$CRATES" ]; then
	CRATES=$CRATE
    else
	CRATES="$CRATES $CRATE"
    fi
}

for I in $(seq 1 $NFECMAPS); do

    FECMAP=$(echo "$FECMAPS" | sed -n ${I}p)
    #echo "FECMAP: $FECMAP"

    while IFS= read -r line; do
	SID=$(echo "$line" | cut -f 1)
	if [ -z "$SID" ]; then continue; fi
	CID=$((SID/8))
	#echo "$SID $CID"

	D=$(echo "$line" | cut -f 3)

	
	d=1
	while [ true ]; do
	    DE=$(echo "$DES" | tr " " "\n" | sed -n ${d}p)
	    if [ -z "$DE" ]; then break; fi
	    d=$((d+1))

	    if [ x"$DE" != x"$D" ]; then continue; fi

	    #echo "add_crate $CID"
	    add_crate $CID
	done
    done < "$FECMAP"
    echo -n "* "

done

echo ""

if [ ! -z "$CRATES" ]; then
    echo "./reload-solar-crates.sh $CRATES"
    ./reload-solar-crates.sh $CRATES
fi
