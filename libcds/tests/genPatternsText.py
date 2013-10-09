 #!/usr/bin/python

#    genPatternsText.py
#    Copyright (C) 2011  Francisco Claude F.
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

import sys, random, bisect

if len(sys.argv)!=5:
    print "usage: "+sys.argv[0]+" <wordseq> <offsets> <P> <N>"
    sys.exit(0)

P = int(sys.argv[3])
N = int(sys.argv[4])

sys.stderr.write("Reading text\n")
text_fp = open(sys.argv[1],"r")
text_str = text_fp.read()
text_fp.close()
text = text_str.split()

sys.stderr.write("Reading offsets\n")
offsets = map(int, open(sys.argv[2],"r").read().split())

def reasonable(a):
    pos = bisect.bisect_right(offsets,a)
    return a < offsets[pos+1]-P

sys.stderr.write("Generating patterns\n")
for i in xrange(N):
    a = random.randint(0,len(text)-2)
    while not reasonable(a): a=random.randint(0,len(text)-2)
    print P," ".join(text[a+1:a+P+1])
