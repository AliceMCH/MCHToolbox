#! /bin/bash

#FLPS="148 149 150 151 152 153 154 155 156 157 158"
#FLPS="154 156"
FLPS="$(cat flplist.txt)"

#TAG=ped-20211116-with-hv-filter
#TAG=cont-20211116
#TAG=cont-600V-20220316
#TAG=ped-std_config-20220318
TAG=ped-20220323

for FLP in $FLPS; do 
	ssh -Y mch@alio2-cr1-flp${FLP}.cern.ch "cp -a /home/mch/Data/data-flp.raw /home/mch/Data/data-${FLP}-${TAG}.raw && ls -lh /home/mch/Data/data-${FLP}-${TAG}.raw"
	sleep 1
done
