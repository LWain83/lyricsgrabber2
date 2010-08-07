# -*- coding: utf-8 -*-
# Inspired by Sergio Pistone's ruby script for wiki_lyrics plugin for Amarok
from encodings import cp1252, utf_8
import re
from Html2Text import ConvertHtmlToText
from Lucky import *
from grabber import LyricProviderBase


class AZLyrics(LyricProviderBase):
    def GetName(self):
        return "AZ Lyrics"

    def GetAuthor(self):
        return "T.P Wang"

    def GetURL(self):
        return "http://www.azlyrics.com"
    
    def GetVersion(self):
        return "0.3"

    def Query(self, handles, status, abort):
        AZ_LYRICS_SITE = 'azlyrics.com'
        result = []
        
        for handle in handles:
            # Progress + 1
            status.Advance()
            
            if abort.Aborting():
                return result

            keywords = handle.Format('[%artist%][ %title%] lyrics')
            
            try:              
                html = GoogleLuck(keywords, AZ_LYRICS_SITE).read()
                # I think there's a better way, however, I'm not good at Python :(
                html = unicode(html, 'cp1252').encode('utf_8')
                html = re.sub(r'[ \n\r\t]', " ", html)
                m = re.search(r'\<!-- END OF RINGTONE 1 --> *?<B> ?"([^<]+)" ?</b>', html, re.I)
                
                if m == None:
                    result.append('')
                    continue
                
                html, n1 = re.subn(r'(?i)^.*"</b><br> *<BR> *', '', html)
                html, n2 = re.subn(r'(?i) *<BR> *<BR> *\[ <a href=.*$', '', html)

                if n1 == 0 or n2 ==0:
                    result.append('')
                    continue

                html = re.sub(r'<i>\[Thanks to.*$', '', html)
                result.append(ConvertHtmlToText(html).encode('utf_8').strip())
            except BadLuck:
                result.append('')
                continue
            except Exception, e:
                traceback.print_exc(file=sys.stdout)
                result.append('')
                continue
            
        return result

if __name__ == "__main__":
    LyricProviderInstance = AZLyrics()
