"""
" Author: Michael Gorlin
" Date: 2015-10-04
" 
" This file contains the server's program.
"""

from socket import *
from threading import *
import os 
import argparse as ap

from httpheaders import *


SERVER_403 = '<html><body>403 FORBIDDEN</body></html>'
SERVER_404 = '<html><body>404 Not Found</body></html>'

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


def handleMessage(msg):
    title, fields, body = parseHeader(msg, REQ_V)
    if title[0].upper() == REQ_GET:
        return handleGetResponse(title[1], fields)
    else:
        return handlePutResponse(title[1], fields, body)


"""
" This function handles a get request, by finding the file path
" and then either: reads the file and generates a 200 OKAY response
" or generates a 404 Not Found response.
"
" path - the path send by the client.
" fields - additional header fieldsVk
"
" returns a valid HTTP get response
"""
def handleGetResponse(path, fields):

    filepath = getFilePath(path)

    if filepath is None:
        return generateResponse(RES_404, fields, SERVER_404)
    fread = open(filepath, 'r')
    rspbody = fread.read()
    resp = generateResponse(RES_OK, fields, rspbody)
    fread.close()

    return resp

"""
" Handles a put request by writing the contents of body to 
" the specified file
"
" path - the path send by the client
" fields - additional header fields
" body - the contents of the body in the PUT request
"
" returns a valid HTTP put response, assuming the file can be
" written to.
"""
def handlePutResponse(path, fields, body):
    if os.path.exists(path):
        fields['Connection'] = 'Close'
        return generateResponse(RES_403, fields, SERVER_403)
    fout = open(path, 'w')
    fout.write(body)
    fout.close()
    return generateResponse(RES_OK, fields, "")

    
    

class RThread(Thread):
    """
    " This is the thread object that handles requests.
    """
    def __init__(self, csock, name = None):
        Thread.__init__(self, None, self.handle, name)
        self.csock = csock

    def getMessage(self):
        return self.csock.recv(4028)

    def handle(self):
        print 'SERVER ----- Thread'
        msg = self.getMessage()
        if not msg:
            print "SERVER ----- PHANTOM REQEST DETECTED"
            return
        rsp = handleMessage(msg)
        self.csock.send(rsp)
        self.csock.close()
            

if __name__ == '__main__':
    # Setup/Parse arguments my only high level library use.
    argp = ap.ArgumentParser(description='A simple webserver')
    argp.add_argument('port', type=int, help='The port number to open the socket')
    args = argp.parse_args()
    serverSocket = socket(AF_INET, SOCK_STREAM)

    # Set to whatever you need. This is ip of my virtual machine
    serverSocket.setsockopt(SOL_SOCKET, SO_REUSEADDR, 1)
    serverSocket.bind(('192.168.33.10', 30009))
    #serverSocket.bind(('localhost', args.port))
    serverSocket.listen(2)

    try:
       while 1:
            cs, ca = serverSocket.accept()
            print 'SERVER ----- Received request from: ', ca
            RThread(cs).handle()

    except IOError: 
        pass
    finally:
        print 'closing'
        serverSocket.close()
        
