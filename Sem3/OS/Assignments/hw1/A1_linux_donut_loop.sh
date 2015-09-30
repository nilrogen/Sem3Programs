#! /bin/sh

# usage: A1_linux_donut_loop.sh  number_of_runs_integer
# so from the shell prompt:  $ A1_linux_donut_loop.sh 10 
# will run the producer program and the consumer
# programs (5 of them) 10 times and report how many
# of the 10 runs ended in deadlock

if [ -z $1 ]
then
	echo "USAGE: A1_linux_donut_loop.sh  number_of_runs_integer"
	echo "Try the command again"
	echo " "
fi
if [ -z $1 ]
then
	let $2 = 1
fi
echo " "
echo " "
echo " "
echo "The configuration is for $1 LOOPS"
echo " "

local1=$1
local2=$2
lpcnt=1
count=0
while test $local1 -ne 0
do
	echo "___________________________________________________________"
	echo " "
	echo "Working on LOOP $lpcnt"
	echo " "
	./producer >  prod_out.txt & 
	sleep 1 
	for i in `seq 1 $local2`; do
		./consumer $i > data/c$i &
	done
	sleep 8
	pid=`ps | grep -v 'grep' | grep 'producer' | cut -c1-6`
	if ps | grep -v 'grep' | grep -q 'consumer'
	then
		echo " "
		echo "DEADLOCK DETECTED ON LOOP $lpcnt"
		echo " "
		count=`expr $count + 1`
		pkill -f consumer
	else
		echo "LOOP $lpcnt COMPLETED SUCCESSFULLY"
		echo " "
	fi
	echo ">>>>>>>> NOW KILLING THE PRODUCER PROCESS PID: $pid"
	kill $pid
	local1=`expr $local1 - 1`
	lpcnt=`expr $lpcnt + 1`
	sleep 1
done
echo "___________________________________________________________"
echo " "
echo $1 loops and $count deadlocks
echo " "
echo " "
