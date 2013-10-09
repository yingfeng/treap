#!/usr/bin/python

#    genPatterns.py
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

import random, sys
from bisect import bisect_left

if len(sys.argv)!=3:
    print "usage: " +sys.argv[0] + " <lists> <P>"
    sys.exit(0)

N = 10000
P = int(sys.argv[2])
M = 664193

lists_len = []
lists = open(sys.argv[1],"r").readlines()

print "lengths:",
for lst in lists:
    l = int(lst.split()[0])
    lists_len.append(l)
print len(lists_len)

sums = [0]

print "sums:",
for num in lists_len:
    sums.append(num+sums[-1])
print sums[-1]

def getRndLst():
    v = random.randint(1,sums[-1]-1)
    return bisect_left(sums,v)-1

for i in xrange(N):
    S = set()
    while len(S) < P:
        S.add(getRndLst())
    sys.stderr.write(str(P)+" "+" ".join(map(lambda x:str(getRndLst()),S))+"\n")
