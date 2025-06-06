#! /bin/bash

CRU=$1
THR=$2

for SID in "0" "1"; do

CONFIGFILE="config_sampa_${CRU}_${SID}.txt"
rm -f "${CONFIGFILE}"

#software reset at the beginning
echo "-1 0x0e 0x05" >> "${CONFIGFILE}"
# continuous mode  + power save + cluster sum # VACFG[7:0] = 100101
echo "-1 0x0d 0x25" >> "${CONFIGFILE}" 
# disable 100 ohm and only 1 link
if [ x"${SID}" = "x0" ]; then
    echo "-1 0x12 0x31" >> "${CONFIGFILE}"
else
    echo "-1 0x12 0x11" >> "${CONFIGFILE}"
fi

# TWLEN 63
echo "-1 0x07 0x63" >> "${CONFIGFILE}"
echo "-1 0x08 0x00" >> "${CONFIGFILE}"
# ACQEND 62
echo "-1 0x0b 0x63" >> "${CONFIGFILE}"
echo "-1 0x0c 0x00" >> "${CONFIGFILE}"

echo "-1 0x13 0" >> "${CONFIGFILE}" # set output power to norm
#echo "-1 0x13 0x55" >> "${CONFIGFILE}" # set output power to low
#echo "-1 0x13 0xaa" >> "${CONFIGFILE}" # set output power to max

echo "-1 0x24 0xff" >> "${CONFIGFILE}"
echo "-1 0x25 0xff" >> "${CONFIGFILE}"
echo "-1 0x26 0xff" >> "${CONFIGFILE}"
echo "-1 0x27 0xff" >> "${CONFIGFILE}"

# DPCFG:
# BC1: 0x0000
# BC2: 0x0100
# BC3: 0x0200
if [ x"${BC_MODE}" = "x1" ]; then
    DPCFG="0x0000"
    # Set initial values of the pedestals to the maximum
    for ch in $(seq 0 31); do
	echo "$ch 0x0C 0x3FF" >> "${CONFIGFILE}"
    done
elif [ x"${BC_MODE}" = "x2" ]; then
    DPCFG="0x0100"
elif [ x"${BC_MODE}" = "x23" ]; then
    DPCFG="0x0300"
elif [ x"${BC_MODE}" = "x32" ]; then
    DPCFG="0x0300"
else
    DPCFG="0x0200"
fi
for ch in $(seq 0 31); do
  echo "$ch 0x18 ${DPCFG}" >> "${CONFIGFILE}"
done

# ZSCFG 7F  (7F -> pulses > 2, 7D -> pulses > 1)
if [ x"${BC_MODE}" = "x32" ]; then
  for ch in $(seq 0 31); do
    # bit 7 set to 1 => change position of BC3 in pipeline (BC3 after BC2)
    echo "$ch 0x0b 0x00FF" >> "${CONFIGFILE}"
  done
else
  for ch in $(seq 0 31); do
    echo "$ch 0x0b 0x007F" >> "${CONFIGFILE}"
  done
fi

# ZSTHR 0C
for ch in $(seq 0 31); do
  echo "$ch 0x09 0x00${THR}" >> "${CONFIGFILE}"
  #echo "$ch 0x09 0x0008" >> "${CONFIGFILE}"
done

#echo "-1 0x0e 0x05" >> "${CONFIGFILE}"

done
