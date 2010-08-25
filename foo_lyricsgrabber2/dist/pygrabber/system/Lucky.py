"""
Taked from dotcommie.net
"""
import string
import socket
import urllib

class BadLuck(Exception):
    pass

def GoogleLuck(keywords, site = None):
    find = urllib.quote_plus(keywords) + ('' if site == None else urllib.quote_plus(' site:' + site))
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    ok = 1
    try:
        s.connect(("www.google.com", 80))
    except:
        ok = 0
    if ok == 1:
        s.send('GET /search?hl=en&ie=UTF-8&oe=UTF-8&q='+find+'&btnI=I%27m+Feeling+Lucky HTTP/1.0\r\n')
        s.send('Accept: */*\r\n')
        s.send('Referer: http://www.google.com/search?hl=en&ie=UTF-8&oe=UTF-8&q='+find+'&btnI=I%27m+Feeling+Lucky\r\n')
        s.send('User-Agent: Lyrics Grabber/0.0.1\r\n')
        s.send('Host: www.google.com\r\n')
        s.send('Connection: keep-alive\r\n')
        s.send('Accept-encoding: gzip, deflate\r\n')
        s.send('\r\n')
        data = s.recv(1000)
        s.close()
        data = data.split("\r\n\r\n")
        data = data[0]

        sloc_location=string.find(data, "Location: ") + len("Location: ")
        eloc_location=string.find(data, "\r\n", sloc_location)
        if sloc_location - len("Location: ") > 0:
            return urllib.urlopen(data[sloc_location:eloc_location])
        else:
            raise BadLuck("No results have been found.")
    else:
        raise BadLuck("Can't connect to server")
