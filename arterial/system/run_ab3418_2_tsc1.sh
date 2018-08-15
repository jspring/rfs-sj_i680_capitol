#!/bin/bash

IPADDR=$1
DATESTR=`date +%m%d%y_%H%M%S`
ssh -p 5571 jspring@localhost "/home/atsc/ab3418/lnx/ab3418comm -a $IPADDR -A 10.192.11.93 -o 2011 -P 20 -T 2111 -v" | grep "udp response" >$IPADDR.$DATESTR.txt
