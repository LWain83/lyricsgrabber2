# -*- coding: utf-8 -*-
from encodings import cp1252, utf_8
import urllib
import re
from LevenshteinDistance import LevenshteinDistance
from grabber import LyricProviderBase


class LyrDB(LyricProviderBase):
    def GetName(self):
        return "LyrDB"
    
    def GetVersion(self):
        return "0.3"
    
    def GetAuthor(self):
        return "T.P Wang"
    
    def GetDescription(self):
        return """LyrDB contains more than 1,000,000 lyrics and them are catalogued by title and artist. Also, you'll be able to find songs
        arranged with the original CD's. The artists' discography will let you to find the album you were looking for, and you'll
        discover new artists by following LyrDB's suggested related artists."""
    
    def GetURL(self):
        return "http://www.lyrdb.com"        
    
    def Query(self, handles, status, abort):
        result = []
        
        for handle in handles:
            status.Advance()
            
            if (abort.Aborting()):
                return result
            
            artist = handle.Format("[%artist%]")
            title = handle.Format("[%title%]")
            url = "http://webservices.lyrdb.com/lookup.php?q=%s|%s&for=match&agent=LyricsGrabber" % (urllib.quote_plus(artist), urllib.quote_plus(title))
            
            try:
                lines = urllib.urlopen(url).read()
                m = 0xffffffffffffffff
                best = None
                for line in lines.split("\n"):
                    Id, ti, ar = line.split("\\")  # id\titlle\arist
                    d = LevenshteinDistance(artist, ar) + LevenshteinDistance(title, ti)
                    
                    if m > d:
                        m = d
                        best = Id
                        
                if best == None:
                    result.append('')
                    continue
                
                lyric = urllib.urlopen("http://www.lyrdb.com/getlyr.php?q=%s" % best).read()
                lyric = lyric.decode('cp1252').encode('utf_8')
                result.append(lyric.strip().replace("\n", "\r\n"))
            except Exception, e:
                traceback.print_exc(file=sys.stdout)
                result.append('')
                continue
            
        return result

if __name__ == "__main__":
    LyricProviderInstance = LyrDB()
