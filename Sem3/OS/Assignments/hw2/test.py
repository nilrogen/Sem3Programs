#!/usr/bin/python

import glob 
def test(v):
    t1 = len(v) * (len(v)-1) / 2
    t2 = sum(v)
    print t1, ' ', t2, ' ', t1==t2

    return t1, t2

if __name__ == '__main__':
    d = [[],[],[],[]]
    old = {}
    i = 1

    print '\nTesting Consumers -----'
    for fname in glob.glob("fulloutput/c*"):
        fread = open(fname, 'r')
        for line in fread:
            line = line.strip().split(' ')
            dtype = int(line[0])
            donut = int(line[1])
            if donut in d[dtype]:
                print "THIS IS WRONG. Donut %d, from consumer c%d" \
                       % (donut, i)
                print 'PREVIOUS WAS c%d' % (old[(dtype, donut)])
                i += 1
                break
            d[dtype].append(donut)
            old[(dtype, donut)] = i
            i += 1
        fread.close()


    print 'Actual  Test'
    print 'Length  Length  Equal?'
    print '----------------------'

    v1, v2 = 0, 0
    for i in range(4):
        t1, t2 = test(d[i]) 
        v1 += t1
        v2 += t2

    print '\nTotal   Total\n--------------'
    print '%d   %d' % (v1, v2)
