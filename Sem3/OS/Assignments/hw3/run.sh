#!/bin/bash

./bin/bm_server &

for i in `seq 1 20`; do
	./bin/producer &
done

echo $pid
