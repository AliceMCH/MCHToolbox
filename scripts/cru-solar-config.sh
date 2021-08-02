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

while [ true ]; do
    bash ./cru-check-link-status.sh $CRU
    if [ $? -eq 0 ]; then
	break
    fi
    echo -n "Some SOLAR boards are DOWN, please try to power-cycle the corresponding crates and press enter..."
    read dummy
done

bash ./cru-get-ds-enabled.sh $CRU

#export CRU


REGFILE=gbtx-regs-${CRU}.txt
#rm -f "$REGFILE"


if [ ! -e $REGFILE -o $REGFILE -ot board-enable-${CRU}.txt ]; then

    result=0
    NLINKS=$(echo "${CRU_LINKS1}" | tr "," "\n" | wc -l)
    for L in $(seq 1 $NLINKS); do

	LINK=$(echo "${CRU_LINKS1}" | tr "," "\n" | sed -n ${L}p)
	echo "LINK: $LINK"
	if [ -z "$LINK" ]; then continue; fi

	if [ $TGTLINK -ge 0 -a x"$TGTLINK" != x"$LINK" ]; then continue; fi

	echo "./solar-config.sh ${CRU} $LINK"
	bash ./solar-config.sh ${CRU} $LINK $REGFILE
	result=$?
	if [ x"$result" != "x0" ]; then
	    break
	fi

    done

fi

GBTCMD=../tools/command-line/build/gbt-config

echo "Configuring ${CRU}/${CRU_PCI_ADDR}"
echo "$GBTCMD ${CRU_PCI_ADDR1} ${CRU_PCI_ADDR2} \"f\" \"$REGFILE\""
$GBTCMD ${CRU_PCI_ADDR1} ${CRU_PCI_ADDR2} "f" "$REGFILE" || exit 1


sleep 1

#bash ./cru_solar_phase_aligner_reset.sh $CRU

if [ x"$WAIT" = "x1" ]; then
    echo ""
    echo "Programming finished..."
    read dummy
fi

exit $result
