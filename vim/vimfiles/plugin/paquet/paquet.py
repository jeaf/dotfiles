import bz2
import re
import struct
import sys

def b85enc(s):
    remainder = len(s) % 4
    padding = 4 - remainder if remainder else 0
    s += padding * '\0'
    enc = ''
    for i in xrange(0, len(s), 4):
        q, = struct.unpack('>L', s[i:i+4])
        q,b4 = divmod(q, 85)
        q,b3 = divmod(q, 85)
        q,b2 = divmod(q, 85)
        q,b1 = divmod(q, 85)
        q,b0 = divmod(q, 85)
        enc += chr(b0 + 33)
        enc += chr(b1 + 33)
        enc += chr(b2 + 33)
        enc += chr(b3 + 33)
        enc += chr(b4 + 33)
    enc = enc[:-padding] if padding else enc
    return '<~' + enc + '~>'

def b85dec(enc):
    if enc[:2] != '<~' or enc[-2:] != '~>': raise Exception()
    enc = enc[2:-2]
    remainder = len(enc) % 5
    padding = 5 - remainder if remainder else 0
    enc += padding * 'u'
    dec = ''
    for i in xrange(0, len(enc), 5):
        b0,b1,b2,b3,b4 = struct.unpack('BBBBB', enc[i:i+5])
        val  = (b0-33) * 85**4
        val += (b1-33) * 85**3
        val += (b2-33) * 85**2
        val += (b3-33) * 85**1
        val += (b4-33) * 85**0
        dec += struct.pack('>L', val)
    return dec[:-padding] if padding else dec

def pack(s):
    parts = s.split(u'\n', 1)
    if len(parts) == 2:
        title, content = parts[0], parts[1]
        enc = b85enc(bz2.compress(content.encode()))
        return title + u' ' + unicode(enc)

def unpack(s):
    title, enc = s.split('<~', 1)
    return title.strip() + '\n' + bz2.decompress(b85dec('<~' + enc.strip()))

def main():
    s = sys.stdin.read()
    if re.search('<~.*~>', s):
        print unpack(s)
    else:
        print pack(s)

if __name__ == '__main__':
    main()
