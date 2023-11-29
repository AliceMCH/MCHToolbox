#! /bin/bash

CRU=$1

SCRIPTDIR=$(readlink -f $(dirname $0))
source ${SCRIPTDIR}/env-${CRU}.sh

echo "Configuring trigger for CRU $CRU"

if [ x"$RUNTYPE" = "xpedestals_ul" ]; then
    
    PATT1=${PATT_IDLE}
    PATT2=${PATT_PHYS}
    PATT3=${PATT_SYNC}
    echo "roc-pat-player --id=${CRU_PCI_ADDR} --idle=${PATT1} --sync=${PATT2} --reset=${PATT3} --sync-length=1 --sync-delay=0 --sync-trigger-select=11 --reset-trigger-select=12"
    for addr in ${CRU_PCI_ADDR1} ${CRU_PCI_ADDR2}; do

	# reset pat player
	${prefix}o2-roc-reg-write --id=${addr} --ch=2 --address=00260000 --val=0x1
	${prefix}o2-roc-reg-write --id=${addr} --ch=2 --address=00260000 --val=0x0
	
	P0=$(echo -n ${PATT1} | cut -c 15-22)
	P1=$(echo -n ${PATT1} | cut -c 7-14)
	P2=$(echo -n ${PATT1} | cut -c 3-6)
	${prefix}o2-roc-reg-write --id=${addr} --ch=2 --address=00260004 --val=0x${P0}
	${prefix}o2-roc-reg-write --id=${addr} --ch=2 --address=00260008 --val=0x${P1}
	${prefix}o2-roc-reg-write --id=${addr} --ch=2 --address=0026000C --val=0x${P2}
	

	P0=$(echo -n ${PATT2} | cut -c 15-22)
	P1=$(echo -n ${PATT2} | cut -c 7-14)
	P2=$(echo -n ${PATT2} | cut -c 3-6)
	${prefix}o2-roc-reg-write --id=${addr} --ch=2 --address=00260010 --val=0x${P0}
	${prefix}o2-roc-reg-write --id=${addr} --ch=2 --address=00260014 --val=0x${P1}
	${prefix}o2-roc-reg-write --id=${addr} --ch=2 --address=00260018 --val=0x${P2}


	P0=$(echo -n ${PATT3} | cut -c 15-22)
	P1=$(echo -n ${PATT3} | cut -c 7-14)
	P2=$(echo -n ${PATT3} | cut -c 3-6)
	${prefix}o2-roc-reg-write --id=${addr} --ch=2 --address=0026001C --val=0x${P0}
	${prefix}o2-roc-reg-write --id=${addr} --ch=2 --address=00260020 --val=0x${P1}
	${prefix}o2-roc-reg-write --id=${addr} --ch=2 --address=00260024 --val=0x${P2}

    
	# SYN LENGTH
	${prefix}o2-roc-reg-write --id=${addr} --ch=2 --address=00260028 --val=0x1
	# SYN DELAY
	${prefix}o2-roc-reg-write --id=${addr} --ch=2 --address=0026002C --val=0x0
	# RESET LENGTH
	${prefix}o2-roc-reg-write --id=${addr} --ch=2 --address=00260030 --val=0xBB8
	
	# TRGSEL
	# [4:0] = TRG SYN = 4
	# [20:16] = TRG RST = 12
	${prefix}o2-roc-reg-write --id=${addr} --ch=2 --address=00260034 --val=0xC0004

    done
    exit
fi

CONTINUOUS=0
if [ x"$RUNTYPE" = "xphysics_continuous_ul" -o x"$RUNTYPE" = "xphysics_continuous_ul_csum" -o x"$RUNTYPE" = "xphysics_continuous" -o x"$RUNTYPE" = "xphysics_continuous_csum" ]; then
    CONTINUOUS=1
fi

echo "CONTINUOUS ${CONTINUOUS}"

