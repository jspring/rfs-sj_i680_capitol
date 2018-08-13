#!/bin/bash

YESTERDAY=`/bin/date -d "yesterday" +%m%d`
cd /big/data/ac_rm_1
INFILES="a$YESTERDAY*"
OUTZIP="a$YESTERDAY.zip"

/usr/bin/zip $OUTZIP $INFILES
/usr/bin/scp $OUTZIP jspring@128.32.234.154:/home/ac_rm_data_test

if [[ $? == 0 ]]
then
	/bin/rm $OUTZIP
fi
