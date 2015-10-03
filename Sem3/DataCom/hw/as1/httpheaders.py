"""
" Author: Michael Gorlin
" Date: 2015-09-26
"
" This file contains all nessessary functions/classes to parse and 
" format http messages.
"""

# Response Status constants.
RES_OK  = "HTTP/1.1 200 OK"
RES_404 = "HTTP/1.1 404 Not Found"

# REQUEST TYPES
REQ_GET = "GET"
REQ_PUT = "PUT"

"""
" This method generates data to be sent.
"
" status - The status line, as a string without \r\n. 
"               Example: HTTP/1.1 200 OK
" headerargs - A dictionary of header fields. Both the key and value
"                   should be strings. Content length inside the function.
" data - The message body.
"
" return: A correctly formatted http response message.
"""
def generateResponse(status, headerargs, data):
    resp = status + '\r\n'

    headerargs['Content-Length'] = str(len(data))
    for k, v in headerargs.iteritems():
        resp += k + ': ' + v + '\r\n'
    resp += "\r\n" + data

    return resp
 

def generateRequest(hostname, rtype, headerargs = {}):
    resp =  '%s %s HTTP/1.0\r\n' % (rtype, hostname)

    for k, v in headerargs.iteritems():
        resp += '%s: %s\r\n' % (str(k), str(v))
    resp += '\r\n'
    return resp
       

class headerHandler:
    def __init__(self, message):
        self.message = message
        self.parsed = False
    
    def parse(self):
        pass


class responseHandler(headerHandler):
    def parse(self):
        pass
    def requestType(self):
        pass

class requestHandler(headerHandler):
    def parse(self):
        pass
