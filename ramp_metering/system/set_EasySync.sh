#!/bin/bash

for x in /dev/EasySync? ; do
	/bin/stty -F $x 38400 raw
done
echo `date` >~jspring/bin/datestamp.txt
