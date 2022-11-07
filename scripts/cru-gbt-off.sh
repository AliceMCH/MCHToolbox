#! /bin/bash

CRU=$1

source env-${CRU}.sh

FORCE_CONFIG="--force-config --bypass-fw-check --links 0-11 --clock local --datapathmode packet --gbtmux ttc"

echo "roc-config ${FORCE_CONFIG} --id=${CRU_SN}:0 --loopback"
roc-config ${FORCE_CONFIG} --id=${CRU_SN}:0 --loopback

echo "roc-config ${FORCE_CONFIG} --id=${CRU_SN}:1 --loopback"
roc-config ${FORCE_CONFIG} --id=${CRU_SN}:1 --loopback
