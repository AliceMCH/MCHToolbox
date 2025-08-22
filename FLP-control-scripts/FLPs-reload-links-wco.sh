#! /bin/bash

# wco  stands for With CRU ON
# i.e. cru link  is not disabled during reconfiguration  

date 

#FLPS="148 149 150 151 152 153 154 155 156 157 158"
FLPS="$(cat flplist.txt)"

SCRIPT=./flp-reconfig-links-wco.sh
SIDS=$*

for FLP in $FLPS; do 
	ssh -Y mch@alio2-cr1-flp${FLP}.cern.ch $SCRIPT $SIDS &
	sleep 1
done
