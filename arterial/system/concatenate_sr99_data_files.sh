#!/bin/bash

TARDATE=$1

if [[ q$TARDATE == 'q' ]]
then
	echo "Usage: $0 <date, in YYMMDD format>"
	exit 1
fi

#Check for sequential tarfiles
cd /big/data/ac_rm_1

echo "Here is the list of tarfiles that will be concatenated, IN THE ORDER OF CONCATENATION.
If this is not correct, hit ctrl-C or answer no:
Continue? (y, n)"
for TARFILE in ramp_coord_data_16$TARDATE*.tgz
do
	echo $TARFILE
done
read answer
echo $answer | grep 'y'
if [[ $? -eq 0 ]] 
then 
	echo "Continuing"
else 
	echo $answer | grep 'n'
	if [[ $? -eq 0 ]] 
	then
		echo "Stopping"
		exit
	else
		echo "You don't know what you want!"
		exit
	fi
fi

DIRINDEX=1
for TARFILE in ramp_coord_data_16$TARDATE*.tgz
do 
	DIRNAME[DIRINDEX]=`echo $TARFILE | sed '{s/\./ /g}' | awk '{print $1}'`
	mkdir ${DIRNAME[DIRINDEX]}
	cp $TARFILE ${DIRNAME[DIRINDEX]}
	echo TARFILE $TARFILE DIRNAME ${DIRNAME[DIRINDEX]}
	cd ${DIRNAME[DIRINDEX]}
	tar xvf $TARFILE

	INDEX=0
	for x in `cat /home/sr99_ramp_metering/system/sr99_controller_ip_and_port_list.txt`
	do
		OUTFILE=/big/data/ac_rm_1/${DIRNAME[DIRINDEX]}/"a_"$x".9999_"$TARDATE"_9999.dat"
		if [[ -e $OUTFILE ]]
		then
			echo $OUTFILE already exists. Either delete it or use it.
			exit 1
		fi
		for y in `ls /big/data/ac_rm_1/${DIRNAME[DIRINDEX]}/*"$x"*"$TARDATE"*".dat"`
		do
			echo $y
			FILENAME[INDEX]=/big/data/ac_rm_1/${DIRNAME[DIRINDEX]}/"a_"$x".9999_"$TARDATE"_9999.dat"
			cat $y >>${FILENAME[INDEX]}
		done
		STARTTIME[$INDEX]=`head -1 ${FILENAME[INDEX]} | awk '{print $1}' | sed '{s/:/ /g}' |sed '{s/\./ /1}'|awk '{print ($2*60) + $3}'`
		if [[ ${STARTTIME[$INDEX]} -gt $LASTSTARTTIME ]]
		then
			LASTSTARTTIME=${STARTTIME[$INDEX]}
		fi
		INDEX=$(($INDEX+1))
	done 
	cd /big/data/ac_rm_1
	DIRINDEX=$(($DIRINDEX+1))
done

cd /big/data/ac_rm_1
TARGETDIR="final_"$TARDATE"_9999"
mkdir $TARGETDIR
for x in ${DIRNAME[*]}
do 
	echo $x
	cd $x
	for y in *9999.dat
	do 
		cat $y >>../$TARGETDIR/$y
	done
	cd /big/data/ac_rm_1
done

#the following is from the original concatenation script. It selects the latest starting
#time from all the data files and deletes leading lines in all the other data files to
#synchronize all the data. But there's an implicit problem: what if a data file has all
#zeroes, i.e. it's empty?! I need to fix that!!
#INDEX=0
#LASTSTARTTIME=0
#for x in `cat /home/sr99_ramp_metering/system/sr99_controller_ip_and_port_list.txt`
#do
#	OUTFILE=/big/data/ac_rm_1/"a_"$x".9999_"$DATE"_9999.dat"
#	if [[ -e $OUTFILE ]]
#	then
#		echo $OUTFILE already exists. Either delete it or use it.
#		exit 1
#	fi
#	for y in `ls /big/data/ac_rm_1/*"$x"*"$DATE"*".dat"`
#	do
#		echo $y
#		FILENAME[INDEX]=/big/data/ac_rm_1/"a_"$x".9999_"$DATE"_9999.dat"
#		cat $y >>${FILENAME[INDEX]}
#	done
#	STARTTIME[$INDEX]=`head -1 ${FILENAME[INDEX]} | awk '{print $1}' | sed '{s/:/ /g}' |sed '{s/\./ /1}'|awk '{print ($2*60) + $3}'`
#	if [[ ${STARTTIME[$INDEX]} -gt $LASTSTARTTIME ]]
#	then
#		LASTSTARTTIME=${STARTTIME[$INDEX]}
#	fi
#	INDEX=$(($INDEX+1))
#done 
#
#for((z=0; z<28;z++))
#do
#	DELLINES=$((1+(($LASTSTARTTIME-${STARTTIME[$z]})/15)))
#	more +$DELLINES ${FILENAME[$z]} >${FILENAME[$z]}.mod
#	mv ${FILENAME[$z]}.mod ${FILENAME[$z]}
#done