if [ x"${CONTINUOUS}" = "x1" ]; then

    PATT1=${PATT_IDLE}
    PATT2=${PATT_HBTR_SYNC}
    PATT3=${PATT_SYNC}
    echo "roc-pat-player --id=${CRU_PCI_ADDR} --idle=${PATT1} --sync=${PATT2} --reset=${PATT3} --sync-length=1 --sync-delay=0 --sync-trigger-select=11 --reset-trigger-select=12"
    for addr in ${CRU_PCI_ADDR1} ${CRU_PCI_ADDR2}; do

	# reset pat player
	${prefix}o2-roc-reg-write --id=${addr} --ch=2 --address=00260000 --val=0x1
	${prefix}o2-roc-reg-write --id=${addr} --ch=2 --address=00260000 --val=0x0
	
	P0=$(echo -n ${PATT1} | cut -c 15-22)
	P1=$(echo -n ${PATT1} | cut -c 7-14)
	P2=$(echo -n ${PATT1} | cut -c 3-6)
	${prefix}o2-roc-reg-write --id=${addr} --ch=2 --address=00260004 --val=0x${P0}
	${prefix}o2-roc-reg-write --id=${addr} --ch=2 --address=00260008 --val=0x${P1}
	${prefix}o2-roc-reg-write --id=${addr} --ch=2 --address=0026000C --val=0x${P2}
	

	P0=$(echo -n ${PATT2} | cut -c 15-22)
	P1=$(echo -n ${PATT2} | cut -c 7-14)
	P2=$(echo -n ${PATT2} | cut -c 3-6)
	${prefix}o2-roc-reg-write --id=${addr} --ch=2 --address=00260010 --val=0x${P0}
	${prefix}o2-roc-reg-write --id=${addr} --ch=2 --address=00260014 --val=0x${P1}
	${prefix}o2-roc-reg-write --id=${addr} --ch=2 --address=00260018 --val=0x${P2}


	P0=$(echo -n ${PATT3} | cut -c 15-22)
	P1=$(echo -n ${PATT3} | cut -c 7-14)
	P2=$(echo -n ${PATT3} | cut -c 3-6)
	${prefix}o2-roc-reg-write --id=${addr} --ch=2 --address=0026001C --val=0x${P0}
	${prefix}o2-roc-reg-write --id=${addr} --ch=2 --address=00260020 --val=0x${P1}
	${prefix}o2-roc-reg-write --id=${addr} --ch=2 --address=00260024 --val=0x${P2}

    
	# SYN LENGTH
	${prefix}o2-roc-reg-write --id=${addr} --ch=2 --address=00260028 --val=0x1
	# SYN DELAY
	${prefix}o2-roc-reg-write --id=${addr} --ch=2 --address=0026002C --val=0x0
	# RESET LENGTH
	${prefix}o2-roc-reg-write --id=${addr} --ch=2 --address=00260030 --val=0xBB8
	
	# TRGSEL
	# [4:0] = TRG SYN = 4
	# [20:16] = TRG RST = 12
	${prefix}o2-roc-reg-write --id=${addr} --ch=2 --address=00260034 --val=0xC000B

    done
    exit
fi

exit

### THIS IS THE OLD CODE

if [ x"$RUNTYPE" = "xpedestals" ]; then
    PATT1=${PATT_IDLE}
    PATT2=${PATT_PHYS}
    PATT3=${PATT_SYNC}
    echo "roc-pat-player --id=${CRU_PCI_ADDR} --idle=${PATT1} --sync=${PATT2} --reset=${PATT3} --sync-length=1 --sync-delay=0 --sync-trigger-select=4 --reset-trigger-select=12"
    roc-pat-player --id=${CRU_PCI_ADDR} --idle=${PATT1} \
	--sync=${PATT2} \
        --reset=${PATT3} \
	--sync-length=1 --sync-delay=0 --sync-trigger-select=4 --reset-trigger-select=12
    exit
fi

