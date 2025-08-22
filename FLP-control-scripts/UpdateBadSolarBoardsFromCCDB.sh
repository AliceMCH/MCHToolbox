#! /bin/bash

#echo $HOSTNAME
CCDBURL=
if [[ "${HOSTNAME}" =~ .*flp148.* ]]; then
    CCDBURL="http://o2-ccdb.internal"
else
    CCDBURL="https://alice-ccdb.cern.ch"
fi
root -l -b -q "UpdateBadSolarBoardsFromCCDB.C(\"${CCDBURL}\")" 
