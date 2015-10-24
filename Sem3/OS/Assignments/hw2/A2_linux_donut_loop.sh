#!/bin/bash

# usage: A2_linux_donut_loop.sh  number_of_runs_integer
# so from the shell prompt:  $ A2_linux_donut_loop.sh 10 
# 10 times and report how many of the runs ended in deadlock

if [ -z $1 ]
then
	echo "USAGE: A2_linux_donut_loop.sh  number_of_runs_integer"
	echo "Try the command again"
	echo " "
fi

if [ -n $3 ] 
then
	args=$3
fi

echo " "
echo "The configuration is for $1 LOOPS"
echo " "

local1=$1
proc=$2
lpcnt=1
count=0

echo $@

while test $local1 -ne 0
do
	echo " "
	echo "Working on LOOP $lpcnt"
	echo " "
	if [ -z $3 ]
	then
		./$proc &
	else
		./$proc $3 &
	fi
	pid=`ps | grep $proc | cut -c1-6`
	echo JOB AND PID IS $pid
	if [[ $proc == "proc*" ]] 
	then
		sleep 15
	else
		sleep 20
	fi
	pid=`ps | grep $proc | cut -c1-6`
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
	fi
	local1=`expr $local1 - 1`
	lpcnt=`expr $lpcnt + 1`
	sleep 1
	done
echo $1 loops and $count deadlocks
