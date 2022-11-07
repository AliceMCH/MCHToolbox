#! /bin/bash

CRU=$1

source env-${CRU}.sh

FORCE_CONFIG="--force-config --bypass-fw-check --clock=${CRU_CLKSRC} --datapathmode=STREAMING --gbtmux=ttc --downstreamdata=pattern"

echo "roc-config ${FORCE_CONFIG} --id=${CRU_SN}:0 --loopback"
roc-config ${FORCE_CONFIG} --id=${CRU_SN}:0 --loopback

echo "roc-config ${FORCE_CONFIG} --id=${CRU_SN}:1 --loopback"
roc-config ${FORCE_CONFIG} --id=${CRU_SN}:1 --loopback
