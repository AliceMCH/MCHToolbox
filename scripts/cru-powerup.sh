#! /bin/bash

CRU=$1

if [ cru.map -nt env-${CRU}.sh ]; then ./cru-init-env.sh $CRU; fi

source env-${CRU}.sh

# enable SAMPA cluster sum by default at power-up
export CRU_CSUM_ENABLED=1
bash ./cru-set-feeid.sh $CRU

# enable all links
bash ./cru-ul-links-enable.sh 1
