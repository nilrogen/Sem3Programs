"""
" Author: Michael Gorlin
" Date: 2015-09-26
" 
" This file contains the server's program.
"""

from socket import *

import os 

from httpheaders import *
import glob

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

if __name__ == '__main__':
    serverSocket = socket(AF_INET, SOCK_STREAM)

    # Set to whatever you need. This is ip of my virtual machine
    serverSocket.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)
    #serverSocket.bind(('192.168.33.10', 30009))
    serverSocket.bind(('localhost', 30009))
    serverSocket.listen(2)

    #signalHandler = setupSignalHandler(socket)

    try:
       while 1:
            cs, ca = serverSocket.accept()

            print "Received request:", ca
            msg = cs.recv(512)
            print msg
            resp = generateResponse(RES_404, {"Connection" : "Close"}, "<html><body>404 Not Found</body></html>")
            cs.send(resp)
            cs.close()
    except IOError: 
        pass
    finally:
        print 'closing'
        serverSocket.close()
        
