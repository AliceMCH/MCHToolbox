#! /bin/bash

#SCRIPT_DIR=$(dirname $0)
#source ${SCRIPT_DIR}/env.sh
SCRIPT_DIR=$(pwd)

FLPMAP=flp.map

CHECK=0
if [ "$1" = "-c" ]; then
    CHECK=1
    shift
fi

#xterm -bg black -fg white -geometry 100x15+0+0 -e watch -n 0.9 ${SCRIPT_DIR}/check-packets-loss.sh  &

rm -f /tmp/readout.done
rm -f "$HOME/Data/data-flp.raw"
cat readout-file-full.cfg > readout.cfg
xterm -bg black -fg white -geometry 100x50+0+200 -e ./start-readout.sh 0 &

sleep 1
while [ ! -e "$HOME/Data/data-flp.raw" ]; do
    sleep 1
done

sleep 2


#watch -n 1 ls -lht ${SCRIPT_DIR}/*.raw
while [ ! -e /tmp/readout.done ]; do
    sleep 1
    clear
    ls -lht $HOME/Data/data-flp.raw | head -n 10
done


if [ x"$CHECK" = "x1" ]; then
    ./flp-check-data.sh
fi
