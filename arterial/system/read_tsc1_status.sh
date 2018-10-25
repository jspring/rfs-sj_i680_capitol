#!/bin/bash

if [[ q$1 == 'q' ]]
then
	echo "Usage: $0 <remote ip address>"
	exit 1
fi
IPADDR=$1
DATESTR=`date +%m%d%y_%H%M%S`
LATESTFILE=/linux2/big/data/SANJOSE_ACRM/$IPADDR.$DATESTR.txt
ssh -p 5571 jspring@localhost "/home/atsc/ab3418/lnx/ab3418comm -a $IPADDR -A 192.168.200.120 -o 162 -v" | grep "udp response" >$LATESTFILE

echo $LATESTFILE >"/home/sj_i680_capitol/arterial/data/"$IPADDR"_latest.txt"
