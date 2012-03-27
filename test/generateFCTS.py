#!/usr/bin/env python
# -*- coding: iso-8859-1 -*-

import os.path

def generate(rootdir,fzbin,canonbin,expected):
  i=0
  print "#testsuite(FzConformanceTest,Conformance tests included in the flatzinc distribution)"
  tests = set()
  for root, dirs, files in os.walk(rootdir):
    for name in files:
      path = os.path.join(root, name)
      name,ext = os.path.splitext(path)
      if ext==".fzn":
	tests.add(name)
  for test in tests:
    if os.path.exists(test+".opt"):
      f = open(test+".opt", "r")
      opt = f.readline()[:-1]
    else:
      opt = ""
    print "#test(FzConformanceTest-"+str(i)+","+test+",flatzinc)"
    if expected:
      print  "bash -c \"cat "+test+".exp | "+canonbin+"\""
    else:
      print  "bash -c \""+fzbin+" "+opt+" "+test+".fzn | "+canonbin+"\""
    i+=1
    
if __name__ == "__main__":

  import argparse

  parser = argparse.ArgumentParser(description='Generate a BenchmarkFile to run a flatzinc implementation with the flatzinc conformance tests (from the flatzinc distribution)')
  parser.add_argument('fcts_root_dir', nargs=1,
                    help='path to root directory of the fcts tests')
  parser.add_argument('fz_bin', nargs=1,
                    help='path to the flatzinc implementation binary')
  parser.add_argument('canon_bin', nargs=1,
                    help='path to the binary that produces a canonical form of the flatzinc output')

  args = parser.parse_args()

  generate(args.fcts_root_dir[0],args.fz_bin[0],args.canon_bin[0],False)
  