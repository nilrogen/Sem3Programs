from socket import *
import os
import sys
import argparse as ap

from httpheaders import *

if __name__ == '__main__':
    argp = ap.ArgumentParser(description='Client for webserver')
    argp.add_argument('host', type=str, help='Hostname')
    argp.add_argument('port', type=int, help='Port Number')
    argp.add_argument('type', type=str, help='Type of request')
    argp.add_argument('file', type=str, help='The file to get')

    args = argp.parse_args()

    hostname = args.host
    port = args.port
    for res in getaddrinfo(hostname, port, AF_INET, SOCK_STREAM):
        af, stype, a,b, addr = res

        try: 
            sock = socket(af, stype)
            sock.connect(addr)
        except error as msg:
            print msg
            sock.close()
            sys.exit(1)

        hfdict = { 'Host' : '%s:%d' % addr }
        
        if args.type.upper() == REQ_GET:
            req = generateRequest(args.file, REQ_GET, hfdict)
        elif args.type.upper() == REQ_PUT:
            fout = open(args.file, 'r')
            if fout is None:
                print 'File does not exist'
                sys.exit(1)
                
            body = fout.read()
            hfdict['Content-Length'] = len(body)
            req = generateRequest(args.file, REQ_PUT, hfdict)
            req += body
            fout.close()
        else:
            req = generateRequest(args.file, REQ_GET)

        print 'CLIENT ----- \n', req
        sock.sendall(req)
        data = ''
        while 1:
            tmp = sock.recv(1024)
            if not tmp:
                break
            data += tmp
        #typeinfo, fields, body = parseHeader(data, RES_V)
        #print parseHeader(data, RES_V)
        print 'CLIENT ----- \n', data


        sock.close()

