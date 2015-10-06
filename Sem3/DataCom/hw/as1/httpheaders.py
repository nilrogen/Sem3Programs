"""
" Author: Michael Gorlin
" Date: 2015-09-26
"
" This file contains all nessessary functions/classes to parse and 
" format http messages.
"""

# Response Status constants.
RES_OK  = 'HTTP/1.0 200 OK'
RES_403 = 'HTTP/1.0 403 Forbidden'
RES_404 = 'HTTP/1.0 404 Not Found'

# REQUEST TYPES
REQ_GET = "GET"
REQ_PUT = "PUT"

RES_V = 0
REQ_V = 1

PUT_V = 1
GET_V = 0


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
 
"""
" This method generates a request in a manner similar to generateResponse
"
" fpath - The path of the file requested or to be sent
" rtype - The type of request (either REQ_PUT or REQ_GET)
" headerargs - dictionary of header fields.
"
" return - A formatted http request message.
"""
def generateRequest(fpath, rtype, headerargs = {}):
    resp =  '%s %s HTTP/1.0\r\n' % (rtype, fpath)

    for k, v in headerargs.iteritems():
        resp += '%s: %s\r\n' % (str(k), str(v))
    resp += '\r\n'

    return resp
       

"""
" Parses the first line of the message, (e.g. GET / Http/1.1)
"
" message - The message to parse.
" htype - the type of message (RES_V or REQ_V)
"
" return - a 3 or 2 tuple:
"  if htype == RES_V then its formatted (version, status_code, status_string)
"  if htype == REQ_V and is a PUT request: (GET/PUT, path, version)
"""
def parseType(message, htype):
    splitv = message.split(' ')
    if htype == RES_V: # Response
        return (splitv[0], splitv[1], ' '.join(splitv[2:]))
    elif htype == REQ_V: #Request
        return (splitv[0], splitv[1], splitv[2])


"""
" This function parses a http request/response. 
" 
" message - The 'entire' message.
" htype - The type of message. Either: REQ_V or RES_V
"
" returns - A 3-tuple of types (2/3-tuple, dict, string)
"   rv[0] - A 2 or 3-tuple (depending on message type) of first line
"   rv[1] - A dictionary containing the headers key-value pairs. 
"   rv[2] - The body of the message (or None)
"
"""
def parseHeader(message, htype):
    splitv = message.split('\r\n')
    htype = parseType(splitv[0], htype)
    fields = {}

    for val in splitv[1:]:
        if val == '':
            break
        tmp = val.split(': ') 
        fields[tmp[0]] = tmp[1]
    if splitv[-1] == '':
        body = None
    else:
        body = message.split('\r\n\r\n')[1]

    return htype, fields, body