if [ x"$RUNTYPE" = "xpedestals_ul" ]; then
    PATT1=${PATT_IDLE}

    #PATT2=${PATT_SYNC}
    PATT2=${PATT_PHYS}
    #PATT2=${PATT_PHYS_SYNC}
    #PATT2=${PATT_HBTR}
    #PATT2=${PATT_PHYS_HBTR_SYNC}

    PATT3=${PATT_SYNC}
    echo "roc-pat-player --id=${CRU_PCI_ADDR} --idle=${PATT1} --sync=${PATT2} --reset=${PATT3} --sync-length=1 --sync-delay=0 --sync-trigger-select=11 --reset-trigger-select=12"
    roc-pat-player --id=${CRU_PCI_ADDR} --idle=${PATT1} \
	--sync=${PATT2} \
        --reset=${PATT3} \
	--sync-length=1 --sync-delay=0 --sync-trigger-select=4 --reset-trigger-select=12
    exit
fi

if [ x"$RUNTYPE" = "xber_check" ]; then
    roc-pat-player --id=${CRU_PCI_ADDR} --idle=${PATT_IDLE} \
	--sync=${PATT_IDLE} \
        --reset=${PATT_IDLE} \
	--sync-length=1 --sync-delay=0 --sync-trigger-select=4 --reset-trigger-select=12
    exit
fi

if [ x"$RUNTYPE" = "xhb_check" ]; then
    roc-pat-player --id=${CRU_PCI_ADDR} --idle=${PATT_IDLE} \
	--sync=${PATT_HBTR} \
        --reset=${PATT_SYNC} \
	--sync-length=1 --sync-delay=0 --sync-trigger-select=4 --reset-trigger-select=12
    exit
fi

SEND_HBT=1
if [ x"$RUNTYPE" = "xpedestals" -o x"$RUNTYPE" = "xpedestals_ul" -o x"$RUNTYPE" = "xber_check" ]; then
    SEND_HBT=0
fi

echo "SEND_HBT: ${SEND_HBT}"

CONTINUOUS=0
if [ x"$RUNTYPE" = "xphysics_continuous_ul" -o x"$RUNTYPE" = "xphysics_continuous_ul_csum" -o x"$RUNTYPE" = "xphysics_continuous" -o x"$RUNTYPE" = "xphysics_continuous_csum" ]; then
    CONTINUOUS=1
fi

echo "CONTINUOUS ${CONTINUOUS}"

if [ x"${CONTINUOUS}" = "x1" ]; then
    PATT1=${PATT_IDLE}
    #PATT2=${PATT_HBTR}
    PATT2=${PATT_HBTR_SYNC}
    PATT3=${PATT_SYNC}
    echo "roc-pat-player --id=${CRU_PCI_ADDR} --idle=${PATT1} --sync=${PATT2} --reset=${PATT3} --sync-length=1 --sync-delay=0 --sync-trigger-select=11 --reset-trigger-select=12"
    roc-pat-player --id=${CRU_PCI_ADDR} --idle=${PATT1} \
	--sync=${PATT2} \
	--reset=${PATT3} \
	--sync-length=1 --sync-delay=0 --sync-trigger-select=11 --reset-trigger-select=12

    exit
fi


if [ x"${SEND_HBT}" = "x1" ]; then
    echo "roc-pat-player --id=${CRU_PCI_ADDR} --idle=${PATT_IDLE} --sync=${PATT_PHYS} --reset=${PATT_HBTR} --sync-length=1 --sync-delay=0 --sync-trigger-select=4 --reset-trigger-select=11"
    roc-pat-player --id=${CRU_PCI_ADDR} --idle=${PATT_IDLE} \
	--sync=${PATT_PHYS} \
	--reset=${PATT_IDLE} \ #${PATT_HBTR} \
	--sync-length=1 --sync-delay=0 --sync-trigger-select=4 --reset-trigger-select=11
else
    roc-pat-player --id=${CRU_PCI_ADDR} --idle=${PATT_IDLE} \
	--sync=${PATT_PHYS} \
        --reset=${PATT_SYNC} \
	--sync-length=1 --sync-delay=0 --sync-trigger-select=4 --reset-trigger-select=12
fi

