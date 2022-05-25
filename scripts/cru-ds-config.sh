#! /bin/bash

CRU=$1

SCRIPTDIR=$(readlink -f $(dirname $0))
source ${SCRIPTDIR}/env-${CRU}.sh

./cru-get-ds-enabled.sh $CRU

echo "${RUNTYPE}"
SAMPLES=5
if [ x"${RUNTYPE}" = "xpedestals_ul" ]; then
    SAMPLES=2
fi


./cru-send-hard-reset.sh $CRU

./cru-config-trigger.sh $CRU

#bash ./gen_sampa_config_reset.sh

PEDESTALS=0
RETRIES=${I2C_RETRIES}

if [ x"$RUNTYPE" = "xpedestals" -o x"$RUNTYPE" = "xpedestals_ul" -o x"$RUNTYPE" = "xhb_check" -o x"$RUNTYPE" = "xber_check" ]; then

    echo "./gen-sampa-config-ped.sh $CRU $SAMPLES"
    bash ./gen-sampa-config-ped.sh $CRU $SAMPLES
    PEDESTALS=1

elif [ x"$RUNTYPE" = "xphysics_continuous_ul" -o x"$RUNTYPE" = "xphysics_continuous" ]; then

    echo "gen-sampa-config-continuous.sh ${CRU} ${ADC_THR}"
    bash gen-sampa-config-continuous.sh ${CRU} ${ADC_THR}

elif [ x"$RUNTYPE" = "xphysics_continuous_ul_csum" -o x"$RUNTYPE" = "xphysics_continuous_csum" ]; then

    echo "gen-sampa-config-continuous-csum.sh $CRU ${ADC_THR}"
    bash gen-sampa-config-continuous-csum.sh ${CRU} ${ADC_THR}

elif [ x"$RUNTYPE" = "xphysics_triggered" ] || \
        [ x"$RUNTYPE" = "xphysics_triggered_ul" ]; then

    echo "gen-sampa-config-triggered.sh" ${CRU} ${ADC_THR}
    bash gen-sampa-config-triggered.sh ${CRU} ${ADC_THR}

elif [ x"$RUNTYPE" = "xphysics_triggered_csum" ] || \
        [ x"$RUNTYPE" = "xphysics_triggered_ul_csum" ]; then

    echo "gen-sampa-config-triggered-csum.sh ${CRU} ${ADC_THR}"
    bash gen-sampa-config-triggered-csum.sh ${CRU} ${ADC_THR}

fi


NENABLED=0
NDISABLED=0
CFGFILE=/tmp/ds-config-${CRU}.txt
rm -f $CFGFILE
for CRU_LINK in $(seq 0 23); do
    for DS in $(seq 0 39); do
	TEST=$(cat board-enable-${CRU}.txt | grep "${FEEID} ${CRU_LINK} ${DS} 1")
	if [ -z "$TEST" ]; then continue; fi

	#echo "${FEEID} ${CRU_LINK} ${DS} enabled"
	NENABLED=$((NENABLED+1))

	if [ x"$PEDESTALS" != "x1" ]; then
	    cp config_sampa_${CRU}_0.txt /tmp/config_sampa_${CRU_LINK}_${DS}_0.txt
	    cp config_sampa_${CRU}_1.txt /tmp/config_sampa_${CRU_LINK}_${DS}_1.txt
	    echo "${CRU_LINK} ${DS}  /tmp/config_sampa_${CRU_LINK}_${DS}_0.txt /tmp/config_sampa_${CRU_LINK}_${DS}_1.txt" >> $CFGFILE

	    # mute noisy channels
	    CHLIST=""
	    if [ -e ds_ch_exclude.txt ]; then
		CHLIST=$(cat ds_ch_exclude.txt | grep "^${FEEID} ${CRU_LINK} ${DS} ")
	    fi
	    if [ -n "$CHLIST" ]; then
		NCH=$(echo "$CHLIST" | wc -l)
		#echo "$NCH"
		#echo "$CHLIST"
		for N in $(seq 1 $NCH); do
		    LINE=$(echo "$CHLIST" | sed -n ${N}p)
		    CHIP=$(echo "$LINE" | cut -d" " -f 4)
		    CHAN=$(echo "$LINE" | cut -d" " -f 5)
		    PED=$(echo "$LINE" | tr -s " " | cut -d" " -f 6)
		    NOISE=$(echo "$LINE" | tr -s " " | cut -d" " -f 7)
		    #echo "PED: $PED"
		    #echo "NOISE: $NOISE"
		    if (( $(echo "($NOISE > ${NOISE_THR}) || ($PED > ${PED_THR})" | bc -l) )); then
			echo "Switching off channel: $LINE"
			./sampa-set-chan-threshold.sh /tmp/config_sampa_${CRU_LINK}_${DS}_${CHIP}.txt $CHAN 0xFF
			NDISABLED=$((NDISABLED+1))
		    fi
		done
	    fi
	else
	    echo "${CRU_LINK} ${DS} config_sampa_${CRU}_0.txt config_sampa_${CRU}_1.txt" >> $CFGFILE
	fi

	CRU_LINK2=$(echo "scale=0; ${CRU_LINK}%12" | bc -l)
    done
done
echo "$NDISABLED channels disabled"


#(cd ../alice-tpc-fec-utils-mt/build && make -j 8) || exit 1
ERROR=0
for I in $(seq 1 1); do
    #echo "../alice-tpc-fec-utils-mt/build/src/cru/tdsinit "${CRU_PCI_ADDR}" ds_config.txt"
    #time ../alice-tpc-fec-utils-mt/build/src/cru/tdsinit "${CRU_PCI_ADDR}" ds_config.txt >& sampa_load.log
    #echo "../alice-tpc-fec-utils/build/src/cru/tdsinit "${CRU_PCI_ADDR}" ds_config.txt"
    #time ../alice-tpc-fec-utils/build/src/cru/tdsinit "${CRU_PCI_ADDR}" ds_config.txt $RETRIES >& sampa_load.log
    echo "../tools/command-line/build/ds-init \"${CRU_PCI_ADDR1}\" \"${CRU_PCI_ADDR2}\" $CFGFILE $RETRIES"
    time ../tools/command-line/build/ds-config "${CRU_PCI_ADDR1}" "${CRU_PCI_ADDR2}" $CFGFILE $RETRIES >& sampa_load_${CRU}.log
    RET=$?
    if [ x"$RET" = "x0" ]; then
	echo ""; echo "Configuration of $NENABLED sampa boards finished"
    else
	#exit 0
	echo "SAMPA configuration failed, retrying ($I)"; sleep 1
	#cat sampa_load.log | grep "ERROR" | grep "Configuration" | grep "failed"
        cat sampa_load_${CRU}.log | grep "FAILED"
	echo ""; echo "Configuration of $NENABLED sampa boards finished with errors"
	ERROR=1
    fi
done


exit $ERROR
