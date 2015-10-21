import sys

def test(v):
    t1 = len(v) * (len(v)-1) / 2
    t2 = sum(v)
    print t1, ' ', t2, ' ', t1==t2

    return t1, t2

if __name__ == '__main__':
    d = [[],[],[],[]]
    old = {}

    nc = 50
    print '\nTesting Consumers -----'
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
