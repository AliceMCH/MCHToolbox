#! /bin/bash

CRU=$1


export I2C_RETRIES=2

#CRU_CLKSRC=local
CRU_CLKSRC=ttc

#RUNTYPE=ber_check
RUNTYPE=pedestals
#RUNTYPE=pedestals_ul
#RUNTYPE=physics_triggered
#RUNTYPE=physics_triggered_csum
#RUNTYPE=physics_triggered_ul
#RUNTYPE=physics_triggered_ul_csum
#RUNTYPE=physics_continuous_ul
#RUNTYPE=physics_continuous_ul_csum
#RUNTYPE=hb_check


export ADC_THR=0C
export NOISE_THR=2.0

#export ADC_THR=10
#export NOISE_THR=100

#export ADC_THR=14
#export NOISE_THR=100

#export ADC_THR=FF


CRU_UL_ENABLED=0
if [ x"${RUNTYPE}" = "xpedestals_ul" ] ||              \
   [ x"${RUNTYPE}" = "xphysics_triggered_ul" ] ||      \
   [ x"${RUNTYPE}" = "xphysics_triggered_ul_csum" ] || \
   [ x"${RUNTYPE}" = "xphysics_continuous_ul" ] ||     \
   [ x"${RUNTYPE}" = "xphysics_continuous_ul_csum" ] ||     \
   [ x"${RUNTYPE}" = "xhb_check" ] ||     \
   [ x"${RUNTYPE}" = "xber_check" ]; then
    CRU_UL_ENABLED=1
fi
    
CRU_CSUM_ENABLED=0
if [ x"${RUNTYPE}" = "xphysics_triggered_ul_csum" ] || \
   [ x"${RUNTYPE}" = "xphysics_continuous_ul_csum" ]; then
    CRU_CSUM_ENABLED=1
fi

PCIADDR_POS=1
FEEID_POS=2
SN_POS=3
DW_POS=4
LINKS1_POS=5
LINKS2_POS=6
LINKIDS_POS=7

CRU_PCI_ADDR=""
CRU_PCI_ADDR2=""

CRU_LINKS1_1=""
CRU_LINKS1_2=""
CRU_LINKS1=""
CRU_LINKS2_1=""
CRU_LINKS2_2=""
CRU_LINKS2=""
CRU_LINKIDS_1=""
CRU_LINKIDS_2=""
CRU_LINKIDS=""


LINKS="$(source ./cru-get-links.sh $CRU)"
#echo "$LINKS"


NL=$(echo "$LINKS" | wc -l)
for I in $(seq 1 $NL); do

    L=$(echo "$LINKS" | sed -n ${I}p)

    DW=$(echo "$L" | cut -d" " -f ${DW_POS})
    if [ x"$DW" = "x0" ]; then
	CRU_PCI_ADDR1=$(echo "$L" | cut -d" " -f ${PCIADDR_POS})
	CRU_LINKS1_1=$(echo "$L" | cut -d" " -f ${LINKS1_POS})
	CRU_LINKS2_1=$(echo "$L" | cut -d" " -f ${LINKS2_POS})
	CRU_LINKIDS_1=$(echo "$L" | cut -d" " -f ${LINKIDS_POS})
    fi
    if [ x"$DW" = "x1" ]; then
	CRU_PCI_ADDR2=$(echo "$L" | cut -d" " -f ${PCIADDR_POS})
	CRU_LINKS1_2=$(echo "$L" | cut -d" " -f ${LINKS1_POS})
	CRU_LINKS2_2=$(echo "$L" | cut -d" " -f ${LINKS2_POS})
	CRU_LINKIDS_2=$(echo "$L" | cut -d" " -f ${LINKIDS_POS})
    fi

    CRU_SN=$(echo "$L" | cut -d" " -f ${SN_POS})
    FEEID=$(echo "$L" | cut -d" " -f ${FEEID_POS})

done


if [ -n "${CRU_LINKS1_1}" ]; then
    CRU_LINKS1="${CRU_LINKS1_1}"
    if [ -n "${CRU_LINKS1_2}" ]; then
	CRU_LINKS1="${CRU_LINKS1},"
    fi
fi
if [ -n "${CRU_LINKS1_2}" ]; then
    CRU_LINKS1="${CRU_LINKS1}${CRU_LINKS1_2}"
fi


if [ -n "${CRU_LINKS2_1}" ]; then
    CRU_LINKS2="${CRU_LINKS2_1}"
    if [ -n "${CRU_LINKS2_2}" ]; then
	CRU_LINKS2="${CRU_LINKS2},"
    fi
fi
if [ -n "${CRU_LINKS2_2}" ]; then
    CRU_LINKS2="${CRU_LINKS2}${CRU_LINKS2_2}"
