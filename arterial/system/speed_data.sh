#!/bin/bash

A_FILE=$1

if [[ q$A_FILE == 'q' ]]
then
	echo
	echo "Usage: $0 <one filename in set>"
	echo "Prints out the number of changes (i.e. number of different lines) in flow"
	echo "for demand and passage detectors for all the lanes on all the controllers:"
	echo "Column 1: number of different lines in the data file"
	echo "Column 2: total number of lines in data file"
	echo "Column 3: file name"
	echo

	exit 1
fi

MATCHSTR=`echo $A_FILE | sed '{s/\./ /g}' | awk '{print $(NF-1)}'`
MYPATH=`echo $A_FILE | sed '{s/a_10/ &/g}'| awk '{print $1}'`

echo $MATCHSTR

for THIS_PATH in `ls $MYPATH*$MATCHSTR.dat`
do
	echo `cat $THIS_PATH | awk '{print $8,$15,$22,$29,$36,$43,$238,$245}' | uniq -c | wc | awk '{print $1}'` `wc $THIS_PATH | awk '{print $1,$4}'` 
done
