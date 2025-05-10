#! /bin/bash

FORCE_CONFIG=""
if [ x"$1" = "x-f" ]; then FORCE_CONFIG="--force-config"; shift; fi

CRU=$1
shift
ENABLE=$1
shift
LINKS=$*


# links 0-11 are  contains in the first bits i.e 4095 and the following ones 12-23 start from 65536
LINK_MASK=0
for i in $LINKS; do
	if [[ $i -gt 11 ]]; then
	i=$((i+4))
	fi
	BIT=$((1 << $i))
	LINK_MASK=$((LINK_MASK | BIT))
done
#echo "link_mask is $LINK_MASK"
LINK_MASK_HEXA=$(echo "obase=16; $LINK_MASK" | bc)
echo "link_mask in hexa is $LINK_MASK_HEXA"


if [ cru.map -nt env-${CRU}.sh ]; then ./cru-init-env.sh $CRU; fi

source env-${CRU}.sh

ACTION="DISABLED"
if [ x"$ENABLE" = "x0" ]; then
    LINKS_PATT="0x${LINK_MASK_HEXA}"
    #to keep old behavior in case of 
    if [[ -z $LINKS ]]; then
    	  LINKS_PATT="0x0FFF0FFF"
    fi
else
    ACTION="ENABLED"
    LINKS_PATT="0x00000000"
fi



if [[ -z $LINKS ]]; then
	echo "All links from CRU  $CRU  will be ${ACTION}"
else
	echo "Links $LINKS from CRU  $CRU  will be ${ACTION}"
fi


echo "Setting links pattern for CRU $CRU to ${LINKS_PATT}"
echo "roc-reg-write --id=${CRU_PCI_ADDR} --channel=2 --address=0xc80008 --value=\"${LINKS_PATT}\""
roc-reg-write --id=${CRU_PCI_ADDR} --channel=2 --address=0xc80008 --value="${LINKS_PATT}"
