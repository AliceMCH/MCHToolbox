#! /bin/bash

OUTDIRNAME="Pedestals"

# get the latest Pedestals file from QCDB
if [ -e Pedestals.txt ]; then
    rm -f PreviousPedestals.txt
    mv Pedestals.txt PreviousPedestals.txt
fi
echo $HOSTNAME
if [[ "${HOSTNAME}" =~ .*flp148.* ]]; then
    root -l -b -q "UpdatePedestalsFromQCDB.C(\"ali-qcdb.cern.ch:8083\")"
else
    root -l -b -q "UpdatePedestalsFromQCDB.C(\"ali-qcdb-gpn.cern.ch:8083\")"
fi

# the Pedestals.txt file contains one line for each readout channel, formatted like this:
# CRUID [0..31]  LINKID [0..23]  DSID [0..39]  CHIP [0..1]  CHANNEL [0..32]  PEDESTAL  NOISE  STAT

if [ -e Pedestals.txt ]; then

    # check if pedestals file is the same as previous, if yes  do nothing
    diff Pedestals.txt PreviousPedestals.txt &>/dev/null
    res="$?"
    if [ "$res" -eq 0 ]; then
        echo -n -e "\n"
        echo -n -e "The updated Pedestals.txt file is identical to the previous one \n"
        echo -n -e "----> doing nothing and exiting the script \n"
        echo -n -e "\n"

        exit 1
    fi

    rm -rf ${OUTDIRNAME}
    mkdir -p ${OUTDIRNAME}

    for CRU in $(seq 0 31); do
	echo -n "CRU $CRU / "
	for LINK in $(seq 0 23); do    
	    cat Pedestals.txt | grep "^${CRU} ${LINK}" > Pedestals_${CRU}_${LINK}.txt
	    for DS in $(seq 0 39); do
		cat Pedestals_${CRU}_${LINK}.txt | grep "^${CRU} ${LINK} ${DS}" > Pedestals_${CRU}_${LINK}_${DS}.txt
		for CHIP in $(seq 0 1); do
		    cat Pedestals_${CRU}_${LINK}_${DS}.txt | grep "^${CRU} ${LINK} ${DS} ${CHIP}" | cut -d" " -f5,7,8 > ${OUTDIRNAME}/ds_pedestals_${CRU}_${LINK}_${DS}_${CHIP}.txt
		done
	    done
	    echo -n " $LINK"
	done
	echo " done."
    done
    rm -f Pedestals_*.txt

    # copy bad channel files ${OUTFILENAME} on each flp
    FLPS="$(cat flplist.txt)"
    for FLP in $FLPS; do
        echo ""
        echo "copying pedestals folder to FLP $FLP"
        scp -q -r ${OUTDIRNAME} mch@alio2-cr1-flp${FLP}.cern.ch:/home/mch/MCHToolbox/scripts/.
        sleep 0.1
    done

else
    echo "  "
    echo "  "
    RED='\033[0;31m'
    NC='\033[0m' # No Color
    echo -n -e "${RED} The Pedestals.txt file from QCDB extraction was not found ${NC}"
    read dummy
fi
