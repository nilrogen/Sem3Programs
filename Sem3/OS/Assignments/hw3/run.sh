#!/bin/bash

hname=`hostname`
role=0

if [ $hname == "cs91515-1" ]; then
	./bin/nm 2 &
elif [ $hname == "cs91515-4" ]; then
	./bin/nm 1 &
	./bin/bm_server & 
fi

sleep 1
./bin/producer

killall nm 
killall bm_server



