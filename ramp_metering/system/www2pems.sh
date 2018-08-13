#!/bin/bash
retval=1
count1=0
count2=0
while [[ $retval != 0 ]]
do 
	ping -W 3 -c 1 128.32.234.154
	retval=$?
	if [[ $retval == 1 ]]
	then
		count1=$(($count1+1))
		seconds1=$(($count1*3))
	fi
	if [[ $retval == 2 ]]
	then
		count2=$(($count2+1))
		seconds2=$(($count2))
	fi
	echo `date` retval $retval count1 $count1 seconds1 $seconds1 count2 $count2 seconds2 $seconds2 >>/var/log/autossh.www.err
	sleep 1
done

while [[ 1 ]]
do
	/usr/bin/autossh -M 7 -R 5559:localhost:80 jspring@128.32.234.154 >>/var/log/autossh.www.log 2>>/var/log/autossh.www.err
	retval=$?
	if [[ $retval != 0 ]]
	then
		echo `date` retval $retval Retrying ssh connection to PEMS server....>>/var/log/autossh.www.err
	else
		exit 0
	fi
	sleep 1
done
