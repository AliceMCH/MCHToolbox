#! /bin/bash

#FLPS="148 149 150 151 152 153 154 155 156 157 158"
FLPS="$(cat flplist.txt)"

SCRIPT=./reload-physics.sh

for FLP in $FLPS; do 
	ssh -Y mch@alio2-cr1-flp${FLP}.cern.ch $SCRIPT &
	sleep 1
done
