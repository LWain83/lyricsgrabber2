# -*- coding: utf-8 -*-
from __future__ import division
import encodings.utf_8
from BeautifulSoup import BeautifulSoup
from Html2Text import ConvertHtmlToText
from LevenshteinDistance import LevenshteinDistance
import re
from grabber import *
from Lucky import *


class DarkLyrics(LyricProviderBase):
    def GetName(self):
        return "Dark Lyrics"

    def GetAuthor(self):
    	return "T.P Wang"

    def GetDescription(self):
        return "DarkLyrics.com has been founded in 2001 and since then has become one\nof the most popular metal-oriented resources on the Net."

    def GetURL(self):
        return "http://www.darklyrics.com"

    def GetVersion(self):
        return "0.4"

    def Query(self, handles, status, abort):
        # Threshold, should be configurable
        threshold = 0.8
        DARK_LYRICS_SITE = 'darklyrics.com'
        result = []
        
        for handle in handles:
            # Progress + 1
            status.Advance()
            
            if abort.Aborting():
                return result
            
            jump_to = 0
            best = 0.0
            title = handle.Format('[%title%]')
            compare_to = '. %s' % title
            keywords = handle.Format('[%artist%][ %album%][ %title%]')
            
            try:
                html = GoogleLuck(keywords, DARK_LYRICS_SITE).read()
                
                # Choose the best link of TOC
                for href in re.finditer(r'<a\shref="#(?P<no>\d+)">(?P<text>.+?)</a>', html, re.I):
                    
                    try:
                        no = int(href.group('no'))
                        text = href.group('text')
                        good = 1 - LevenshteinDistance(compare_to, text) / len(compare_to)
                        
                        if good >= threshold and good > best:
                            jump_to = no
                            best = good
                            
                    except:
                        pass
                    
                if jump_to == 0:
                    result.append('')
                    continue
                
                # Extract Lyrics
                soup = BeautifulSoup(html)
                lyric = soup.find('a', {'name' : jump_to})
                [e.extract() for e in lyric.findAll('font')]
                result.append(ConvertHtmlToText(str(lyric)).encode('utf_8').strip())
            except BadLuck:
                result.append('')
                continue
            except Exception, e:
                traceback.print_exc(file=sys.stdout)
                result.append('')
                continue
            
        return result

if __name__ == "__main__":
    LyricProviderInstance = DarkLyrics()
