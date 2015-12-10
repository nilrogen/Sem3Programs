#!/bin/bash

if [ `hostname` == 'cs91515-6' ];
then
	./bin/bm_server &
	./bin/nm 3 &
	sleep 6
	./bin/producer & 
	sleep 1
	./bin/producer &
	sleep 1
	./bin/consumer 2> c31 &
	sleep 1
	./bin/consumer 2> c32 &
	sleep 1
	./bin/consumer 2> c33 &
	sleep 1
	./bin/consumer 2> c34 &
	sleep 1
	./bin/consumer 2> c35 &
elif [ `hostname` == 'cs91515-3' ];
then
	./bin/nm 2 &
	sleep 9
	./bin/producer &
	./bin/consumer 2> c21 &
	sleep 1
	./bin/consumer 2> c22 &
	sleep 1
	./bin/consumer 2> c23 &
	sleep 1
	./bin/consumer 2> c24 &
	sleep 1
	./bin/consumer 2> c25 &

elif [ `hostname` == 'cs91515-5' ];
then
	./bin/nm 1 &
	sleep 12
	./bin/producer & 
	sleep 1
	./bin/producer & 
	sleep 1
	./bin/consumer 2> c11 &
	sleep 1
	./bin/consumer 2> c12 &
	sleep 1
	./bin/consumer 2> c13 &
	sleep 1
	./bin/consumer 2> c14 &
	sleep 1
	./bin/consumer 2> c15 &
fi

