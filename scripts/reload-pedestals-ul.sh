#! /bin/bash

SCRIPTDIR=$(readlink -f $(dirname $0))

cd $SCRIPTDIR && (./set-run-type.sh 1; ./flp-init-env.sh; ./flp-config-all.sh)
