#! /bin/bash

SCRIPTDIR=$(readlink -f $(dirname $0))

CRU=$1
source ${SCRIPTDIR}/env-${CRU}.sh

FILE=$2
if [ -z "$FILE" ]; then
    FILE=$HOME/Data/data-${CRU}.raw
fi

#(cd ${SCRIPTDIR}/../tools/Decode && make)
#source setup.sh


#source ./de_get_ds_enabled.sh $1
./cru-get-ds-enabled.sh $CRU

BOARDS=""
NL=$(cat board-enable-${CRU}.txt | wc -l)
I=1
while [ $I -le $NL ]; do
    LINE=$(sed -n ${I}p board-enable-${CRU}.txt)
    LINK_ID=$(echo "$LINE" | cut -d" " -f 2)
    DS_ADDR=$(echo "$LINE" | cut -d" " -f 3)
    ENABLED=$(echo "$LINE" | cut -d" " -f 4)
    if [ x"$ENABLED" = "x1" ]; then
	DS_ID=$(echo "${LINK_ID} * 40 + ${DS_ADDR}" | bc)
	BOARDS="$BOARDS -b ${DS_ID}"
    fi
    I=$((I+1))
done


if [ x"${CRU_UL_ENABLED}" = "x1" ]; then
    echo "${SCRIPTDIR}/../tools/command-line/build/cru-decode-ul -c $CRU -d 0 -n $BOARDS $FILE"
    ${SCRIPTDIR}/../tools/command-line/build/cru-decode-ul -c $CRU -d 0 -n $BOARDS "$FILE"
else
#gdb -ex run 
    echo "${SCRIPTDIR}/../tools/command-line/build/cru-decode -c $1 -d 0 -n $BOARDS $FILE"
    ${SCRIPTDIR}/../tools/command-line/build/cru-decode -c $CRU -d 0 -n $BOARDS "$FILE" #| tee $HOME/decode.log
fi

#read dummy
