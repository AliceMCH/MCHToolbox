#! /bin/bash

CRU=$1
LINKID=$2
REGFILE=$3

SCRIPTDIR=$(readlink -f $(dirname $0))
source ${SCRIPTDIR}/env-${CRU}.sh


#PATT1=10101
#PATT2=00101
#PATT3=00000
#PATT4=00000
#PATT5=00000
#PATT6=00000
#PATT7=00000
#PATT8=00000
#PATT="$PATT8""$PATT7""$PATT6""$PATT5""$PATT4""$PATT3""$PATT2""$PATT1"

PATT="1111111111111111111111111111111111111111"
PATT0="0000000000000000000000000000000000000000"
#PATT=00000111

#echo "PATT: $PATT"

# clock disable
offset=0
for iter in $(seq 1 5); do
    P=$(echo -n "$PATT" | tail -c $(($iter*8)) | head -c 8)
    INPATTREG=$(echo "ibase=2; $P" | bc)
    for reg in 255 333 348; do
	R=$((reg+offset))
	echo "${LINKID} $R 0" >> "$REGFILE"
    done
    offset=$((offset+3))
done


# signal output disable
offset=0
for iter in $(seq 1 5); do
    for reg in 256 334 349; do
	R=$((reg+offset))
	echo "${LINKID} $R 0" >> "$REGFILE"
    done
    offset=$((offset+3))
done
