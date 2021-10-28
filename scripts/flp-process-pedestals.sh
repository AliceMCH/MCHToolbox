#! /bin/bash

#SCRIPT_DIR=$(dirname $0)
#source ${SCRIPT_DIR}/env.sh
SCRIPT_DIR=$(pwd)

rm -f ds_ch_exclude.txt
FLPMAP=flp.map

while read LINE; do

    CRU=$(echo "$LINE" | cut -f 1)
    echo "CRU: $CRU"

    if [ -n "$CRU" ]; then
	./cru-process-pedestals.sh $CRU "$1" >& log-$CRU.txt
	cat /tmp/ds_ch_exclude.txt >> ds_ch_exclude.txt
    fi
done < $FLPMAP
