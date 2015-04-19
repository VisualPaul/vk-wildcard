import os
def read_file(name):
    with open(name) as f:
        return f.read()
def read_data(name):
    with open(name) as f:
        n = int(f.readline())
        s = []
        for i in xrange(n):
            s.append(f.readline().split())
        return s

for i in xrange(1, 11):
    os.system('cd tests/%02d; ../../main; cd ../..' % i)
    if read_data('tests/%02d/output.txt' % i) != read_data('tests/%02d.a' % i):
        print 'test #%d failed' % i
    else:
        print 'test #%d passed' % i
    
