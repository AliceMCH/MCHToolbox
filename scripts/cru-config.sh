#! /bin/bash

FORCE_CONFIG=""
if [ x"$1" = "x-f" ]; then FORCE_CONFIG="--force-config"; shift; fi

CRU=$1

if [ cru.map -nt env-${CRU}.sh ]; then ./cru-init-env.sh $CRU; fi

source env-${CRU}.sh

CRUCONF=$(./cru-config-gen.sh $CRU)
CRUCONF1=$(echo "$CRUCONF" | sed -n 1p)
CRUCONF2=$(echo "$CRUCONF" | sed -n 2p)

echo "roc-config ${FORCE_CONFIG} $CRUCONF1"
roc-config ${FORCE_CONFIG} $CRUCONF1

echo "roc-config ${FORCE_CONFIG} $CRUCONF2"
roc-config ${FORCE_CONFIG} $CRUCONF2



# EP 0
roc-reg-read --id=${CRU_PCI_ADDR} --channel=2 --address=0x600034
# EP 1
roc-reg-read --id=${CRU_PCI_ADDR} --channel=2 --address=0x700034

if [ x"${CRU_UL_ENABLED}" = "x1" ]; then
    bash ./cru-set-feeid.sh $CRU
fi

LINKS_PATT="0x00000000"
#LINKS_PATT="0x0FFF0FFF"
echo "Setting links pattern for CRU $CRU to ${LINKS_PATT}"
echo "roc-reg-write --id=${CRU_PCI_ADDR} --channel=2 --address=0xc80008 --value=\"${LINKS_PATT}\""
roc-reg-write --id=${CRU_PCI_ADDR} --channel=2 --address=0xc80008 --value="${LINKS_PATT}"


# set pat-player in default mode
#echo "roc-pat-player --id=${CRU_PCI_ADDR} --idle=0x0C0300C0300C0300C030 --sync=0x3C0F03C0F03C0F03C0F0 --sync-length=1 --sync-delay=0 --sync-trigger-select=4"
roc-pat-player --id=${CRU_PCI_ADDR} --idle=0x0C0300C0300C0300C030 \
    --sync=0x3C0F03C0F03C0F03C0F0 \
    --sync-length=1 --sync-delay=0 --sync-trigger-select=4

echo ""; echo "Configuration of CRU $CRU completed."
