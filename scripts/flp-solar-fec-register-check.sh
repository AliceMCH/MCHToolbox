#! /bin/bash

rm -f stop-fec-check

while [ true ]; do 
    if [ -e stop-fec-check ]; then
	break
    fi

    echo ""; echo "----------";
    date
    echo "----------"; echo "";
    ./flp-solar-fec-register-read.sh

    sleep 30
done
