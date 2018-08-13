#!/bin/bash

A_FILE=$1

echo
echo "Usage: $0 <one filename in set>"
echo "Prints out the number of changes (i.e. number of different lines) in occupancy"
echo "for queue detectors for all the lanes on all the controllers:"
echo "Column 1: number of different lines in the data file"
echo "Column 2: total number of lines in data file"
echo "Column 3: file name"
echo

if [[ q$A_FILE == 'q' ]]
then
        A_FILE=`ls -t /big/data/ac_rm_1/a_10* | head  -1`
fi

MATCHSTR=`echo $A_FILE | sed '{s/\./ /g}' | awk '{print $(NF-1)}'`
MYPATH=`echo $A_FILE | sed '{s/a_10/ &/g}'| awk '{print $1}'`

echo $MATCHSTR

for x in `cat /home/sr99_ramp_metering/system/control_ips_and_ports.txt` 
do
        THIS_PATH=`ls $MYPATH"a_"$x.$MATCHSTR".dat"`
	echo `cat $THIS_PATH | awk '{print $154,$159,$164,$169,$174,$179,$184,$189,$194,$199,$204,$209,$214,$219,$224,$229}' | uniq -c | wc | awk '{print $1}'` `wc $THIS_PATH | awk '{print $1,$4}'` 
done
