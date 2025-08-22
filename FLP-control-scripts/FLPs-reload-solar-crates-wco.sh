#! /bin/bash

CRATES=$*

FLPS=""
CRUS=""
SIDS=""


# build the entire possible list of SID from a SOLAR crate ID
for i in ${CRATES}; do
    for j in {0..5}; do 
        SID=$(($i*8+$j))
        SIDS="$SIDS $SID"
    done
    echo $SIDS
done


date 

#FLPS="148 149 150 151 152 153 154 155 156 157 158"
FLPS="$(cat flplist.txt)"

# note that the script  executedc on each flp will select the SID 
# that matches the SID that are connected on each  flp 
SCRIPT=./flp-reconfig-links-wco.sh

for FLP in $FLPS; do 
    ssh -Y mch@alio2-cr1-flp${FLP}.cern.ch $SCRIPT $SIDS &
    #echo  mch@alio2-cr1-flp${FLP}.cern.ch $SCRIPT $SIDS
    sleep 1
done
