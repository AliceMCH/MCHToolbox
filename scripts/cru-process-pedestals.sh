#! /bin/bash

SCRIPTDIR=$(readlink -f $(dirname $0))

CRU=$1
source ${SCRIPTDIR}/env-${CRU}.sh

FILE=$2

if [ -z "$FILE" ]; then
    FILE=$HOME/Data/data-${CRU}.raw
fi

if [ x"${_CRU_UL_ENABLED}" = "x1" ]; then
    echo "${SCRIPTDIR}/../tools/command-line/build/cru-decode-ul -c $CRU -d 0 -n $FILE"
    ${SCRIPTDIR}/../tools/command-line/build/cru-decode-ul -c $CRU -d 0 -n "$FILE"
else
#gdb -ex run 
    echo "${SCRIPTDIR}/../tools/command-line/build/cru-decode -c $1 -d 0 -n $FILE"
    ${SCRIPTDIR}/../tools/command-line/build/cru-decode -c $CRU -d 0 -n "$FILE" #| tee $HOME/decode.log
fi
