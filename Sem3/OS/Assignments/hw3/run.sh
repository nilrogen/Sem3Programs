#!/bin/bash

ssh mgorlin@cs91515-4 hw3/bin/nm 3 &
ssh mgorlin@cs91515-4 hw3/bin/bm_server &

ssh mgorlin@cs91515-3 hw3/bin/nm 2 &

ssh mgorlin@cs91515-2 hw3/bin/nm 1 &
ssh mgorlin@cs91515-1 hw3/bin/producer





