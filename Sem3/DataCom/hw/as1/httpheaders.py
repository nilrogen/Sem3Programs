"""
" Author: Michael Gorlin
" Date: 2015-09-26
"
" This file contains all nessessary functions/classes to parse and 
" format http messages.
"""

# Response Status constants.
RES_OK  = "HTTP/1.0 200 OK"
RES_404 = "HTTP/1.0 404 Not Found"

# REQUEST TYPES
REQ_GET = "GET"
REQ_PUT = "PUT"

REQ_V = 0
RES_V = 1

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
 
def generateRequest(fpath, rtype, headerargs = {}):
    resp =  '%s %s HTTP/1.0\r\n' % (rtype, fpath)

    for k, v in headerargs.iteritems():
        resp += '%s: %s\r\n' % (str(k), str(v))
    resp += '\r\n'
    return resp
       

def parseType(message, htype):
    splitv = message.split(' ')
    print splitv

    if htype == RES_V: # Response
        return (splitv[0], splitv[1], ' '.join(splitv[2:]))
    elif htype == REQ_V: #Request
        return (splitv[0], splitv[1], splitv[2])


def parseHeader(message, htype):
    splitv = message.split('\r\n')
    htype = parseType(splitv[0], htype)
    fielddict = {}
    print splitv
    return htype

