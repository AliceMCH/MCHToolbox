#! /bin/bash

SCRIPTDIR=$(readlink -f $(dirname $0))
for CRU in $(seq 0 31); do
    if [ ! -e ${SCRIPTDIR}/solar-clock-phases-${CRU}.txt ]; then continue; fi
    rm -rf ${SCRIPTDIR}/solar-clock-phases-${CRU}.txt
done
for CRU in $(seq 0 31); do
    if [ ! -e ${SCRIPTDIR}/env-${CRU}.sh ]; then continue; fi
    source ${SCRIPTDIR}/env-${CRU}.sh

    NLINKS=$(echo "${CRU_LINKS1}" | tr "," "\n" | wc -l)
    for L in $(seq 1 $NLINKS); do
	
	LINK=$(echo "${CRU_LINKS1}" | tr "," "\n" | sed -n ${L}p)
	echo "LINK: $LINK"
	if [ -z "$LINK" ]; then continue; fi
	LINKID=$(echo "${CRU_LINKIDS}" | tr "," "\n" | sed -n ${L}p)
	echo "LINKID: $LINKID"
	if [ -z "$LINKID" ]; then continue; fi

	if [ ! -e "${SCRIPTDIR}/GBT Clock Phases/solar-clock-phase-${CRU}-${LINK}.txt" ]; then continue; fi

	CRATEID=$(echo "scale=0; $LINKID / 8" | bc)
	PHASES=$(cat "${SCRIPTDIR}/GBT Clock Phases/solar-clock-phase-${CRU}-${LINK}.txt")

	echo "$LINK $CRATEID $PHASES" >> ${SCRIPTDIR}/solar-clock-phases-${CRU}.txt

    done

done
