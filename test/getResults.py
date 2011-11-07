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
  
def getTime(file1):
  secs1=getTestsField(file1,"number(secs)")

  for i in secs1.keys():
    print i,

  for i in secs1.keys():
    print secs1[i],

if __name__ == "__main__":

  import argparse

  parser = argparse.ArgumentParser(description='Get timings from BenchmarkResults.xml file')
  parser.add_argument('xml', nargs=1,
		    help='path to the source BenchmarkResults.xml file to store')

  args = parser.parse_args()

  getTime(args.xml[0])
