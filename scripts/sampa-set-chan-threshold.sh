#! /bin/bash

CONFIGFILE=$1
CH=$2
THR=$3

# ZSTHR 0C
echo "$CH 0x09 $THR" >> "${CONFIGFILE}"
