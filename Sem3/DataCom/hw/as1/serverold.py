"""
" Author: Michael Gorlin
" Date: 2015-09-26
" 
" This file contains the server's program.
"""

from socket import *
from threading import *

import signal
import os 

from httpheaders import *
import glob

mutex = Lock()
openedthreads = []
serversocket = None
    
def setupSignalHandler(sock):
    def handler(signum, frame):
        print "Signal:", signum, "fired."
        for v in openedthreads:
            if v.is_alive():
                print "Killing Thread:", v.name
                v.terminate() 
        print "Killing server socket."
        serverSocket.shutdown(SHUT_RDWR)
        serverSocket.close()


    sigs = [signal.SIGHUP, signal.SIGINT, signal.SIGQUIT, signal.SIGBUS, \
            signal.SIGTERM, signal.SIGSEGV, signal.SIGFPE]

    for s in sigs:
        signal.signal(s, handler)

    return handler


"""
" Translates the requsted pathname into an absolute path. 
" path is relative to the working directory of the server.py
" process. 
"
" Paths that represent a directory name map to 
" the index.html file in that directory. 
"
" If the path does not exist in the filesystem then None is
" returned.
""" 
def getFilePath(location):
    wd = os.getcwd()

    if location == '/':
        path = wd
    else:
        # remove '/' from location
        path = os.path.join(wd, location[1:])

    # Check if path is a valid directory
    if os.path.isdir(path):
        path = os.path.join(path, 'index.html')

    # if path maps to nothing return none
    if not os.path.isfile(path): 
        return None
    
    return path

class ConnectionThread(Thread):

    def __init__(self, connSocket, connAddr, name=None):
        Thread.__init__(self, None, self.handle, name)
        self.connSocket = connSocket
        selfconnAddr = connAddr


    def terminate(self):
        print 'Killing connection'
        self.connSocket.close()

    def get(self, location):
        pass
       
    def put(self):
        pass

class blgag:
    def __init__(self, cs, ca, n):
        self.connSocket = cs
        self.ca = ca
        self.n  = n

    def handle(self):
        try:
            msg = self.connSocket.recv(512)
            print msg
            resp = generateResponse(R_404, {"Connection" : "Close"}, "<html><body>404 Not Found</body></html>")
            self.connSocket.send(resp)
        except IOError:
            print "IO ERROR"


if __name__ == '__main__':
    serverSocket = socket(AF_INET, SOCK_STREAM)

    # Set to whatever you need. This is ip of my virtual machine
    serverSocket.bind(('192.168.33.10', 30009))
    #serverSocket.bind(('localhost', 30009))
    serverSocket.listen(2)
    serverSocket.setsockopt(SOL_SOCKET, SO_REUSEADDR, True)

    signalHandler = setupSignalHandler(socket)

    try:
       while 1:
            cs, ca = serverSocket.accept()

            print "Received request:", ca
            msg = cs.recv(512)
            print msg
            resp = generateResponse(R_404, {"Connection" : "Close"}, "<html><body>404 Not Found</body></html>")
            cs.send(resp)

            cs.close()
    except IOError: 
        pass
    finally:
        serverSocket.close()
        
