#! /bin/bash

CRU=$1
DWADDR1=""
DWADDR2=""
CRU_LINKS1=""
CRU_LINKS2=""
CRU_LINKIDS1=""
CRU_LINKIDS2=""
CRU_SN=""
FEEID=""

CRUMAP=cru.map

# get link IDs associated to this DE
rm -f /tmp/__linkids__
while read LINE; do

	
    FEEID=$(echo "$LINE" | cut -f 2)
    if [ -z "$FEEID" ]; then continue; fi
    
    if [ "$CRU" = "$FEEID" ]; then
	
	CRU_SN=$(echo "$LINE" | cut -f 4)
	if [ -z "${CRU_SN}" ]; then continue; fi
	
	CRU2=$(echo "$LINE" | cut -f 6)
	if [ -z "$CRU2" ]; then continue; fi
    
	CRU3=$(echo "$LINE" | cut -f 7)
	if [ -z "$CRU3" ]; then continue; fi
	
	DW=$(echo "$LINE" | cut -f 5)
	if [ -z "$DW" ]; then continue; fi
	
	LINK=$(echo "$LINE" | cut -f 3)
	if [ -z "$LINK" ]; then continue; fi
	
	LINKID=$(echo "$LINE" | cut -f 1)
	if [ -z "$LINKID" ]; then continue; fi

	CRATEID=$(echo "scale=0; $LINKID / 8" | bc)
	
	if [ x"$DW" = "x0" ]; then 
	    DWADDR1="$CRU3"; 
	    if [ -z "${CRU_LINKS1}" ]; then
		CRU_LINKS1="${LINK}"
		CRU_LINKIDS1="${LINKID}"
		CRU_CRATEIDS1="${CRATEID}"
	    else
		CRU_LINKS1="${CRU_LINKS1},${LINK}"
		CRU_LINKIDS1="${CRU_LINKIDS1},${LINKID}"
		CRU_CRATEIDS1="${CRU_CRATEIDS1},${CRATEID}"
	    fi
	fi
	if [ x"$DW" = "x1" ]; then 
	    DWADDR2="$CRU3";
	    LINK2=$((LINK-12))
	    if [ -z "${CRU_LINKS2}" ]; then
		CRU_LINKS2="${LINK}"
		CRU_LINKS2b="${LINK2}"
		CRU_LINKIDS2="${LINKID}"
		CRU_CRATEIDS2="${CRATEID}"
	    else
		CRU_LINKS2="${CRU_LINKS2},${LINK}"
		CRU_LINKS2b="${CRU_LINKS2b},${LINK2}"
		CRU_LINKIDS2="${CRU_LINKIDS2},${LINKID}"
		CRU_CRATEIDS2="${CRU_CRATEIDS2},${CRATEID}"
	    fi
	fi
    fi
    
done < "$CRUMAP"

if [ -n "${DWADDR1}" ]; then
    echo "${DWADDR1} ${CRU} ${CRU_SN} 0 ${CRU_LINKS1} ${CRU_LINKS1} ${CRU_LINKIDS1} ${CRU_CRATEIDS1}"
fi
if [ -n "${DWADDR2}" ]; then
    echo "${DWADDR2} ${CRU} ${CRU_SN} 1 ${CRU_LINKS2} ${CRU_LINKS2b} ${CRU_LINKIDS2} ${CRU_CRATEIDS2}"
fi

