#!/usr/bin/env python
# -*- coding: iso-8859-1 -*-


def getTestsField(filename,field):
  from xml.etree.ElementTree import ElementTree
  r = dict()
  doc = ElementTree(file=filename)
  for test in doc.iterfind('.//test'):
    if test.find('result').text=="ok":
      testName=test.attrib['id']
      r[testName]=test.find(field).text
  return r

 
def getstats(file1):
  import re
  cout = getTestsField(file1,"cout")
  secs = getTestsField(file1,"secs")

  byfilter = dict()
  byfiltereff = dict()
    
  for k in cout.keys():
    #print cout[k]
    filters = re.search(r"Number of filters\s*:\s*([0-9]+)",cout[k])
    props = re.search(r"Number of filter executions\s*:\s*([0-9]+)",cout[k])
    effprops = re.search(r"Number of effective filter executions\s*:\s*([0-9]+)",cout[k])
    domups = re.findall(r"updates\s*:\s*([0-9]+)",cout[k])
    fprops = re.findall(r"Number of executions of\s*([\S]+):\s*([0-9]+)",cout[k])
    efprops = re.findall(r"Number of effective executions of\s*([\S]+):\s*([0-9]+)",cout[k])
    
  #  print k,secs[k],filters.group(1),props.group(1),effprops.group(1),sum(int(i) for i in domups)

    for i in fprops:
        if i[0] not in byfilter.keys():
            byfilter[i[0]] = []
        byfilter[i[0]].append(i[1])    
    for i in efprops:
        if i[0] not in byfiltereff.keys():
            byfiltereff[i[0]] = []
        byfiltereff[i[0]].append(i[1])    

  for k in byfilter.keys():
      if k!='unknown':
          print k,
          for f,ef in zip(byfilter[k],byfiltereff[k]):
              print float(ef)/float(f),
          print    

    
if __name__ == "__main__":

  import argparse

  parser = argparse.ArgumentParser(description='Get stats from benchmark file')
  parser.add_argument('file', nargs=1,
		    help='name of the benchmark file')

  args = parser.parse_args()
  getstats(args.file[0])
