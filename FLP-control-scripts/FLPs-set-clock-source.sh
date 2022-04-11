#! /bin/bash

./FLPs-run-command.sh ./set-clock-source.sh $1
./FLPs-run-command.sh ./flp-init-env.sh
./FLPs-run-command-parallel.sh ./flp-config.sh -f
