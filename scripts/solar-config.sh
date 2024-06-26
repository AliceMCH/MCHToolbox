#! /bin/bash

CRU=$1
LINKID=$2
REGFILE=$3

SCRIPTDIR=$(readlink -f $(dirname $0))
source ${SCRIPTDIR}/env-${CRU}.sh


#PATT1=10101
#PATT2=00101
#PATT3=00000
#PATT4=00000
#PATT5=00000
#PATT6=00000
#PATT7=00000
#PATT8=00000
#PATT="$PATT8""$PATT7""$PATT6""$PATT5""$PATT4""$PATT3""$PATT2""$PATT1"

PATT="1111111111111111111111111111111111111111"
PATT0="0000000000000000000000000000000000000000"
#PATT=00000111

#echo "PATT: $PATT"

if [ -e board-enable-${CRU}.txt ]; then
    PATT="$PATT0"
    NL=$(cat board-enable-${CRU}.txt | wc -l)
    I=1  
    while [ $I -le $NL ]; do
	LINE=$(sed -n ${I}p board-enable-${CRU}.txt)
	_LINK_ID=$(echo "$LINE" | cut -d" " -f 2)
	if [ x"${_LINK_ID}" = x"${LINKID}" ]; then
	    _DS_ADDR=$(echo "$LINE" | cut -d" " -f 3)
	    _ENABLED=$(echo "$LINE" | cut -d" " -f 4)
	    if [ x"${_ENABLED}" = "x1" ]; then
		ID1=$((39-${_DS_ADDR}))
		ID2=$((39-${_DS_ADDR}+1))
		ID3=$((39-${_DS_ADDR}+2))
		PREFIX=""
		SUFFIX=""
		if [ $ID1 -gt 0 ]; then PREFIX=$(echo "$PATT" | cut -c 1-$ID1); fi
		if [ $ID3 -le 40 ]; then SUFFIX=$(echo "$PATT" | cut -c $ID3-40); fi
		#echo "L ${LINKID} DS ${_DS_ADDR} enabled - IDS: $ID1 $ID2 $ID3 - prefix=\"$PREFIX\" suffix=\"$SUFFIX\""
		PATT="${PREFIX}1${SUFFIX}"
	    fi
	fi
	I=$((I+1))
    done
fi

echo -n "J1: "; echo -n "$PATT" | cut -c 36-40
echo -n "J2: "; echo -n "$PATT" | cut -c 31-35
echo -n "J3: "; echo -n "$PATT" | cut -c 26-30
echo -n "J4: "; echo -n "$PATT" | cut -c 21-25
echo -n "J5: "; echo -n "$PATT" | cut -c 16-20
echo -n "J6: "; echo -n "$PATT" | cut -c 11-15
echo -n "J7: "; echo -n "$PATT" | cut -c 6-10
echo -n "J8: "; echo -n "$PATT" | cut -c 1-5
#exit


#echo "../GBT_IC/build/gbt-ic ${CRU_PCI_ADDR} w ${LINKID} 35 $(( 16#42 ))"
#../GBT_IC/build/gbt-ic ${CRU_PCI_ADDR} "w" ${LINKID} 35 $(( 16#42 ))
#echo "${LINKID} 35 $(( 16#42 ))" >> "$REGFILE"



if [ -e solar-modifiedChargepumpRegister-${CRU}.txt ]; then

	while read line;do
		read -a array <<< "$line"
         	if  [ x"${LINKID}" = x"${array[0]}" ]  ; then 
			echo "Setting  custom config  (${array[1]}) on solar  link ${LINKID} " 
			echo "${LINKID} 35 $(( 16#${array[1]} ))" >> "$REGFILE"
		fi 

	done < solar-modifiedChargepumpRegister-${CRU}.txt
else 
	echo "${LINKID} 35 $(( 16#42 ))" >> "$REGFILE"
fi 


# clock enable
offset=0
for iter in $(seq 1 5); do
    P=$(echo -n "$PATT" | tail -c $(($iter*8)) | head -c 8)
    INPATTREG=$(echo "ibase=2; $P" | bc)
    for reg in 255 333 348; do
	R=$((reg+offset))
	#../GBT_IC/build/gbt-ic ${CRU_PCI_ADDR} "w" ${LINKID} $R 255
	echo "${LINKID} $R $INPATTREG" >> "$REGFILE"
	#echo "${LINKID} $R 255" >> "$REGFILE"
    done
    offset=$((offset+3))
