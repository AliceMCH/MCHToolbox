#! /bin/bash

SIDS=$*
NIDS=$(echo "$SIDS" | tr " " "\n" | wc -l)
CRULINKS=()

for I in $(seq 0 31); do
    CRULINKS+=( "" )
done

for I in $(seq 1 $NIDS); do

    SID=$(echo "$SIDS" | cut -d" " -f ${I})
    #echo "SID: $SID"
    if [ x"$SID" = "x" ]; then continue; fi

    CRULINK=$(./find-solar-id.sh $SID)
    if [ -z "$CRULINK" ]; then continue; fi

    CRU=$(echo "$CRULINK" | cut -d" " -f 1)
    if [ -z "$CRU" ]; then continue; fi

    LINK=$(echo "$CRULINK" | cut -d" " -f 2)
    if [ -z "$LINK" ]; then continue; fi

    # append link to this CRU
    CRULINKS[$CRU]="${CRULINKS[$CRU]} $LINK"

done

for CRU in $(seq 0 31); do
    LINKS="${CRULINKS[$CRU]}"
    if [ -z "${LINKS}" ]; then continue; fi
    echo "xterm -bg black -fg white -geometry 100x45 -T \"cru-fec-reconfig-wco.sh $CRU $LINKS\" -hold -e ./cru-fec-reconfig-wco.sh $CRU $LINKS"
    xterm -bg black -fg white -geometry 100x45 -T "cru-fec-reconfig-wco.sh $CRU $LINKS" -hold -e ./cru-fec-reconfig-wco.sh $CRU $LINKS &
done

wait
