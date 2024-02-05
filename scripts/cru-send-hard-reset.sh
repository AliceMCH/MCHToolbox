#! /bin/bash

CRU=$1

SCRIPTDIR=$(dirname $0)
source ${SCRIPTDIR}/env-${CRU}.sh
 
roc-pat-player --id=${CRU_PCI_ADDR} --pat0=${PATT_IDLE} --pat2=0x0 --pat2-length=3000  --execute-pat2-now
roc-pat-player --id=${CRU_PCI_ADDR} --read-back 

exit


PATT1=${PATT_IDLE}
P0=$(echo -n ${PATT1} | cut -c 15-22)
P1=$(echo -n ${PATT1} | cut -c 7-14)
P2=$(echo -n ${PATT1} | cut -c 3-6)

for addr in ${CRU_PCI_ADDR1} ${CRU_PCI_ADDR2}; do

    ${prefix}o2-roc-reg-write --id=${addr} --ch=2 --address=00260004 --val=0x${P0}
    ${prefix}o2-roc-reg-write --id=${addr} --ch=2 --address=00260008 --val=0x${P1}
    ${prefix}o2-roc-reg-write --id=${addr} --ch=2 --address=0026000C --val=0x${P2}

    ${prefix}o2-roc-reg-write --id=${addr} --ch=2 --address=0026001C --val=0x0
    ${prefix}o2-roc-reg-write --id=${addr} --ch=2 --address=00260020 --val=0x0
    ${prefix}o2-roc-reg-write --id=${addr} --ch=2 --address=00260024 --val=0x0

    # RESET LENGTH
    ${prefix}o2-roc-reg-write --id=${addr} --ch=2 --address=00260030 --val=0xBB8

    # TRIGGER RESET
    ${prefix}o2-roc-reg-write --id=${addr} --ch=2 --address=00260000 --val=0x10

done

#roc-pat-player --id=${CRU_PCI_ADDR} --idle=${PATT_IDLE} --reset 0x0 --reset-length=3000  --trigger-reset
