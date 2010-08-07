# -*- coding: utf-8 -*-
# Code from LyricZilla - Linux 下的滚动歌词
from encodings import gb18030, utf_8
import urllib
from xml.dom import minidom
from LevenshteinDistance import LevenshteinDistance
from grabber import LyricProviderBase


__all__ = 'Lyricist'

def __get_lyric_list(title, artist):
    title = urllib.quote(unicode(title, 'utf_8').encode('gb18030'))
    artist = urllib.quote(unicode(artist, 'utf_8').encode('gb18030'))
    url = 'http://www.winampcn.com/lyrictransfer/get.aspx?song=%s&artist=%s&lsong=%s&Datetime=20060601' % (title, artist, title)
    xmltext = urllib.urlopen(url).read()

    try:
        xmltext = xmltext.decode('gbk').encode('utf_8')
        xmltext = xmltext.replace('encoding="gb2312"', 'encoding="utf-8"')
    except:
        pass

    xmldoc = minidom.parseString(xmltext)

    ret = []
    root = xmldoc.documentElement

    for node in root.getElementsByTagName('LyricUrl'):
        got_title = node.attributes['SongName'].value
        got_artist = node.attributes['Artist'].value
        got_url = node.childNodes[0].data
        ret.append({'url': got_url, 'title': got_title, 'artist': got_artist})

    return ret

def __get_lyric_source(url):
    return urllib.urlopen(url).read().decode('utf_8')

def get_lyric_list(title, artist):
    l = __get_lyric_list(title, artist)
    return l

def get_lyric(url):
    try:
        return __get_lyric_source(url)
    except:
        return ''

class Lyricist(LyricProviderBase):
    def GetName(self):
        return "乐辞·歌词（LRC）"

    def GetVersion(self):
        return "0.3"

    def GetURL(self):
        return "http://www.winampcn.com"

    def Query(self, handles, status, abort):
        result = []

        for handle in handles:
            status.Advance()

            if abort.Aborting():
                return result

            title = handle.Format("[%title%]")
            artist = handle.Format("[%artist%]")

            try:
                l = get_lyric_list(title, artist)
                m = 0xffffffffffffffff
                best = None

                for url, ti, ar in l:
                    d = LevenshteinDistance(artist, ar) + LevenshteinDistance(title, ti)
                    if m > d:
                        m = d
                        best = url

                if best == None:
                    result.append('')
                else:
                    lyric = get_lyric(best)
                    result.append(lyric)
                    continue
            except Exception, e:
                traceback.print_exc(file=sys.stdout)
                result.append('')
                continue

        return result

if __name__ == "__main__":
    LyricProviderInstance = Lyricist()
