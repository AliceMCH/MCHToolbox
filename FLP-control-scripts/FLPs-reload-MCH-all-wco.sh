#! /bin/bash

#FLPS="148 149 150 151 152 153 154 155 156 157 158"
FLPS="$(cat flplist.txt)"

SCRIPT=./reload-detection-elements-wco.sh
DES=$(seq -s " " 100 103)
DES="$DES $(seq -s " " 200 203)"
DES="$DES $(seq -s " " 300 303)"
DES="$DES $(seq -s " " 400 403)"
DES="$DES $(seq -s " " 500 517)"
DES="$DES $(seq -s " " 600 617)"
DES="$DES $(seq -s " " 700 725)"
DES="$DES $(seq -s " " 800 825)"
DES="$DES $(seq -s " " 900 925)"
DES="$DES $(seq -s " " 1000 1025)"

echo "DEs: $DES"

for FLP in $FLPS; do 
	ssh -Y mch@alio2-cr1-flp${FLP}.cern.ch $SCRIPT $DES &
	sleep 1
done
