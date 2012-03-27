#!/usr/bin/env python
# -*- coding: iso-8859-1 -*-

import sys
import re

s= set()
curSol=""
final=False
for l in sys.stdin.readlines():
  if l=="----------\n":
    s.add(curSol)
    curSol = ""
  elif l=="==========\n":
    final=True
  else:
    curSol += l

for l in sorted(s):    
  print l,"----------"
if final:
  print "=========="
  