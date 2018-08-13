#!/bin/bash

A_FILE=$1

if [[ q$A_FILE == 'q' ]]
then
	echo
	echo "Usage: $0 <one filename in set>"
	echo "Prints out the number of changes (i.e. number of different lines) in occupancy"
	echo "for queue detectors for all the lanes on all the controllers:"
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
	wc $THIS_PATH | awk '{print $1,$4}'
	cat $THIS_PATH | awk '{print $155,$160,$165,$170,$175,$180,$185,$190,$195,$200,$205,$210,$215,$220,$225,$230}' | uniq -c
done
