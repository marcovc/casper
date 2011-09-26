#!/usr/bin/env python
# -*- coding: iso-8859-1 -*-


def getTestsField(filename,field):
  from Ft.Xml import Parse
  r = dict()
  doc = Parse(filename)
  testsuite = doc.documentElement
  tests = testsuite.xpath('.//test')
  for test in tests:
    if test.xpath('string(result)')=="ok":
      testName=test.xpath('string(name)')
      r[testName]=test.xpath(field)
  return r
  
def compare(file1,file2):
  couteq = getTestsField(file1,"string(cout)")==getTestsField(file2,"string(cout)")
  cerreq = getTestsField(file1,"string(cerr)")==getTestsField(file2,"string(cerr)")

  secs1=getTestsField(file1,"number(secs)")
  secs2=getTestsField(file2,"number(secs)")

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

  kbs1=getTestsField(file1,"number(kb)")
  kbs2=getTestsField(file2,"number(kb)")

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
    print "different stdout!"
  if not cerreq:
    print "different stderr!"
  print "average time ratio (file2/file1) =",tgavg
  print "average memory ratio (file2/file1) =",mgavg
  print "ratio of problems solved faster by file2 =",favg
