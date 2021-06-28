#! /bin/bash

CRU=$1
TGTLINK=$2
if [ x"$TGTLINK" = "x" ]; then
    TGTLINK=-1
fi
if [ x"$WAIT" = "x" ]; then
    WAIT=0
fi

source env-${CRU}.sh

bash ./cru-check-link-status.sh $CRU
if [ $? -ne 0 ]; then
    exit 1
fi

bash ./cru-get-ds-enabled.sh $CRU

#export CRU

# program the SOLAR boards
result=0
NLINKS=$(echo "${CRU_LINKS1}" | tr "," "\n" | wc -l)
for L in $(seq 1 $NLINKS); do

	LINK=$(echo "${CRU_LINKS1}" | tr "," "\n" | sed -n ${L}p)
	echo "LINK: $LINK"
	if [ -z "$LINK" ]; then continue; fi

	if [ $TGTLINK -ge 0 -a x"$TGTLINK" != x"$LINK" ]; then continue; fi

	#TEST_DOWN=$(./link_status.sh ${LINK} | grep DOWN)
	#if [ -n "${TEST_DOWN}" ]; then continue; fi

	#bash ./link_is_up.sh ${CRU_ADDR} ${LINK}
	#ISDOWN=$?
	#if [ $ISDOWN -eq 1 ]; then continue; fi

	echo "./solar-config.sh ${CRU} $LINK"
	bash ./solar-config.sh ${CRU} $LINK
	result=$?
	if [ x"$result" != "x0" ]; then
	    break
	fi

done

sleep 1

#bash ./cru_solar_phase_aligner_reset.sh $CRU

if [ x"$WAIT" = "x1" ]; then
    echo ""
    echo "Programming finished..."
    read dummy
fi

exit $result
