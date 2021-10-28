#! /bin/bash

TYPE=$1

if [ x"$TYPE" = "x" ]; then
    echo "Usage: ./set-run-type.sh RUNTYPE"
    echo ""
    echo "RUNTYPE can take the following values:"
    echo "0: pedestals"
    echo "1: pedestals_ul"
    echo "2: physics_triggered"
    echo "3: physics_triggered_csum"
    echo "4: physics_triggered_ul"
    echo "5: physics_triggered_ul_csum"
    echo "6: physics_continuous_ul"
    echo "7: physics_continuous_ul_csum"
    echo "8: physics_continuous"
    echo "9: physics_continuous_csum"
    echo "10: hb_check"
    echo "11: ber_check"
    exit 1
fi

case $TYPE in
    0) RUNTYPE=pedestals;;
    1) RUNTYPE=pedestals_ul;;
    2) RUNTYPE=physics_triggered;;
    3) RUNTYPE=physics_triggered_csum;;
    4) RUNTYPE=physics_triggered_ul;;
    5) RUNTYPE=physics_triggered_ul_csum;;
    6) RUNTYPE=physics_continuous_ul;;
    7) RUNTYPE=physics_continuous_ul_csum;;
    8) RUNTYPE=physics_continuous;;
    9) RUNTYPE=physics_continuous_csum;;
    10) RUNTYPE=hb_check;;
    11) RUNTYPE=ber_check;;
esac

echo "RUNTYPE=$RUNTYPE"
echo "RUNTYPE=$RUNTYPE" > runtype.sh
