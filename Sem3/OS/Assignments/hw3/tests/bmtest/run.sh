#!/bin/bash

./server &
pid=`ps | grep server | cut -c1-6`
echo Running on server at pid =$pid
./prod
sleep 2
kill $pid



