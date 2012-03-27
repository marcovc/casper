#!/usr/bin/env python
# -*- coding: iso-8859-1 -*-

 ###########################################################################
 #   Python script for automating benchmark execution and statistics       #
 #                                                                         #
 #   Copyright:                                                            #
 #   2011-2012 - Marco Correia <marco.v.correia@gmail.com>                 #
 #                                                                         #
 #   Licensed under the Apache License, Version 2.0 (the "License");       # 
 #   you may not use this file except in compliance with the License.      # 
 #   You may obtain a copy of the License at                               # 
 #            http://www.apache.org/licenses/LICENSE-2.0                   # 
 #   Unless required by applicable law or agreed to in writing, software   # 
 #   distributed under the License is distributed on an                    # 
 #   "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,          # 
 #   either express or implied.                                            # 
 #   See the License for the specific language governing permissions and   # 
 #   limitations under the License.                                        #
 ###########################################################################


import subprocess
from multiprocessing import Pool
from multiprocessing import Condition
from multiprocessing import Manager
from multiprocessing import cpu_count
import sys
import time
import threading
import os
import exceptions
import re

onlinux = os.name=="posix"
if onlinux:
    import resource

def calcETA(x, y, total):
    import numpy
    maxdegree = 4
    x = numpy.array(x)
    y = numpy.array(y)
    p = numpy.poly1d(numpy.polyfit(x,y,min(len(x),maxdegree)))
    return p(total-len(x)),0
        
import math
import shlex

class ProgressBar:
    def __init__(self,width=40):
        self.count = 0
        self.completed = 0
        self.width = width
    def reset(self):
        self.completed=0
    def setTaskCount(self,c):
        self.count = c
    def signalTaskComplete(self):
        self.completed += 1
    def display(self):
        nbars = int(self.completed/float(self.count)*self.width)
        
        percent = self.completed/float(self.count)*100        
        percentStr = str("%(percent)d" % {"percent" : percent})+"%"  
        percentWidth = len(percentStr)

        startPercent = (self.width-percentWidth)/2 
        endPercent = startPercent+percentWidth-1

        r = "["
        for i in range(0,self.width):
            if i < startPercent and i < nbars:
                r += "+"
            elif i == startPercent:
                r += percentStr
            elif i > endPercent and i < nbars:
                r += "+"
            elif (i < startPercent or i > endPercent) and i >= nbars:
                r += " "
        r += "]"
        return r
    
    
class Stats:
    def start(self):
        if  onlinux:
            usage = resource.getrusage(resource.RUSAGE_CHILDREN)
            self.secs = usage.ru_utime+usage.ru_stime
        else:
            self.startTime = time.clock()
    def finish(self):
        if onlinux:
            usage = resource.getrusage(resource.RUSAGE_CHILDREN)
            self.secs = usage.ru_utime+usage.ru_stime - self.secs
        else:
            self.secs = time.clock()-self.startTime 
        self.memkb = 0
        
class Benchmark:
    skipped, ok, timeout, memout,error = range(5)
    result2str = { 0 : "skipped", 1 : "ok", 2 : "timeout", 3 : "memout", 4 : "error"}
    
    def resultAsStr(self):
        if self.result!=self.error:
            return self.result2str[self.result]
        return "error: "+str(self.exception)
    
    def __init__(self,command,name=None,description=None,categories=["Benchmark"]):
        self.command = command
        if name==None:
            name = shlex.split(command)[0]
        if description==None:
            description = command
        self.name = name
        self.description = description
        self.result = self.skipped
        self.secs = 0.0
        self.memkb = 0.0
        self.categories = categories
        self.cout = None
        self.cerr = None
        self.exception = None
        self.stats = Stats()

      
    def __call__(self,maxtime,maxmem):
        def target():
            cmd = shlex.split(self.command)
            self.stats.start()
            try:
                #if maxmem!=None and onlinux:	FIXME
                #    resource.setrlimit(resource.RLIMIT_AS,(maxmem*1024,maxmem*1024))
                self.process = subprocess.Popen(cmd,stderr=subprocess.PIPE,stdout=subprocess.PIPE)
                self.cout,self.cerr = self.process.communicate()
                self.err = None
            except Exception as err:
                self.err = err
            self.stats.finish()
        thread = threading.Thread(target=target)
        thread.start()
        if maxtime != None:
            thread.join(maxtime)
        else:
            thread.join()
            
        if thread.is_alive():
            thread.join()
            return (self.cout,self.cerr,True,self.err,self.stats)
        return (self.cout,self.cerr,False,self.err,self.stats)
        
    def store_results(self,(cout,cerr,timedout,exc,stats)):
        self.cout = str(cout)
        self.cerr = str(cerr)
        self.secs = stats.secs
        self.memkb = stats.memkb
        if timedout == True:
            self.result = self.timeout
        elif exc==None:
            self.result = self.ok
        elif type(exc)==exceptions.MemoryError:
            self.result = self.memout
        else:
            self.result = self.error
            self.exception = exc

import sys

