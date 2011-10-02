#!/usr/bin/python
# -*- coding: iso-8859-1 -*-

import os
import re

doxygen_xml_path="./doc/xml/"
sphinx_src_path="./doc/manual/source/"
ref_url="/home/marco/projects/casper/repos/doc/html/"

dfiles = os.listdir(doxygen_xml_path)
sfiles = os.listdir(sphinx_src_path)

# get sphinx source files (*.rst-in)
rsts = []
for fname in sfiles:
  if re.match("^.+\.rst\-in$",fname)!=None:
    rsts.append(sphinx_src_path+fname)
    
# read all rst-in files
rsts_contents = []
for fname in rsts:
  f = open(fname,"r")
  rsts_contents.append((fname,f.read()))
  f.close()
  
# get doxygen file list
xmls = []
# parse structs and classes
for fname in dfiles:
  r = re.match("^.+casper_1_1([^0-9]+)\.xml",fname)
  if r !=None:
    xmls.append((ref_url+fname[:-3]+"html",r.group(1)))

for (rsts_fname,contents) in rsts_contents:
  for (dox_fname,obj) in xmls:
    contents=re.sub(r"([^a-zA-Z0-9_]+)"+obj+r"([^a-zA-Z0-9_]+)",r"\1`"+obj+r" <"+dox_fname+r">`_\2",contents)
  # replace links to reference manual
  contents = contents.replace("refmanurl",ref_url)
  f = open(rsts_fname[:-3],"w")
  f.write(contents)
  f.close()
  
