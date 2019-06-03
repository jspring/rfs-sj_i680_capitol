#!/bin/bash

DATETIME=`date +%m%d%Y_%H%M%S`
ssh -p 5571 jspring@localhost "/home/atsc/ab3418/lnx/ab3418comm -A 192.168.200.120 -f blah -o 162 -P  9 -a 10.192.131.7  -v " >>/linux2/big/data/SANJOSE_ACRM/mckee_on_$DATETIME.txt &
ssh -p 5571 jspring@localhost "/home/atsc/ab3418/lnx/ab3418comm -A 192.168.200.120 -f blah -o 162 -P  9 -a 10.192.131.6  -v " >>/linux2/big/data/SANJOSE_ACRM/mckee_on_$DATETIME.txt &
ssh -p 5571 jspring@localhost "/home/atsc/ab3418/lnx/ab3418comm -A 192.168.200.120 -f blah -o 162 -P  9 -a 10.192.131.1  -v " >>/linux2/big/data/SANJOSE_ACRM/mckee_on_$DATETIME.txt &
