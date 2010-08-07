# -*- coding: utf-8 -*-
import encodings.utf_8
import urllib
from xml.dom import minidom
from grabber import LyricProviderBase

class LeosLyrics(LyricProviderBase):
    def GetName(self):
        return "Leo's Lyrics"
    
    def GetVersion(self):
        return "0.3"
    
    def GetURL(self):
        return "http://www.leoslyrics.com"
    
    def Query(self, handles, status, abort):
        result = []
        
        for handle in handles:
            status.Advance()
            
            if abort.Aborting():
                return result
            
            artist = handle.Format("[%artist%]")
            title = handle.Format("[%title%]")
            
            try:
                string = urllib.urlopen("http://api.leoslyrics.com/api_search.php?auth=LeosLyrics5&artist=%s&songtitle=%s" % (urllib.quote(artist), urllib.quote(title))).read()
                doc = minidom.parseString(string)
                child = doc.getElementsByTagName("searchResults")[0]
                hid = None
                
                for e in child.getElementsByTagName("result"):
                    if e.getAttribute("exactMatch") == "true":
                        hid = e.getAttribute("hid").encode('utf_8')
                        break

                if hid == None:
                    result.append('')
                    continue
                
                string = urllib.urlopen("http://api.leoslyrics.com/api_lyrics.php?auth=LeosLyrics5&hid=%s" % hid).read()
                doc = minidom.parseString(string)
                child = doc.getElementsByTagName("lyric")[0]
                text = child.getElementsByTagName("text")[0]
                lyric = text.childNodes[0].data.encode('utf_8')
                result.append(lyric)
            except Exception, e:
                traceback.print_exc(file=sys.stdout)
                result.append('')
                continue
        
        return result

if __name__ == "__main__":
    LyricProviderInstance = LeosLyrics()
