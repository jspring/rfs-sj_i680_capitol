#!/bin/bash

CTR=0

ps -elf |grep -v grep | grep 'wrfiles_ac_rm'

if [[ $? -ne 0 ]]
then
	CTR=1
fi

ps -elf |grep -v grep | grep '/home/atsc/ab3418/lnx/receive_urms_data'

if [[ $? -ne 0 ]]
then
	CTR=2
fi

ps -elf |grep -v grep | grep '/home/sr99_ramp_metering/system/receive_urms_data_go.sh'

if [[ $? -ne 0 ]]
then
	CTR=3
fi

ps -elf |grep -v grep | grep '/home/atsc/ab3418/lnx/ab3418comm'

if [[ $? -ne 0 ]]
then
	CTR=4
fi

ps -elf |grep -v grep | grep '/home/path/db/lnx/db_slv'

if [[ $? -ne 0 ]]
then
	CTR=5
fi

if [[ $CTR -ne 0 ]]
then
	echo CTR $CTR
	/home/sr99_ramp_metering/system/urmsgo
	exit 1
fi

exit 0
