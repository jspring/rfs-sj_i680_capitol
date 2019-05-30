#!/bin/bash

sudo killall ab3418comm
sleep 5
DBNUM=4000
for IP in `cat /home/sj_i680_capitol/arterial/doc/Arterial_signal_IP_addresses.txt |grep "10\." | awk '{print $2}'`
do
#	DBNUM=`echo $IPDB | awk '{print $2}'`
	DATETIME=`date +%d%m%Y_%H%M%S`
	/home/atsc/ab3418/lnx/ab3418comm -R -f "/linux2/big/data/SANJOSE_ACRM/arterial_data/"$IP".txt" -D $DBNUM -u -i 10000 >"/linux2/big/data/SANJOSE_ACRM/arterial_data/data_"$IP"_"$DATETIME".txt" &
	
echo "IP $IP DBNUM $DBNUM"
	DBNUM=$(($DBNUM+100))
done
