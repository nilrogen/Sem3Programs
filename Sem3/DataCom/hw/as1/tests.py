import os

from server import *

def testConnectionGet():
        
    wd = os.getcwd()
    
    return \
        getFilePath('/') == os.path.join(wd, 'index.html') and \
        getFilePath('/index.html') == os.path.join(wd, 'index.html') and \
        getFilePath('/w/index.html') == None


       
        
        

if __name__ == '__main__':
    if not testConnectionGet():
        print "Failed Connection get"

    

