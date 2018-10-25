#!/bin/bash

if [[ q$1 == 'q' ]]
then
	echo "Usage: $0 <remote ip address>"
	exit 1
fi

IPADDR=$1
#DATESTR=`date +%m%d%y_%H%M%S`
sudo /home/atsc/ab3418/lnx/ab3418comm -a $IPADDR -A 192.168.200.120 -o 162 -v -f "/big/data/$IPADDR.txt" -E