def formatTime(secs):
    if secs < 0:
        secs = 0
    mins = secs/60
    if mins > 0:
        secs = secs%60
    hours = mins/60
    if hours > 0:
        mins = mins%60
    days = hours/24
    if days>0:
        hours = hours%24
    # printing
    r = ""
    if days>0:
        r +=  str(days) + "d:"
    if hours>0:
        r += str(hours) + "h:"
    if mins>0:
        r += str(mins) + "m:"
    r += str(secs)+"s"
    return r
    
def average(l):
    avg = 0
    for i in l:
        avg += i
    return avg / len(l)
 
def std(l):
    avg = average(l)
    stddev = 0
    for i in l:
        stddev += math.pow(i-avg,2);
    return math.sqrt(stddev/len(l));
                
class RunQueue:
    def __init__(self,nbworkers):
        self.nbworkers = nbworkers
        self.pool = Pool(processes=nbworkers)
        self.progressBar = ProgressBar()
        self.nbTasks = 0
        self.nbCompleted = 0
        self.timeoutCount = 0
        self.errorCount = 0
        self.exectimes = []
        self.lock = Condition()
        
    def displayProgress(self):
        r = self.progressBar.display()
        
        avg = average(self.exectimes)
        stddev = std(self.exectimes)
        
        if False: #self.nbCompleted>1:
            x=[]; y=[]
            s = 0; c= 0
            for i in self.exectimes:
                c += 1
                s += i
                x.append(c)
                y.append(s)
            eta,stddev = calcETA(x,y,self.nbTasks)
        else:
            eta = avg * (self.nbTasks-self.nbCompleted)
               
        r += " avgtime:" + formatTime(int(avg)) \
                  + "|ETA:" + formatTime(int(eta)) \
                  + chr(177) + formatTime(int(stddev*(self.nbTasks-self.nbCompleted))) \
                          + "|timeouts:" + str(self.timeoutCount) + "|errors:" + str(self.errorCount);

        sys.stdout.write(r)
        sys.stdout.write("       \r")
        sys.stdout.flush()
     
    def signalTaskEnd(self,(cout,cerr,timedout,exc,stats)):
        self.lock.acquire()
        self.nbCompleted += 1
        self.progressBar.signalTaskComplete()
        self.timeoutCount += timedout
        self.errorCount += exc!=None
        self.exectimes += [stats.secs]
        self.displayProgress()
        self.lock.release()
        #print self.nbCompleted,self.progressBar.count
        
    def add(self,benchmark,endTaskHandler,maxtime=None,maxmem=None):
        self.nbTasks += 1
        self.progressBar.setTaskCount(self.nbTasks)        
        self.pool.apply_async(func=benchmark,args=[maxtime,maxmem],callback=endTaskHandler)
        
    def wait(self):          
        self.pool.close()
        self.pool.join()
        sys.stdout.write("\n")
        sys.stdout.flush()

class EndTaskHandler:
    def __init__(self,benchmark,runqueue,samplecount):
        self.benchmark = benchmark
        self.runqueue = runqueue
        self.samplecount = samplecount
        self.besttime = -1
        self.lock = Condition()
        #print "handler: creating ",id(self)
    #def __del__(self):
        #print "handler: destroying",id(self)
        
    def __call__(self,(cout,cerr,timedout,exc,stats)):
        #print "handler: calling",id(self)
        self.lock.acquire()
        if self.besttime == -1 or (stats.secs<self.besttime and exc==None):
            self.benchmark.store_results((cout,cerr,timedout,exc,stats))
        self.lock.release()
        self.runqueue.signalTaskEnd((cout,cerr,timedout,exc,stats))

class BenchmarkFile:
    def __init__(self,path):
        f = open(path,"r")
        comment = re.compile("^#.*$")
        testsuite_info = re.compile(r"^#testsuite\(([^,]*),([^,]*)((?:,[^,]+)*)\).*$")
        test_info = re.compile(r"^#test\(([^,]+),([^,]+)((?:,[^,]+)*)\).*$")
        self.benchmarks = []
        has_test_info = False
        for line in f.readlines():
            if len(line.strip())>0:
                match_testsuite_info = testsuite_info.match(line)
                match_test_info = test_info.match(line)
                match_comment = comment.match(line) 
                if match_testsuite_info != None:
                    self.name = match_testsuite_info.group(1)
                    self.description = match_testsuite_info.group(2)
                    if match_testsuite_info.groups()>3 and len(match_testsuite_info.group(3))>0:
                        match_testsuite_info.group(3)[1:-1].split(",")
                elif match_test_info != None:
                    has_test_info = True
                    test_name = match_test_info.group(1)
                    test_description = match_test_info.group(2)
                    if match_test_info.groups()>3 and len(match_test_info.group(3))>0:
                        test_categories = match_test_info.group(3)[1:-1].split(",")
                    else:
                        test_categories = None
                elif match_comment == None: # data
                    if not has_test_info:
                        self.benchmarks.append(Benchmark(command=line))
                    else:
                        self.benchmarks.append(Benchmark(command=line,\
                                                         name=test_name,description=test_description,categories=test_categories))
                    has_test_info = False


class XMLNode:
    def __init__(self,name,attrs,data,children):
        self.name = name
        self.attrs = attrs
        self.data = data
        self.children = children
        
