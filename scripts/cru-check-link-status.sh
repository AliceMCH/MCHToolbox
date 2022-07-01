#! /bin/bash

CRU=$1
shift
TGTLINKS=$*
if [ -n "$TGTLINKS" ]; then
    TGTLINKS=" $TGTLINKS "
fi

SCRIPTDIR=$(dirname $0)
source ${SCRIPTDIR}/env-${CRU}.sh

./cru-get-links.sh $CRU

RESULT=0
for DW in 0 1; do
    if [ $DW -eq 0 -a x"${CRU_LINKS1_1}" = "x" ]; then
	continue;
    fi
    if [ $DW -eq 1 -a x"${CRU_LINKS1_2}" = "x" ]; then
	continue;
    fi
    TEST=$(./cru-link-status.sh $CRU $DW)
    echo -n "CRU $1:$DW link status: "
    OK=1
    if [ -n "$TEST" ]; then
	NL=$(echo "$TEST" | wc -l)
	for L in $(seq 1 $NL); do
	    LINE=$(echo "$TEST" | sed -n ${L}p)
	    #echo "$LINE"

	    LINK=$(echo "$LINE" | tr -s " " | cut -d" " -f 2)
	    if [ $DW -eq 1 ]; then LINK=$((LINK+12)); fi

	    STATUS=$(echo "$LINE" | tr -s " " | cut -d" " -f 10)
	    #echo "STATUS: $STATUS"
	    if [ x"$STATUS" != "xUP" ]; then
		OK=0
		RESULT=1
		echo ""
		echo "$LINE"
		./cru-describe-link.sh $CRU $LINK
	    fi
	done
    fi
    if [ $OK -eq 0 ]; then
	echo "Some links are down"
    else
	echo "OK"
    fi
done

echo ""; echo "";

GBTCMD=../tools/command-line/build/gbt-config

echo "CRU $1 read/write status: "
NLINKS=$(echo "${CRU_LINKS1}" | tr "," "\n" | wc -l)
OK=1
for L in $(seq 1 $NLINKS); do

    LINK=$(echo "${CRU_LINKS1}" | tr "," "\n" | sed -n ${L}p)
    #echo "LINK: $LINK"
    if [ -z "$LINK" ]; then continue; fi

    if [ -n "$TGTLINKS" ]; then
	FOUND=$(echo "$TGTLINKS" | grep " $LINK ")
	if [ x"$FOUND" = "x" ]; then
	    continue
	fi
    fi

    for VAL in 170 0; do
	#echo "Writing $VAL into register 81 of ${CRU}/${CRU_PCI_ADDR}/${LINK}"
	#echo "$GBTCMD ${CRU_PCI_ADDR1} ${CRU_PCI_ADDR2} \"w\" ${LINK} 81 170"
	STATUS=$($GBTCMD ${CRU_PCI_ADDR1} ${CRU_PCI_ADDR2} "w" ${LINK} 81 $VAL)
	RET=$?
	if [ $RET != 0 ]; then
	    OK=0
	    RESULT=1
	    echo ""
	    echo "$STATUS"
	    ./cru-describe-link.sh $CRU $LINK
	    break;
	fi

	#echo "Reading register 81 from ${CRU}/${CRU_PCI_ADDR}/${LINK}"
	#echo "$GBTCMD ${CRU_PCI_ADDR1} ${CRU_PCI_ADDR2} \"r\" ${LINK} 81"
	READ=$($GBTCMD ${CRU_PCI_ADDR1} ${CRU_PCI_ADDR2} "r" ${LINK} 81 | cut -d ' ' -f 3)
	if [ x"$READ" != x"$VAL" ]; then
	    OK=0
	    RESULT=1
	    echo ""
	    echo "Expected $VAL, got $READ"
	    ./cru-describe-link.sh $CRU $LINK
	    break;
	fi
    done

    if [ $OK -eq 1 ]; then
	echo "Link $LINK OK"
    fi

done

if [ $OK -eq 0 ]; then
    echo "Some links are not responding"
else
    echo "OK"
fi

exit $RESULT
