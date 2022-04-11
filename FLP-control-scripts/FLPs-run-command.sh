#! /bin/bash

#FLPS="148 149 150 151 152 153 154 155 156 157 158"
#FLPS="158"
FLPS="$(cat flplist.txt)"

CMD="$*"
echo "CMD: \"$CMD\""

for FLP in $FLPS; do 
        echo ""; echo "============ $FLP ============"; echo "";
	ssh -Y mch@alio2-cr1-flp${FLP}.cern.ch "$CMD" 
	#sleep 1
done
