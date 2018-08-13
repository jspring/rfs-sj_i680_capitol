#!/bin/bash

YESTERDAY=`/bin/date -d "yesterday" +%m%d`
cd /big/data/ac_rm_1

for x in a$YESTERDAY*
do
        y=`echo $x | sed '{s/\./ /g}' | awk '{print $1}'`
        /usr/bin/zip $y.zip $x
        z=0
        while [[ z -eq 0 ]]
        do
                /usr/bin/scp $y.zip jspring@128.32.234.154:/home/ac_rm_data
                if [[ $? == 0 ]]
                then
                        z=1
                else
                        sleep 1
                fi
        done
done
