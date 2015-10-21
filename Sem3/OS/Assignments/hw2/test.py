import os
import glob as g


d = [[],[],[],[]]
old = {}

nc = 50

print 'Testing Consumers -------'
for i in range(nc):
    f = open('crap/c'+str(i+1), 'r')
    for val in f:
        val = val.strip().split(' ')
        dtype = int(val[0])
        donut = int(val[1])
        if donut in d[dtype]:
            print "THIS IS WRONG. Donut %d, from consumer c%d" % (donut, i+1)
            print 'PREVIOUS WAS c%d' % (old[(dtype, donut)])
            break
        d[dtype].append(donut)
        old[(dtype, donut)] = i+1
    f.close()

def test(v):
    t1 = len(v) * (len(v)-1) / 2
    t2 = sum(v)
    print t1, ' ', t2, ' ', t1==t2

print ''
print 'Actual  Test'
print 'Length  Length  Equal?'
print '----------------------'
test(d[0])
test(d[1])
test(d[2])
test(d[3])
