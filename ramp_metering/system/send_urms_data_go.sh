#!/bin/bash

while [[ 1 ]]
do
	if [[ $1 == "home" ]]
	then
		/home/atsc/urms/lnx/send_urms_data -r 10.0.1.202

# Use the following if sobu5 is used, otherwise, send directly to TSCP
# laptop over 10.x.x.x LAN
#		/home/atsc/urms/lnx/send_urms_data -r 10.0.1.5
	else
		/home/atsc/urms/lnx/send_urms_data
	fi
	sleep 5
done
