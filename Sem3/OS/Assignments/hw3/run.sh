#!/bin/bash

ssh cs91515-4 hw3/bin/bm_server &
ssh cs91515-4 hw3/bin/nm 3 &

ssh cs91515-3 hw3/bin/nm 2 &

ssh cs91515-2 hw3/bin/nm 1 &

ssh cs91515-1 hw3/bin/producer &
ssh cs91515-2 hw3/bin/producer &
ssh cs91515-1 hw3/bin/consumer &
ssh cs91515-3 hw3/bin/consumer &





