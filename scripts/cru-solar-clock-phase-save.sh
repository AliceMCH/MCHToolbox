#! /bin/bash

CRU=$1

source env-${CRU}.sh

# program the SOLAR boards
NLINKS=$(echo "${CRU_LINKS1}" | tr "," "\n" | wc -l)
for L in $(seq 1 $NLINKS); do

	LINK=$(echo "${CRU_LINKS1}" | tr "," "\n" | sed -n ${L}p)
	echo "LINK: $LINK"
	if [ -z "$LINK" ]; then continue; fi

	#echo "./solar_clock_phase_aligner_reset_start.sh ${CRU_PCI_ADDR} 0 $LINK"
	echo "./solar-clock-phase-read.sh ${CRU} $LINK"
	bash ./solar-clock-phase-read.sh ${CRU} $LINK >& /tmp/solar-clock-phase-${CRU}-${LINK}.txt
	cat /tmp/solar-clock-phase-${CRU}-${LINK}.txt
	cat /tmp/solar-clock-phase-${CRU}-${LINK}.txt | tail -n 1 > solar-clock-phase-${CRU}-${LINK}.txt

done
