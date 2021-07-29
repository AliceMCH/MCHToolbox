#! /bin/bash

WAIT=1
if [ $# -ge 1 ]; then
    WAIT=$1
fi

#aliswmod enter Readout/v1.3.10.2.RDH6-1

export INFOLOGGER_MODE=stdout


rm -f /tmp/readout.done

#rm -f /tmp/readout-pipe
#mkfifo /tmp/readout-pipe
#roc-setup-hugetlbfs
echo "o2-readout-exe file://$(pwd)/readout.cfg"
o2-readout-exe file://$(pwd)/readout.cfg #>& readout.log

echo ""
echo "Readout finished."

if [ x"$WAIT" = "x1" ]; then read dummy; fi

touch /tmp/readout.done

if [ x"${CRU_CLKSRC}" == "xlocal" ]; then
    ./stop_daq.sh
fi
