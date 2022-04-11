#! /bin/bash

CRATES=$*

FLPS=""
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
    
    FLP=$(($CRU/3+148))
    if [ -z "$FLPS" ]; then
	FLPS=$FLP
    else
	FLPS="$FLPS $FLP"
    fi
}

c=1
while [ true ]; do
    CRATE=$(echo "$CRATES" | tr " " "\n" | sed -n ${c}p)
    if [ -z "$CRATE" ]; then break; fi
    c=$((c+1))

    _CRUS=$(./find-solar-crate.sh $CRATE)
    
    #MAP=$(./find-solar-crate.sh $CRATE)    
    #FLPS=$(echo "$MAP" | sed -n 1p)
    #CRUS=$(echo "$MAP" | sed -n 2p)
    
    echo "CRUs for crate $CRATE: ${_CRUS}"
    i=1
    while [ true ]; do
	CRU=$(echo "${_CRUS}" | tr " " "\n" | sed -n ${i}p)
	if [ -z "$CRU" ]; then break; fi
	i=$((i+1))
	
	echo "add_CRU $CRU"
	add_CRU $CRU
    done
done
    
i=1
while [ true ]; do
    FLP=$(echo "$FLPS" | tr " " "\n" | sed -n ${i}p)
    if [ -z "$FLP" ]; then break; fi
    CRU=$(echo "$CRUS" | tr " " "\n" | sed -n ${i}p)
    if [ -z "$CRU" ]; then break; fi    
    i=$((i+1))
    
    echo "Reloading CRU $CRU on FLP $FLP"
    ssh -Y mch@alio2-cr1-flp${FLP}.cern.ch "xterm -hold -bg black -fg white -geometry 100x45 -T \"FLP$FLP $CRU\" -e ./cru-fec-config.sh $CRU" &
	
done

wait
