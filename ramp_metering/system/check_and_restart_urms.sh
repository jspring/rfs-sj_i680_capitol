#!/bin/bash

CTR=0

ps -elf |grep -v grep | grep 'wrfiles_ac_rm'

if [[ $? -ne 0 ]]
then
	CTR=1
fi

ps -elf |grep -v grep | grep 'send_urms_data'

if [[ $? -ne 0 ]]
then
	CTR=1
fi

ps -elf |grep -v grep | grep '/home/atsc/urms/lnx/urms'

if [[ $? -ne 0 ]]
then
	CTR=1
fi

ps -elf |grep -v grep | grep '/home/path/db/lnx/db_slv'

if [[ $? -ne 0 ]]
then
	CTR=1
fi

if [[ $CTR -ne 0 ]]
then
	/home/sr99_ramp_metering/system/urmsgo
fi

exit 0
