import os
import glob as g


d = [[],[],[],[]]

nc = 50

for i in range(nc):
    f = open('crap/f'+str(i+1), 'r')
    for val in f:
        val = val.strip().split(' ')
        dtype = int(val[0])
        donut = int(val[1])
        if donut in d[dtype]:
            print "THIS IS WRONG"
            break
        d[dtype].append(donut)
    f.close()

def test(v):
    t1 = len(v) * (len(v)-1) / 2
    t2 = sum(v)
    print t1, t2, t1==t2

test(d[0])
test(d[1])
test(d[2])
test(d[3])
