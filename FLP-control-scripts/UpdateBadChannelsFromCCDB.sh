#! /bin/bash

CRUMAPS=$(ls -1 ../Mapping/cru-*.map)
NCRUMAPS=$(echo "$CRUMAPS" | wc -l)
OUTFILENAME="ds_ch_exclude.txt"

if [ ${NCRUMAPS} -ne 20 ]; then
    echo "  "
    echo "   "
    RED='\033[0;31m'
    NC='\033[0m' # No Color
    echo -n -e "${RED} Missing some CRU mapping files :  ${NCRUMAPS} seen over 20 ${NC} needed"
    read dummy
    wait
fi

# get the latest BadChannels file from ccdb
mv BadChannels.txt PreviousBadChannels.txt
echo $HOSTNAME
if [[ "${HOSTNAME}" =~ .*flp148.* ]]; then
    o2-mch-bad-channels-ccdb -v -c http://o2-ccdb.internal -q >BadChannels.txt
else
    o2-mch-bad-channels-ccdb -v -c https://alice-ccdb.cern.ch -q >BadChannels.txt
fi

# bad channel list example
#number of bad channels = 1429
#solarid  410 elinkid    3 ch 17
#solarid  410 elinkid    3 ch 18

#fec map example
#392     6       100     3       4       5       6       7
#392     4       100     8       9       10      11      12
#392     2       100     13      14      15      16      17
#392     0       100     18      19      20      46      47
#392     7       100     27      1       2       28      54

#cru map example
#275     2       15      0976    1       d8:0.0  d9:0.0
#276     2       16      0976    1       d8:0.0  d9:0.0
#277     2       17      0976    1       d8:0.0  d9:0.0

# more ds_ch_exclude.txt_old
#0 0 0 1 0  89.016  2.903  5150 0
#CRU     LINK    FEEID[0-40]     SAMPAID[0-1]     CHANNELID[0-31]   PEDESTAL    NOISE   ??      0(?)

if [ -e BadChannels.txt ]; then

    # check if bad channels file is the same as previous, if yes  do nothing
    # note that a simple diff will always give different results since we have four lines
    # related to the ccdb request at the begining of the file, skip them. 
    diff -I "<<<" -I "[INFO]" BadChannels.txt PreviousBadChannels.txt &>/dev/null
    res="$?"
    if [ "$res" -eq 0 ]; then
        echo -n -e "\n"
        echo -n -e "The updated BadChannel.txt file is identical to the previous one \n"
        echo -n -e "----> doing nothing and exiting the script \n"
        echo -n -e "\n"

        exit 1
    fi

    # remove previous file
    if [ -e ${OUTFILENAME} ]; then
        rm -f ${OUTFILENAME}
    fi

    NBadChannels=$(cat BadChannels.txt | grep -c ^solarid)
    echo "Scanning $NBadChannels bad channels, will need a minute or two..."

    #loop first over cru maps (faster)
    for I in $(seq 1 $NCRUMAPS); do
        CRUMAP=$(echo "$CRUMAPS" | sed -n ${I}p)
        #echo "CRUMAP: $CRUMAP"

        #read each CRU map file
        while IFS= read -r line; do

            SID=$(echo "$line" | cut -f 1)
            CRUID=$(echo "$line" | cut -f 2)
            FIBERID=$(echo "$line" | cut -f 3)
            #echo "Searching solar ${SID} link ${FIBERID} from CRU ${CRUID} from $CRUMAP "

            while read line; do
                read -a array <<<"$line"

                #to skip the first lines
                if [ x"${array[0]}" != x"solarid" ]; then continue; fi

                solarid="${array[1]}"
                elinkid="${array[3]}"
                ch=${array[5]}
                sampaid=0
                channelid="${ch}"
                if [ "${ch}" -ge 32 ]; then
                    sampaid=1
                    channelid=$((ch - 32))
                fi

                if [ "${solarid}" = "${SID}" ]; then

                    # echo "  -----> Found CRU ${CRUID}  and fiber ${FIBERID} for  solar ${solarid} from badchannel ccdb file"
                    # dummy values for now ped=512 noise=12 code=1111 (?)  and last value 0 ?
                    echo "${CRUID} ${FIBERID} ${elinkid} ${sampaid} ${channelid} 512 12 1111 0" >> ${OUTFILENAME}

                fi

            done <BadChannels.txt

        done <"$CRUMAP"

    done

    # copy bad channel files ${OUTFILENAME} on each flp
    FLPS="$(cat flplist.txt)"
    for FLP in $FLPS; do
        echo ""
        echo "copying bad channel file to FLP $FLP"
        scp  ${OUTFILENAME} mch@alio2-cr1-flp${FLP}.cern.ch:/home/mch/MCHToolbox/scripts/.
        sleep 0.1
    done

else
    echo "  "
    echo "  "
    RED='\033[0;31m'
    NC='\033[0m' # No Color
    echo -n -e "${RED} The BadChannel.txt file from ccdb extraction was not found ${NC}"
    read dummy
fi