def getXML(node,ident=""):
    r = ident+"<"+node.name
    for k,v in node.attrs.iteritems():
        r += " "+k+"="+"'"+v+"'"
    r += ">"
    if len(node.children)>0:
        r += "\n"        
        cident = str(ident)+"    "
        for childNode in node.children:
            r += getXML(childNode,cident)
        r += ident
    elif node.data != None:
        if '\n' in str(node.data):
            r += "\n"
        r += str(node.data)
        
#    if len(str(node.data))>0:
 #       r+="\n"
    r += "</"+node.name+">\n"
    return r
           
def getBenchmarksXML(benchmarkFile,product,buildenv,runenv):    
    testsuite = XMLNode("testsuite",{"id": benchmarkFile.name},"",[])
    testsuite.children += [XMLNode("description",{},benchmarkFile.description,[])]
    testsuite.children += \
                    [XMLNode("product",{"id":"1"},"",
                        [ XMLNode("name",{},product['name'],[]),
                          XMLNode("version",{},product['version'],[]),
                          XMLNode("revision",{},product['revision'],[]),
                          XMLNode("note",{},product['note'],[]) ]),
                     XMLNode("buildenv",{"id":"1"},"",
                        [ XMLNode("compiler",{},buildenv['compiler'],[]),
                          XMLNode("stdlib",{},buildenv['stdlib'],[]),
                          XMLNode("buildflags",{},buildenv['flags'],[]) ]),
                     XMLNode("runenv",{"id":"1"},"",
                        [ XMLNode("os",{},runenv['os'],[]),
                          XMLNode("arch",{},runenv['arch'],[]),
                          XMLNode("processor",{},runenv['processor'],[]) ])]
    
    for b in benchmarkFile.benchmarks:
        testsuite.children +=[XMLNode("test",{"id":b.name},"",
                        [ XMLNode("description",{},b.description,[]),
                          XMLNode("cout",{},b.cout,[]),
                          XMLNode("cerr",{},b.cerr,[]),
                          XMLNode("secs",{},b.secs,[]),
                          XMLNode("kb",{},b.memkb,[]),
                          XMLNode("result",{},b.resultAsStr(),[]),
                          XMLNode("categories",{},"",[XMLNode("category",{},i,[]) for i in b.categories])])]

    return getXML(testsuite)        

## main function
def runBenchmarks(infilename,outfilename,sample_count,timeout,memout,product,buildenv,number_workers = cpu_count()):
    import platform    
    runenv = { "os" : " ".join(platform.system_alias(platform.system(),platform.release(),platform.version())),
               "arch" : platform.machine(),
               "processor" : platform.processor() }

    bf = BenchmarkFile(infilename)
    
    runqueue = RunQueue(number_workers)
    for b in bf.benchmarks:
        endTaskHandler = EndTaskHandler(b,runqueue,sample_count)
        for i in range(0,sample_count):
            runqueue.add(b,endTaskHandler,maxtime=timeout,maxmem=memout)
        
    runqueue.wait()    

    f = open(outfilename,"w")
    f.write(getBenchmarksXML(bf,product,buildenv,runenv))
    f.close()
               
if __name__ == '__main__':

    import argparse
    
    
    parser = argparse.ArgumentParser(description='Run a set of commands and store benchmarks')
    parser.add_argument('input_file', metavar='INFILE',\
                    help='name of file containing commands, one for each line')
    parser.add_argument('output_file', metavar='OUTFILE',\
                    help='name of file to output command output and stats')
    parser.add_argument('--sample-count', type=int,default=1,
                    help='number of times to sample each command')
#    parser.add_argument('--flush-interval',type=int,default=1,
#                    help='flush results to disk after processing this number of commands')
    parser.add_argument('--timeout', type=int,default=10,
                    help='max time for each command (in seconds)')
    parser.add_argument('--memout', type=int,default=900e6,
                    help='max memory for each command (in bytes)')
    parser.add_argument('--number-workers', type=int,default=cpu_count(),
                    help='number of commands to execute in parallel')
    
    parser.add_argument('--product-name',\
                    help='product name')
    parser.add_argument('--product-version',\
                    help='product version')
    parser.add_argument('--product-revision',\
                    help='product revision')
    parser.add_argument('--product-notes',\
                    help='product notes')
    parser.add_argument('--buildenv-compiler',\
                    help='compiler info')
    parser.add_argument('--buildenv-stdlib',\
                    help='stdlib info')
    parser.add_argument('--buildenv-flags',\
                    help='build flags')


    args = parser.parse_args()

    product = { "name": args.product_name,
                "version": args.product_version,
                "revision": args.product_revision,
                "note": args.product_notes }

    buildenv = { "compiler": args.buildenv_compiler,
                "stdlib": args.buildenv_stdlib,
                "flags": args.buildenv_flags,
                "note": args.product_notes }
    
    runBenchmarks(infilename=args.input_file,outfilename=args.output_file,
                  number_workers=args.number_workers,sample_count=args.sample_count,
                  timeout=args.timeout,memout=args.memout,product=product,
                  buildenv=buildenv)
 
    