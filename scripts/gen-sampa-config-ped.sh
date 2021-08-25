#! /bin/bash

#SAMPLES=2
CRU=$1
SAMPLES=$2

SAMPLESX=$(echo "obase=16; $((SAMPLES-1))" | bc)

P1=0x${SAMPLESX}
P2=0x00

export CONFIGFILE="config_sampa_${CRU}_0.txt"
rm -f "${CONFIGFILE}"

echo "-1 0x0e 0x05" >> "${CONFIGFILE}"
echo "-1 0x0d 0x20" >> "${CONFIGFILE}"
echo "-1 0x12 0x31" >> "${CONFIGFILE}"
#echo "-1 0x07 0x09" >> "${CONFIGFILE}"
echo "-1 0x07 $P1" >> "${CONFIGFILE}"
echo "-1 0x08 $P2" >> "${CONFIGFILE}"
echo "-1 0x13 0" >> "${CONFIGFILE}" # set output power to norm
#echo "-1 0x13 0x55" >> "${CONFIGFILE}" # set output power to low
#echo "-1 0x13 0xaa" >> "${CONFIGFILE}" # set output power to max
echo "-1 0x24 0xff" >> "${CONFIGFILE}"
echo "-1 0x25 0xff" >> "${CONFIGFILE}"
echo "-1 0x26 0xff" >> "${CONFIGFILE}"
echo "-1 0x27 0xff" >> "${CONFIGFILE}"
echo "-1 0x0e 0x05" >> "${CONFIGFILE}"

export CONFIGFILE="config_sampa_${CRU}_1.txt"
rm -f "${CONFIGFILE}"

echo "-1 0x0e 0x05" >> "${CONFIGFILE}"
echo "-1 0x0d 0x20" >> "${CONFIGFILE}"
echo "-1 0x12 0x11" >> "${CONFIGFILE}"
#echo "-1 0x07 0x09" >> "${CONFIGFILE}"
echo "-1 0x07 $P1" >> "${CONFIGFILE}"
echo "-1 0x08 $P2" >> "${CONFIGFILE}"
echo "-1 0x13 0" >> "${CONFIGFILE}" # set output power to norm
#echo "-1 0x13 0x55" >> "${CONFIGFILE}" # set output power to low
#echo "-1 0x13 0xaa" >> "${CONFIGFILE}" # set output power to max
echo "-1 0x24 0xff" >> "${CONFIGFILE}"
echo "-1 0x25 0xff" >> "${CONFIGFILE}"
echo "-1 0x26 0xff" >> "${CONFIGFILE}"
echo "-1 0x27 0xff" >> "${CONFIGFILE}"
echo "-1 0x0e 0x05" >> "${CONFIGFILE}"
echo "${CONFIGFILE}"
