#! /bin/bash

./FLPs-run-command.sh ./set-run-type.sh $1
./FLPs-run-command.sh ./flp-init-env.sh
./FLPs-run-command-parallel.sh ./flp-config.sh
