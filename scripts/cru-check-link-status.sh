#! /bin/bash

CRU=$1

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

exit $RESULT
