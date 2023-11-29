#! /bin/bash

FORCE_CONFIG=""
if [ x"$1" = "x-f" ]; then FORCE_CONFIG="--force-config"; shift; fi

CRU=$1

source env-${CRU}.sh

P1=$(roc-reg-read --id=${CRU_PCI_ADDR} --ch=2 --add=0x00260004 | cut -d"x" -f 2)
P2=$(roc-reg-read --id=${CRU_PCI_ADDR} --ch=2 --add=0x00260008 | cut -d"x" -f 2)
P3=$(roc-reg-read --id=${CRU_PCI_ADDR} --ch=2 --add=0x0026000C | cut -d"x" -f 2)

while [ true ]; do
    L1=$(echo ${P1} | wc -c)
    L2=$(echo ${P2} | wc -c)
    L3=$(echo ${P3} | wc -c)

    if [ $L1 -le 8 ]; then
	P1="0${P1}"
    fi
    if [ $L2 -le 8 ]; then
	P2="0${P2}"
    fi
    if [ $L3 -le 8 ]; then
	P3="0${P3}"
    fi

    if [ $L1 -gt 8 -a $L2 -gt 8 -a $L3 -gt 8 ]; then
	break
    fi
done

echo "PAT0: 0x${P3}${P2}${P1}"

P1=$(roc-reg-read --id=${CRU_PCI_ADDR} --ch=2 --add=0x00260010 | cut -d"x" -f 2)
P2=$(roc-reg-read --id=${CRU_PCI_ADDR} --ch=2 --add=0x00260014 | cut -d"x" -f 2)
P3=$(roc-reg-read --id=${CRU_PCI_ADDR} --ch=2 --add=0x00260018 | cut -d"x" -f 2)

while [ true ]; do
    L1=$(echo ${P1} | wc -c)
    L2=$(echo ${P2} | wc -c)
    L3=$(echo ${P3} | wc -c)

    if [ $L1 -le 8 ]; then
	P1="0${P1}"
    fi
    if [ $L2 -le 8 ]; then
	P2="0${P2}"
    fi
    if [ $L3 -le 8 ]; then
	P3="0${P3}"
    fi

    if [ $L1 -gt 8 -a $L2 -gt 8 -a $L3 -gt 8 ]; then
	break
    fi
done

echo "PAT1: 0x${P3}${P2}${P1}"

P1=$(roc-reg-read --id=${CRU_PCI_ADDR} --ch=2 --add=0x0026001C | cut -d"x" -f 2)
P2=$(roc-reg-read --id=${CRU_PCI_ADDR} --ch=2 --add=0x00260020 | cut -d"x" -f 2)
P3=$(roc-reg-read --id=${CRU_PCI_ADDR} --ch=2 --add=0x00260024 | cut -d"x" -f 2)

while [ true ]; do
    L1=$(echo ${P1} | wc -c)
    L2=$(echo ${P2} | wc -c)
    L3=$(echo ${P3} | wc -c)

    if [ $L1 -le 8 ]; then
	P1="0${P1}"
    fi
    if [ $L2 -le 8 ]; then
	P2="0${P2}"
    fi
    if [ $L3 -le 8 ]; then
	P3="0${P3}"
    fi

    if [ $L1 -gt 8 -a $L2 -gt 8 -a $L3 -gt 8 ]; then
	break
    fi
done

echo "PAT2: 0x${P3}${P2}${P1}"
echo ""

REG=$(roc-reg-read --id=${CRU_PCI_ADDR} --ch=2 --add=0x00260040)
echo "TIME STAMP:  ${REG}"

REG=$(roc-reg-read --id=${CRU_PCI_ADDR} --ch=2 --add=0x00260034)
echo "TRIG MASK 1: ${REG}"

REG=$(roc-reg-read --id=${CRU_PCI_ADDR} --ch=2 --add=0x00260038)
echo "TRIG MASK 2: ${REG}"
