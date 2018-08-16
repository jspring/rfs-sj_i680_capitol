#!/bin/bash

IPADDR=$1
DATESTR=`date +%m%d%y_%H%M%S`
LATESTFILE=/linux2/big/data/SANJOSE_ACRM/$IPADDR.$DATESTR.txt
ssh -p 5571 jspring@localhost "/home/atsc/ab3418/lnx/ab3418comm -a $IPADDR -A 10.192.131.150 -o 2011 -P 20 -T 2111 -v" | grep "udp response" >$LATESTFILE

echo $LATESTFILE >"/home/sj_i680_capitol/arterial/data/"$IPADDR"_latest.txt"