done


# input enable
offset=0
for iter in $(seq 1 5); do
    P=$(echo -n "$PATT" | tail -c $(($iter*8)) | head -c 8)
    INPATTREG=$(echo "ibase=2; $P" | bc)
    #INPATTREG=255
    #echo "I $iter  P: $P"
    #echo "INPATTREG: $INPATTREG"
    for reg in 81 82 83; do
	R=$((reg+offset))
	#../GBT_IC/build/gbt-ic ${CRU_PCI_ADDR} "w" ${LINKID} $R 255
	echo "${LINKID} $R $INPATTREG" >> "$REGFILE"
	#echo "${LINKID} $R 255" >> "$REGFILE"
	#echo "$R $INPATTREG"
    done
    offset=$((offset+24))
done


# signal output enable
offset=0
for iter in $(seq 1 5); do
    for reg in 256 334 349; do
	R=$((reg+offset))
	#../GBT_IC/build/gbt-ic ${CRU_PCI_ADDR} "w" ${LINKID} $R 255
	echo "${LINKID} $R 255" >> "$REGFILE"
    done
    offset=$((offset+3))
done


#for reg in $(seq 320 326); do
#    python gbtx_set_reg.py 3b:0.0 ${GBT_CH} $reg 0
#done


# output mode and clock frequency
for reg in 332 335 338 341 344 347 350 353 356 359; do
    #../GBT_IC/build/gbt-ic ${CRU_PCI_ADDR} "w" ${LINKID} $reg 5
    echo "${LINKID} $reg 5" >> "$REGFILE"
done


# set output power to maximum
for reg in 327 328 329 330 331; do
    echo "${LINKID} $reg 0" >> "$REGFILE"
    #echo "$reg 170" >> "$REGFILE"
done


# e-ports DLL configuration
for reg in 64 88 112 136 160 184 208; do
    echo "${LINKID} $reg $(( 16#BB ))" >> "$REGFILE"
    echo "${LINKID} $((reg+1)) $(( 16#B ))" >> "$REGFILE"
done

for reg in 84 85 86 108 109 110 132 133 134 156 157 158 180 181 182 204 205 206 228 229 230; do
    echo "${LINKID} $reg 255" >> "$REGFILE"
done

CLOCK_PHASE_FILE="GBT Clock Phases/solar-clock-phase-${CRU}-${LINKID}.txt"
if [ -e "${CLOCK_PHASE_FILE}" ]; then
    echo "Reading clock phases from \"${CLOCK_PHASE_FILE}\""

    # set the phase tracking mode to static
    echo "${LINKID} 62 0" >> "$REGFILE"

    # Phase-Aligners DLL reset
    #for reg in 65 89 113 137 161 185 209; do
    #echo "$reg $(( 16#7B ))" >> "$REGFILE"
    #done
    #for reg in 65 89 113 137 161 185 209; do
    #echo "$reg $(( 16#B ))" >> "$REGFILE"
    #done

    I=1
    for reg in 69 68 67 66 93 92 91 90 117 116 115 114 141 140 139 138 165 164 163 162; do
    #for reg in 69 68 67 66 93 92 91 90 117 116; do
	PHASE1=$(cat "${CLOCK_PHASE_FILE}" | cut -d" " -f $I)
	PHASE2=$(cat "${CLOCK_PHASE_FILE}" | cut -d" " -f $((I+1)))
	VAL=$(echo "ibase=16; ${PHASE2}${PHASE1}" | bc)
	echo "${LINKID} $reg $VAL" >> "$REGFILE"
	echo "${LINKID} $((reg+4)) $VAL" >> "$REGFILE"
	echo "${LINKID} $((reg+8)) $VAL" >> "$REGFILE"

	I=$((I+2))
    done
else
    echo "Clock phases not found, starting automatic phase alignment"

    echo "${LINKID} 62 42" >> "$REGFILE"

    # Phase-Aligners DLL reset
    #for reg in 65 89 113 137 161 185 209; do
    #echo "${LINKID} $reg $(( 16#70 ))" >> "$REGFILE"
    #done
    #for reg in 65 89 113 137 161 185 209; do
    #echo "${LINKID} $reg $(( 16#B ))" >> "$REGFILE"
    #done

    for reg in 84 85 86 108 109 110 132 133 134 156 157 158 180 181 182 204 205 206 228 229 230; do
	echo "${LINKID} $reg 0" >> "$REGFILE"
    done
fi

