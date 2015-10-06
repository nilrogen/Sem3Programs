"""
" Author: Michael Gorlin
" Date:   2015-10-06
" 
" This is the client program.
"""
from socket import *
import os
import sys
import argparse as ap

from httpheaders import *

if __name__ == '__main__':
    # Get cmd line args
    argp = ap.ArgumentParser(description='Client for webserver')
    argp.add_argument('-v', action='store_true', dest='verbose', help='Prints more info')
    argp.add_argument('host', type=str, help='Hostname')
    argp.add_argument('port', type=int, help='Port Number')
    argp.add_argument('type', type=str, help='Type of request')
    argp.add_argument('file', type=str, help='The file to get')

    args = argp.parse_args()

    print args.verbose

    hostname = args.host
    port = args.port

    # Try all possible addresses for a domain. Sometimes a call to this
    # function has multiple results, www.reddit.com is an example.
    # The program uses the returned address. 
    # 
    for res in getaddrinfo(hostname, port, AF_INET, SOCK_STREAM):
        af, stype, a,b, addr = res

        # Try to open a socket, On failure exit the program
        try: 
            sock = socket(af, stype)
            sock.connect(addr)
        except error as msg:
            print msg
            sock.close()
            sys.exit(1)

        # The header fields. Start with the Host set
        hfdict = { 'Host' : '%s:%d' % addr }
        
        # Check type of message to send
        if args.type.upper() == REQ_GET:
            req = generateRequest(args.file, REQ_GET, hfdict)
        elif args.type.upper() == REQ_PUT:
            try:
                fout = open(args.file, 'r')
            except IOError:
                print 'Client Error ----- File does not exist'
                sys.exit(1)
                
            body = fout.read()
            # Send the length of the document
            hfdict['Content-Length'] = len(body)
            req = generateRequest(args.file, REQ_PUT, hfdict)
            req += body
            fout.close()
        else: # Default to get request
            req = generateRequest(args.file, REQ_GET)

        print 'CLIENT ----- \n', req
        sock.sendall(req)

        # Read all data from the socket.
        data = ''
        while 1:
            tmp = sock.recv(1024)
            if not tmp:
                break
            data += tmp
        
        # Only print the body if -v option set otherwise print the header.
        if args.verbose:
            print 'CLIENT ----- Server Response\n', data
        else:
            print 'CLIENT ----- Server Response Header\n', \
                   data[:data.find('\r\n\r\n')]

        sock.close()
        sys.exit(0)

