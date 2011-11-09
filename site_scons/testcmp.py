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

 
def compare(file1,file2):
  couteq = getTestsField(file1,"cout")==getTestsField(file2,"cout")
  cerreq = getTestsField(file1,"cerr")==getTestsField(file2,"cerr")

  secs1=getTestsField(file1,"secs")
  secs2=getTestsField(file2,"secs")
  
  tgavg=1.0
  countPositive = 0
  countFaster = 0
  for i in secs1.keys():
    if  float(secs1[i])>0 and float(secs2[i])>0:
      countPositive+=1
      tgavg *= float(secs2[i])/float(secs1[i])
      if float(secs2[i])>float(secs1[i]):
          countFaster += 1

  tgavg=pow(tgavg,1.0/countPositive)

  kbs1=getTestsField(file1,"kb")
  kbs2=getTestsField(file2,"kb")

  mgavg=1.0
  for i in kbs1.keys():
    mgavg *= float(kbs2[i])/float(kbs1[i])
  mgavg=pow(mgavg,1.0/len(kbs1.keys()))
  
  return (couteq,cerreq,tgavg,mgavg,float(countFaster)/float(countPositive))

if __name__ == "__main__":

  import argparse

  parser = argparse.ArgumentParser(description='Compare two BenchmarkResults.xml files')
  parser.add_argument('file1', nargs=1,
		    help='name of the first file to compare')
  parser.add_argument('file2', nargs=1,
		    help='name of the second file to compare')

  args = parser.parse_args()

  (couteq,cerreq,tgavg,mgavg,favg) = compare(args.file1[0],args.file2[0])
  if not couteq:
    print "warning: different stdout!"
  if not cerreq:
    print "warning: different stderr!"
  print "average time ratio (file2/file1) =",tgavg
  print "average memory ratio (file2/file1) =",mgavg
  print "ratio of problems solved faster by file2 =",favg
