#! /bin/bash

for i in 3b 3c af b0 d8 d9 ; do
	echo 1 > /sys/bus/pci/devices/0000\:$i\:00.0/remove
done
echo 1 > /sys/bus/pci/rescan
