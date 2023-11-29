#! /bin/bash

CRU=$1

SCRIPTDIR=$(readlink -f $(dirname $0))
source ${SCRIPTDIR}/env-${CRU}.sh

#prefix=/tmp/

echo "Configuring trigger for CRU $CRU"

PATT0=${PATT_IDLE}
PATT1=${PATT_IDLE}
PATT2=${PATT_IDLE}
PATT2=${PATT_IDLE}
PATT3=${PATT_IDLE}
TRIGMASK1=0x0
TRIGMASK2=0x0
TRIGMASK3=0x0
TFRATE=351 # Hz
TSTAMP=0

if [ x"$RUNTYPE" = "xpedestals" -o x"$RUNTYPE" = "xpedestals_ul" ]; then
    PATT0=${PATT_IDLE}
    
    PATT1=${PATT_HBTR_SYNC}
    # bits 11 & 12, TF start and FE reset
    TRIGMASK1=0x1800
    
    PATT2=${PATT_PHYS}
    # physics trigger, sent via TF/ORBIT/BC mask
    TRIGMASK2=0x0

    # PHYS trigger rate of 4 Hz
    TSTAMP="0x"$(echo "obase=16; scale=0; ${TFRATE} / 4" | bc)"01DEB"
    echo $TSTAMP
fi


if [ x"$RUNTYPE" = "xphysics_continuous_ul" -o x"$RUNTYPE" = "xphysics_continuous_ul_csum" -o x"$RUNTYPE" = "xphysics_continuous" -o x"$RUNTYPE" = "xphysics_continuous_csum" ]; then
    PATT0=${PATT_IDLE}

    # SYNC + HB at TF start (bit 11)
    PATT1=${PATT_HBTR_SYNC}
    # bit 11, TF start
    TRIGMASK1=0x800

    # SYNC at FE reset (bit 12)
    PATT2=${PATT_SYNC}
    # bit 12, FE reset
    TRIGMASK2=0x1000
fi


for addr in ${CRU_PCI_ADDR1} ${CRU_PCI_ADDR2}; do

# PAT0 IDLE
P0=$(echo -n ${PATT0} | cut -c 15-22)
P1=$(echo -n ${PATT0} | cut -c 7-14)
P2=$(echo -n ${PATT0} | cut -c 3-6)
echo "PAT0: $PATT0   P0: $P0   P1: $P1   P2: $P2"
${prefix}o2-roc-reg-write --id=${addr} --ch=2 --address=00260004 --val=0x${P0}
${prefix}o2-roc-reg-write --id=${addr} --ch=2 --address=00260008 --val=0x${P1}
${prefix}o2-roc-reg-write --id=${addr} --ch=2 --address=0026000C --val=0x${P2}
# PAT1 SYNC
P0=$(echo -n ${PATT1} | cut -c 15-22)
P1=$(echo -n ${PATT1} | cut -c 7-14)
P2=$(echo -n ${PATT1} | cut -c 3-6)
echo "PAT1: $PATT1   P0: $P0   P1: $P1   P2: $P2"
${prefix}o2-roc-reg-write --id=${addr} --ch=2 --address=00260010 --val=0x${P0}
${prefix}o2-roc-reg-write --id=${addr} --ch=2 --address=00260014 --val=0x${P1}
${prefix}o2-roc-reg-write --id=${addr} --ch=2 --address=00260018 --val=0x${P2}
# PAT2 RESET
P0=$(echo -n ${PATT2} | cut -c 15-22)
P1=$(echo -n ${PATT2} | cut -c 7-14)
P2=$(echo -n ${PATT2} | cut -c 3-6)
echo "PAT2: $PATT2   P0: $P0   P1: $P1   P2: $P2"
${prefix}o2-roc-reg-write --id=${addr} --ch=2 --address=0026001C --val=0x${P0}
${prefix}o2-roc-reg-write --id=${addr} --ch=2 --address=00260020 --val=0x${P1}
${prefix}o2-roc-reg-write --id=${addr} --ch=2 --address=00260024 --val=0x${P2}

# PAT 1 LENGTH
${prefix}o2-roc-reg-write --id=${addr} --ch=2 --address=00260028 --val=0x1
# PAT 1 DELAY
${prefix}o2-roc-reg-write --id=${addr} --ch=2 --address=0026002C --val=0x0
# PAT 2 LENGTH
${prefix}o2-roc-reg-write --id=${addr} --ch=2 --address=00260030 --val=0x1
# PAT 3 LENGTH
${prefix}o2-roc-reg-write --id=${addr} --ch=2 --address=00260044 --val=0x1

#### TRIGGER MASK ... NOT CODE ANYMORE
# PAT 1 TRG MASK
${prefix}o2-roc-reg-write --id=${addr} --ch=2 --address=00260034 --val=${TRIGMASK1}
# PAT 2 TRG MASK
echo "${prefix}o2-roc-reg-write --id=${addr} --ch=2 --address=00260038 --val=${TRIGMASK2}"
${prefix}o2-roc-reg-write --id=${addr} --ch=2 --address=00260038 --val=${TRIGMASK2}

#### TIME_STAMP
# PAT 2 TIME STAMP TF[31:20] ORBIT[19:12] BC[11:0]
${prefix}o2-roc-reg-write --id=${addr} --ch=2 --address=00260040 --val=${TSTAMP}

done
