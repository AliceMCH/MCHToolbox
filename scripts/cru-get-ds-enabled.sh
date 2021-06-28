#! /bin/bash

CRU=$1

SCRIPTDIR=$(readlink -f $(dirname $0))
source ${SCRIPTDIR}/env-${CRU}.sh


FECMAP=fec.map
CRUMAP=cru.map
if [ -e board-enable-${CRU}.txt -a board-enable-${CRU}.txt -nt "$FECMAP" -a board-enable-${CRU}.txt -nt "$CRUMAP" ]; then
    exit
fi

# get link IDs associated to this DE
rm -f board-enable-${CRU}.txt

NLINKS=$(echo "${CRU_LINKIDS}" | tr "," "\n" | wc -l)
for LI in $(seq 1 $NLINKS); do

    CRU_LINK=$(echo "${CRU_LINKS1}" | tr "," "\n" | sed -n ${LI}p)
    LID=$(echo "${CRU_LINKIDS}" | tr "," "\n" | sed -n ${LI}p)
    #echo "LID: $LID"
    if [ x"$LID" = "x" ]; then continue; fi

    while read LINE; do
	
	LINKID=$(echo "$LINE" | cut -f 1)

	if [ x"$LID" != x"$LINKID" ]; then continue; fi

	#echo "$LINE"
	GROUPID=$(echo "$LINE" | cut -f 2)
    	for I in $(seq 4 8); do
	    DSID=$(echo "$LINE" | cut -f $I)
	    #echo "LINKID=$LINKID  GROUPID=$GROUPID  DSID$((I-4))=$DSID"
	    if [ -z "$DSID" ]; then continue; fi
	    if [ x"$DSID" = "x0" ]; then continue; fi
	    if [ $DSID -lt 0 ]; then continue; fi
	    DSID2=$(echo "scale=0; $GROUPID * 5 + $I - 4" | bc -l)
	    #echo "  DSID2=$DSID2"
	    echo "${FEEID} ${CRU_LINK} $DSID2 1" >> board-enable-${CRU}.txt
	done		

    done < "$FECMAP"

done

