import os
import sys
from socket import *

from httpheaders import *

if __name__ == '__main__':
    hostname = 'www.cs.uml.edu'
    port = 80
    for res in getaddrinfo(hostname, port, AF_INET, SOCK_STREAM):
        print res
        af, stype, a,b, addr = res

        try: 
            sock = socket(af, stype)
            sock.connect(addr)
        except error as msg:
            print msg
            sock.close()
            sys.exit(1)
        
        req = generateRequest('/', REQ_GET)
        print req
        sock.sendall(req)
        data = sock.recv(1024)
        print data
        sock.close()




            

