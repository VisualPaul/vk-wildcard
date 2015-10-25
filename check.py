import os, sys

def read_file(name):
    with open(name) as f:
        return f.read()
def read_data(name):
    with open(name) as f:
        n = int(f.readline())
        s = []
        for i in xrange(n):
            s.append(f.readline().split())
        for x in s:
            x.sort()
        s.sort()
        return s

for i in xrange(1, 11):
    if '-n' not in sys.argv:
        os.system('cd tests/%02d; ../../main; cd ../..' % i)
    a = read_data('tests/%02d/output.txt' % i)
    b = read_data('tests/%02d.a' % i)
    if a != b:
        print 'test #%d failed' % i
    else:
        print 'test #%d passed' % i
    if '-p' in sys.argv and a != b:
        print "not found"
        for x in set(map(tuple, b)).difference(set(map(tuple, a))):
            print ' '.join(x)
        print "found more"
        for x in set(map(tuple, a)).difference(set(map(tuple, b))):
            print ' '.join(x)
        
    
