import htmllib
import htmlentitydefs
import formatter
import cStringIO
import codecs
import encodings.utf_8


def ConvertHtmlToText(strHTML):
    ''' strHTML should passed as utf8 text'''

    class Formatter(formatter.AbstractFormatter):
        def add_line_break(self):
            
            if not (self.hard_break or self.para_end):
                self.have_label = self.parskip = 0
            
            self.writer.send_line_break()
            self.hard_break = self.nospace = 1
            self.softspace = 0

    class HTML2TextParser(htmllib.HTMLParser):
    	# entities should be encoded as utf8
        entitydefs = dict([ (k, unichr(v).encode('utf_8')) for k, v in htmlentitydefs.name2codepoint.items() ])

        def anchor_end(self):
            if self.anchor:
                self.anchor = None
       
        def convert_charref(self, name):
            try:
                n = unichr(int(name))
            except ValueError:
                return
            return self.convert_codepoint(int(name))
        
        def convert_codepoint(self, codepoint):
            # codepoint should also be encoded as utf8
            return unichr(codepoint).encode('utf_8')
        
        def handle_image(self, src, alt, *args):
            # ignore images
            pass
        
    # cStringIO for output string stream
    sio = cStringIO.StringIO()
    encoder, decoder, reader, writer = codecs.lookup('utf_8')
    utf8io = codecs.StreamReaderWriter(sio, reader, writer, 'replace')
    writer = formatter.DumbWriter(sio)
    prettifier = Formatter(writer)
    parser = HTML2TextParser(prettifier)
    # Parse HTML to plain text
    parser.feed(strHTML)
    parser.close()
    utf8io.seek(0)
    result = utf8io.read()
    sio.close()
    utf8io.close()
    return result.replace('\n','\r\n')
