#! /bin/bash

# usage: A4_linux_donut_loop.sh  number_of_runs_integer
# so from the shell prompt:  $ A4_linux_donut_loop.sh 10 
# 10 times and report how many of the runs ended in deadlock

if [ -z $1 ]
then
	echo "USAGE: A4_linux_donut_loop.sh  number_of_runs_integer"
	echo "Try the command again"
	echo " "
fi

if [ -z $2 ] 
then 
	pname=procscope
else
	pname=threadscope
fi
		
	

echo " "
echo "The configuration is for $1 LOOPS"
echo " "

local1=$1
lpcnt=1
count=0
pname=procscope
while test $local1 -ne 0
do
	echo " "
	echo "Working on LOOP $lpcnt"
	echo " "
	./$pname &
	pid=`ps | grep $pname | cut -c1-6`
	echo JOB AND PID IS $pid
	sleep 3
	pid=`ps | grep $pname | cut -c1-6`
	if [ -n "$pid" ]
	then
		echo PID REMAINING IS $pid
		echo " "
		echo "DEADLOCK DETECTED ON LOOP $lpcnt"
		sleep 1
		echo " "
		count=`expr $count + 1`
		echo "#### KILLING ALL THREADS"
		kill $pid
	else
		python test.py
	fi
	local1=`expr $local1 - 1`
	lpcnt=`expr $lpcnt + 1`
	sleep 1
	done
echo $1 loops and $count deadlocks