fi


if [ -n "${CRU_LINKIDS_1}" ]; then
    CRU_LINKIDS="${CRU_LINKIDS_1}"
    if [ -n "${CRU_LINKIDS_2}" ]; then
	CRU_LINKIDS="${CRU_LINKIDS},"
    fi
fi
if [ -n "${CRU_LINKIDS_2}" ]; then
    CRU_LINKIDS="${CRU_LINKIDS}${CRU_LINKIDS_2}"
fi

if [ -z "${CRU_PCI_ADDR1}" ]; then
    CRU_PCI_ADDR1=${CRU_PCI_ADDR2}
fi
if [ -z "${CRU_PCI_ADDR2}" ]; then
    CRU_PCI_ADDR2=${CRU_PCI_ADDR1}
fi

CRU_PCI_ADDR=${CRU_PCI_ADDR1}

ONUID=12
if [ -e ltu.map ]; then
    #echo "cat ltu.map | grep \"^${CRU_SN}\" | cut -f 2"
    ONUID=$(cat ltu.map | grep "^${CRU_SN}" | cut -f 2)
fi
if [ -z "$ONUID" ]; then
    ONUID=12
fi

export RUNTYPE
export CRU_PCI_ADDR
export CRU_PCI_ADDR1
export CRU_PCI_ADDR2
export CRU_CLKSRC
export CRU_UL_ENABLED
export CRU_CSUM_ENABLED
export FEEID
export ONUID
export CRU_SN
export CRU_LINKS1_1
export CRU_LINKS1_2
export CRU_LINKS1
export CRU_LINKS2_1
export CRU_LINKS2_2
export CRU_LINKS2
export CRU_LINKIDS_1
export CRU_LINKIDS_2
export CRU_LINKIDS

export PATT_IDLE=0x0C0300C0300C0300C030
export PATT_PHYS=0x3C0F03C0F03C0F03C0F0
export PATT_HBTR=0x0CC330CC330CC330CC33
export PATT_SYNC=0x0F03C0F03C0F03C0F03C

ENVFILE=env-${CRU}.sh

echo "export RUNTYPE=${RUNTYPE}" > ${ENVFILE}
echo "export CRU_PCI_ADDR=${CRU_PCI_ADDR}" >> ${ENVFILE}
echo "export CRU_PCI_ADDR1=${CRU_PCI_ADDR1}" >> ${ENVFILE}
echo "export CRU_PCI_ADDR2=${CRU_PCI_ADDR2}" >> ${ENVFILE}
echo "export CRU_CLKSRC=${CRU_CLKSRC}" >> ${ENVFILE}
echo "export CRU_SN=${CRU_SN}" >> ${ENVFILE}
echo "export CRU_UL_ENABLED=${CRU_UL_ENABLED}" >> ${ENVFILE}
echo "export CRU_CSUM_ENABLED=${CRU_CSUM_ENABLED}" >> ${ENVFILE}
echo "export FEEID=${FEEID}" >> ${ENVFILE}
echo "export ONUID=${ONUID}" >> ${ENVFILE}
echo "export CRU_LINKS1_1=${CRU_LINKS1_1}" >> ${ENVFILE}
echo "export CRU_LINKS1_2=${CRU_LINKS1_2}" >> ${ENVFILE}
echo "export CRU_LINKS1=${CRU_LINKS1}" >> ${ENVFILE}
echo "export CRU_LINKS2_1=${CRU_LINKS2_1}" >> ${ENVFILE}
echo "export CRU_LINKS2_2=${CRU_LINKS2_2}" >> ${ENVFILE}
echo "export CRU_LINKS2=${CRU_LINKS2}" >> ${ENVFILE}
echo "export CRU_LINKIDS_1=${CRU_LINKIDS_1}" >> ${ENVFILE}
echo "export CRU_LINKIDS_2=${CRU_LINKIDS_2}" >> ${ENVFILE}
echo "export CRU_LINKIDS=${CRU_LINKIDS}" >> ${ENVFILE}
echo "export PATT_IDLE=${PATT_IDLE}" >> ${ENVFILE}
echo "export PATT_PHYS=${PATT_PHYS}" >> ${ENVFILE}
echo "export PATT_HBTR=${PATT_HBTR}" >> ${ENVFILE}
echo "export PATT_SYNC=${PATT_SYNC}" >> ${ENVFILE}
echo "export NOISE_THR=${NOISE_THR}" >> ${ENVFILE}
echo "export ADC_THR=${ADC_THR}" >> ${ENVFILE}
echo "export I2C_RETRIES=${I2C_RETRIES}" >> ${ENVFILE}
