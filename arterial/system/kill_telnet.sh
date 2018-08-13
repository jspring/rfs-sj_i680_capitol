#!/bin/bash

for x in `ps -elf|grep telnet|awk '{print $4}'`
do
	kill -s TERM $x
done
