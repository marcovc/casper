 ###########################################################################
 #   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   #
 #                                                                         #
 #   Copyright:                                                            #
 #   2007-2011 - Marco Correia <marco.v.correia@gmail.com>                 #
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
 
##################
##	EXAMPLES	##
##################

extra_example_libs={}

cp_examples=[]
cp_examples+=['int/golomb.cpp']
cp_examples+=['int/golfer.cpp']
cp_examples+=['int/langford.cpp']
cp_examples+=['int/allinterval.cpp']
#cp_examples+=['int/magic.cpp']
cp_examples+=['int/ortholatin.cpp']
cp_examples+=['int/latin.cpp']
#cp_examples+=['int/cripto.cpp']
cp_examples+=['int/labs.cpp']
cp_examples+=['int/codes.cpp']
cp_examples+=['int/nqueens.cpp']
#cp_examples+=['int/xp_nogoods.cpp']
#cp_examples+=['int/xp_domains.cpp']
#cp_examples+=['int/debug.cpp']

#cp_examples+=['real/equation.cpp']
#cp_examples+=['real/newton.cpp']
cp_examples+=['real/inverse.cpp']
extra_example_libs['examples/cp/real/inverse.cpp']=['gsl']


cp_examples+=['set/steiner.cpp']
cp_examples+=['set/golfer.cpp']
#cp_examples+=['set/debug.cpp']
#cp_examples+=['set/covering.cpp']
#cp_examples+=['set/packing.cpp']
#cp_examples+=['set/test.cpp']
#cp_examples+=['set/consistency.cpp']
cp_examples+=['set/hamming.cpp']
#cp_examples+=['set/bacp.cpp']
cp_examples+=['set/partition.cpp']


lp_examples=[]
lp_examples+=['debug.cpp']
lp_examples+=['tritype.cpp']
lp_examples+=['binsearch.cpp']

for i in lp_examples:
	extra_example_libs['examples/lp/'+i]=['glpk']

kernel_examples=[]
kernel_examples+=['debug.cpp']
#kernel_examples+=['goal_sched.cpp']
kernel_examples+=['getversion.cpp']

example_srcs=[]
for i in kernel_examples:
	example_srcs += ["examples/kernel/"+i]
for i in cp_examples:
	example_srcs += ["examples/cp/"+i]
for i in lp_examples:
	example_srcs += ["examples/lp/"+i]

examples_for_extra_lib={}
for k,v in extra_example_libs.iteritems():
	for i in v:
		if examples_for_extra_lib.has_key(i):
			examples_for_extra_lib[i].append(k)
		else:
			examples_for_extra_lib[i] = [k]
		
##################
##	LIBRARY		##
##################

casper_kernel=[]
casper_kernel+=['goal/goal.cpp']
casper_kernel+=['goal/explorer.cpp']
casper_kernel+=['goal/terminal.cpp']
casper_kernel+=['notify/susplist.cpp']
casper_kernel+=['state/state.cpp']
casper_kernel+=['state/env.cpp']

casper_cp=[]
casper_cp+=['filter/common.cpp']
casper_cp+=['filter/dom.cpp']
casper_cp+=['filter/bnd.cpp']
casper_cp+=['filter/val.cpp']
casper_cp+=['goal/heuristic.cpp']
casper_cp+=['scheduler.cpp']
casper_cp+=['store.cpp']
casper_cp+=['solver.cpp']

casper_util=[]
casper_util+=['container/stdrange.cpp']
casper_util+=['memory.cpp']
casper_util+=['debug.cpp']
casper_util+=['options.cpp']
casper_util+=['timer.cpp']
casper_util+=['util.cpp']

casper_lp=[]
casper_lp+=['driver.cpp']
casper_lp+=['solver.cpp']
casper_lp+=['var.cpp']
casper_lp+=['linearexprrepr.cpp']
casper_lp+=['lineareqrepr.cpp']


###################
##	BINDINGS 	 ##
###################

casperbind_cpp = []
casperbind_cpp+=['symbol.cpp']
casperbind_cpp+=['container.cpp']
casperbind_cpp+=['expression.cpp']
casperbind_cpp+=['predicate.cpp']
casperbind_cpp+=['index.cpp']
casperbind_cpp+=['print.cpp']
#casperbind_cpp+=['validate.cpp']
casperbind_cpp+=['visitor.cpp']
casperbind_cpp+=['solver.cpp']
casperbind_cpp+=['eval.cpp']

casperbind_cpp_tests = []
casperbind_cpp_tests += ['test.cpp']

casperbind_xcsp = []
casperbind_xcsp+= ['parser.cpp']
casperbind_xcsp+= ['exprparser.cpp']


##################
##	MINICASPER	 #
##################

minicasper=[]
minicasper+=['kernel.cpp']
minicasper+=['expression.cpp']
minicasper+=['operator.cpp']
minicasper+=['int.cpp']
minicasper+=['instance.cpp']

# experimental stuff
minicasper_phd=[]
minicasper_phd+=['sac/constraintgraph.cpp']

#########################
##	MINICASPER-CSPXML  ##
#########################

minicasper_cspxml=[]
minicasper_cspxml+=['converter.cpp']
minicasper_cspxml+=['cspxmlinstance.cpp']
minicasper_cspxml+=['symgraph.cpp']
minicasper_cspxml+=['system.cpp']
minicasper_cspxml+=['main.cpp']

######################################################################
######################################################################
# DON'T EDIT FROM THIS POINT BELOW UNLESS YOU KNOW WHAT YOU'RE DOING #
######################################################################
######################################################################

import os


########### distribution info ###########

casper_version_major = 0
casper_version_minor = 4
casper_version_release = 0

casper_version = str(casper_version_major)+'.'+\
				 str(casper_version_minor)+'.'+\
				 str(casper_version_release)
casper_cspxml_version = '1.0'
casper_license = 'Apache-2.0'
casper_vendor = 'N/A'
x_msi_language = 1033
x_msi_license_text = 'LICENSE Text' 

casper_url = 'http://proteina.di.fct.unl.pt/casper'
casper_src_url = casper_url+'/casper-'+casper_version+'.tar.gz'

casper_summary = 'Constraint Solving Platform for Engineering and Research'

casper_description = '''
CaSPER is a C++ library for generic constraint solving which aims to be efficient, extendible and easy to understand.

The library follows a set of design patterns proved useful for constraint solving such as event-driven execution, callback scheduling, garbage collection, and trail-aware generic data structures. Additionally, it departs from standard black box solvers by featuring custom propagator scheduling, efficient domain delta information availability, and an expressive modeling interface available directly in the host programming language.

Domain specific reasoning extends the kernel in a modular fashion - currently there are modules for finite domain variables, finite set domain variables, graph domain variables, generic interval-based reasoning and for 3d space reasoning.

Having being idealized to accommodate a quickly changing research environment, the library's design is utterly committed to flexibility and openness. The implementation is based on the generic programming idiom which has been proved successful for achieving this goal.
'''

########### find all sources ###########

library_headers = [] 
for root,dir,files in os.walk("casper"):
	if '.svn' not in root and 'minicasper' not in root:
		filelist = [ os.path.join(root,fi) for fi in files if (fi.endswith(".h") or fi.endswith(".hpp")) ]
		for f in filelist: 
			library_headers.append(f)

library_cpps = []
for root,dir,files in os.walk("casper"):
	if '.svn' not in root and 'minicasper' not in root:        
		filelist = [ os.path.join(root,fi) for fi in files if fi.endswith(".cpp") ]
		for f in filelist: 
			library_cpps.append(f)
              
library_sources = library_headers+library_cpps

example_sources = []
for root,dir,files in os.walk("examples"):
	if '.svn' not in root:        
		filelist = [ os.path.join(root,fi) for fi in files ]
		for f in filelist: 
			example_sources.append(f)

cspxml_sources = []
for root,dir,files in os.walk("minicasper"):
	if '.svn' not in root:        
		filelist = [ os.path.join(root,fi) for fi in files ]
		for f in filelist: 
			cspxml_sources.append(f)


extra_files_info = ['AUTHORS','LICENSE','NOTICE','README','REVISION']
extra_files = ['Doxyfile','SConstruct','.project','.cproject']+extra_files_info
for root,dir,files in os.walk("site_scons"):
	if '.svn' not in root:        
		filelist = [ os.path.join(root,fi) for fi in files ]
		for f in filelist: 
			extra_files.append(f)


dist_sources = library_sources+example_sources+extra_files

############ support for build flags ############

defined_macros = []
build_dir_prefix = '.'

vars = Variables()
vars.Add(EnumVariable('debug_level', '0 (none) to 3 (full)', '0',
						allowed_values=('0','1', '2','3')), converter=int)
vars.Add(EnumVariable('optimize_level', '0 (none) to 3 (full)', '0',
						allowed_values=('0','1', '2','3')), converter=int)
vars.Add(BoolVariable('warnings', 'compilation with -Wall', True))
vars.Add(BoolVariable('profile', 'build for profiling with gprof/gcov', False))
vars.Add(BoolVariable('safe_rounding', 'perform safe rounded operations with float types', 1))
vars.Add(BoolVariable('asserts', 'extra run-time checks', False))
vars.Add(BoolVariable('log', 'enable builtin debugger', False))
vars.Add(BoolVariable('static_link', 'static link', False))
vars.Add(BoolVariable('cpp0x', 'enable c++0x extensions', True))
vars.Add(BoolVariable('lp', 'builds interface to glpk (requires preinstalled glpk library)', True))
vars.Add(PathVariable('install_prefix', 'installation prefix', '.'))
vars.Add(PathVariable('boost_path','path where boost libraries are installed',None))
vars.Add(PathVariable('gmp_path','path where gmp library is installed',None))
vars.Add(PathVariable('mpfr_path','path where mpfr library is installed',None))


compiler = ARGUMENTS.get('tool', 'default')   #allows user defined compiler


vars.options[0].converter = int
vars.options[1].converter = int

AddOption('--notes',
                  dest='notes',
                  type='string',
                  nargs=1,
                  action='store',
                  metavar='NOTES',
                  help='notes associating with current code instance (stored with test results)')


############ support for building on multiple platforms ############

import sys

# user defined environment
env = Environment(ENV=os.environ,
                  variables = vars,
				  CPPPATH=['#'],
				  CPPDEFINES = defined_macros,				  
				  CXXFILESUFFIX='.cpp'
				 )

env.EnsureSConsVersion(1, 2, 0)
 
env['LIBPATH'] = []

# example of cross-compiling to 64 bits (TODO: put this in site_scons)
# env['ENV']['PATH']='/usr/x86_64-pc-linux-gnu:/usr/x86_64-pc-linux-gnu/lib'+env['ENV']['PATH']
# env['CXX']='/usr/i686-pc-linux-gnu/x86_64-pc-linux-gnu/gcc-bin/4.4.2/x86_64-pc-linux-gnu-g++'
# env['CC']='/usr/i686-pc-linux-gnu/x86_64-pc-linux-gnu/gcc-bin/4.4.2/x86_64-pc-linux-gnu-gcc'
# env['AR'] = '/usr/i686-pc-linux-gnu/x86_64-pc-linux-gnu/binutils-bin/2.20/ar'
# env['CPPPATH'] += ['/usr/x86_64-pc-linux-gnu/usr/include/','/usr/include']
# env['LIBPATH'] += ['/usr/x86_64-pc-linux-gnu/usr/lib/','/usr/x86_64-pc-linux-gnu/lib/']

if env.has_key('boost_path'):
	env['CPPPATH'] += [env['boost_path']]
if env.has_key('gmp_path'):	
	env['CPPPATH'] += [env['gmp_path']+"/lib/"]	
	env['LIBPATH'] += [env['gmp_path']+"/lib/"]
if env.has_key('mpfr_path'):	
	env['CPPPATH'] += [env['mpfr_path']+"/lib/"]
	env['LIBPATH'] += [env['mpfr_path']+"/lib/"]
 
if compiler!='default':
	Tool(compiler)(env)

Tool('packaging')(env)

# common configure tests
confCommonEnv = env.Clone()
confCommon = Configure( confCommonEnv ) 
if not confCommon.CheckLib( library='rt', symbol='clock_gettime',autoadd=1):
	confCommon.env.Append(CPPDEFINES = ['CASPER_NO_RT'])
if not env['safe_rounding'] or \
	not confCommon.CheckLib( library='mpfr', autoadd=1) or \
	not confCommon.CheckLib( library='gmp', autoadd=1):
	confCommon.env.Append(CPPDEFINES = ['CASPER_UNSAFE_ROUNDING'])
if env['profile']:
	env['profile'] = confCommon.CheckLib( library='gcov', autoadd=1)	
	if not env['profile']:
		print "Warning: gcov library not found. No code coverage available."
confCommon.CheckLib(library='boost_program_options',language='C++', autoadd=1)
for (k,v) in examples_for_extra_lib.iteritems():
	if not confCommon.CheckLib( library=k, autoadd=0):
		print "Warning: library "+k+" not found. Examples "+v+" will not be built."
		for i in v: 
			example_srcs.remove(v)
confCommon.Finish();

if not env['lp']:
	for i in examples_for_extra_lib['glpk']:
		example_srcs.remove(i)
	
env.Append(CPPDEFINES = confCommonEnv['CPPDEFINES'])

if not confCommonEnv.has_key('LIBPATH'):
	confCommonEnv['LIBPATH']=[]

if env['debug_level'] == 0 and env['asserts'] == 0 :
	defined_macros += ['NDEBUG']

if env['log']:
	defined_macros += ['CASPER_LOG']
	
if env['cpp0x']:
	defined_macros += ['CASPER_CPP0X']
	

env.Append(CPPDEFINES = defined_macros)


def getBuildFlags(env,debug_level,optimize_level):
	build_flags = []
	link_flags = []
	# this is surely not the better way of knowing which compiler we are using
	if env["CC"]=='cl':
		if debug_level>0:
			build_flags += ['/GS','/Zi']
			link_flags += ['/DEBUG']
		if optimize_level==1:
			build_flags += ['/O1']
		elif optimize_level==2:
			build_flags += ['/O2']
		elif optimize_level==3:
			build_flags += ['/Ox']
		if env['warnings']:
			build_flags += ['/W3']
		build_flags += ['/EHsc','/DNOMINMAX','/FC']
	else: # assume the compiler takes gcc options
		build_flags += ['-g${debug_level}','-O${optimize_level}']
		if env['warnings']:
			build_flags += ['-Wall']
		if env['profile']:
			build_flags += ['-fprofile-arcs','-ftest-coverage','-pg']
			link_flags += ['-pg']
		if env['static_link']:
			build_flags += ['-static']
			link_flags += ['-static']
		if env['cpp0x']:
			build_flags += ['-std=gnu++0x']
		build_flags += ['-Wfatal-errors']
		#build_flags += ['-ffast-math']
		#link_flags += ['-ffast-math']	
		#factor = 64
		#inlineLimit=1000*factor
		#unitGrowth = 50*factor
		#functionGrowth = 100*factor
		#build_flags += ['-fno-inline-functions']
		#build_flags += ['-finline-limit='+str(inlineLimit),\
		#				'--param','large-function-growth='+str(functionGrowth),\
		#				'--param','inline-unit-growth='+str(unitGrowth)]
		#build_flags += ['-finline-limit=1000'] ,'--param','large-function-growth=1000000','--param','large-unit-insns=1000000']
		#build_flags += ['-fvisibility=hidden','-fPIC','-pipe','-fno-strict-aliasing','-finline-limit=3000','-ffast-math']
		#link_flags += ['-fvisibility=hidden','-fPIC','-pipe','-fno-strict-aliasing','-finline-limit=3000','-ffast-math']
	return (build_flags,link_flags)

# input environment
(build_flags,link_flags)=getBuildFlags(env,env['debug_level'],env['optimize_level'])
env.Append(CCFLAGS = build_flags)
env.Append(LINKFLAGS = link_flags)

# environment with full debug (for msvs)
denv = env.Clone()
(dbuild_flags,dlink_flags)=getBuildFlags(denv,3,0)
denv.Append(CCFLAGS = dbuild_flags)
denv.Append(LINKFLAGS = dlink_flags)

# environment with full optimizations (for msvs)
renv = env.Clone()
(rbuild_flags,rlink_flags)=getBuildFlags(renv,0,3)
renv.Append(CCFLAGS = rbuild_flags)
renv.Append(LINKFLAGS = rlink_flags)

import commands
import time
import re

# try to guess current revision, first from git and then consulting the REVISION file
def getRevision():
	revision = -1
	(gitdescribe_status,gitdescribe_output) = commands.getstatusoutput("git describe --always")
	if gitdescribe_status==0:
		revision = gitdescribe_output
	if revision==-1:
		f=open("REVISION", "r")
		revision = f.readline()[:-1]
	return revision

casper_revision=getRevision()
env.Command(File("REVISION"), [Value(casper_revision)], 'echo '+casper_revision+' > REVISION')
		
build_str = build_flags[0]
for i in range(1,len(build_flags)):
	build_str += " "+build_flags[i]
version_macros = defined_macros+\
				[("CASPER_VERSION_MAJOR",str(casper_version_major))]+ \
				[("CASPER_VERSION_MINOR",str(casper_version_minor))]+ \
				[("CASPER_VERSION_RELEASE",str(casper_version_release))]+ \
				[("CASPER_REVISION",str(casper_revision))]+ \
				[("CASPER_BUILDFLAGS",build_str)]
				#+ \
				#[("CASPER_BUILDDATE",time.asctime())]

############ generate the help message ############

Help("""
usage: scons [options] target

targets:
--------

library             builds library		
examples            builds all examples (and library)
minicasper-cspxml   builds the cspxml solver executable
msvcsolution        creates msvs projects to use with msvs IDE
package             creates packages with the current CaSPER source and binary distribution
   	
options:
--------
""")

Help(vars.GenerateHelpText(env))
       
############ support for preprocessing macro headers ############

import fnmatch
import os
import string

def RecursiveGlob(rootdir,pattern):
	matches = []
	for root, dirnames, filenames in os.walk(rootdir):
	  for filename in fnmatch.filter(filenames, pattern):
	      matches.append(os.path.join(root, filename))
	return matches

macros = RecursiveGlob("casper/","*.m")
macros_h = [string.replace(i,".m",".h") for i in macros]
#env.NoClean(macros_h)

def preprocessMacros(env):
	env.Append(CPPSUFFIXES='.m')

	macroHBuilder = Builder(single_source=1,
						src_suffix='.m',
						suffix='.h',
						action="$CC  $_CPPINCFLAGS -x c++ -P -CC -E $SOURCE -o $TARGET")

	env.Append(BUILDERS = {'MacroHBuilder':macroHBuilder} )

	# TODO: find .m files automatically
	macro_targets=[]
	for i in macros:
		macro_targets+=env.MacroHBuilder(i)
 
	Alias("macros",macro_targets)


if env["CC"].find('gcc')>=0:
	preprocessMacros(env)


############ support for debug/release variant builds ############

SUFFIX=""
if env['debug_level'] > 0:
	MODE="debug/"
else:
	MODE="release/"

Export("env")

env['PREFIX']='build/'+MODE
env['ID'] = 0
env.VariantDir(env['PREFIX'], '.', duplicate=0)

denv['PREFIX']='msvs-ide/build/debug/'
denv['ID'] = 1
denv.VariantDir(denv['PREFIX'], '.', duplicate=0)
renv['PREFIX']='msvs-ide/build/release/'
renv['ID'] = 2
renv.VariantDir(renv['PREFIX'], '.', duplicate=0)

############ support for building library ############

def defineVersionObj(env):
	return env.Object(env['PREFIX']+"/casper/version.cpp",CPPDEFINES=version_macros) 
	
versionObjs = {}
versionObjs[env['ID']] = defineVersionObj(env)
versionObjs[renv['ID']] = defineVersionObj(renv)
versionObjs[denv['ID']] = defineVersionObj(denv)

casper_srcs=[]
for i in casper_kernel:
	casper_srcs+=["casper/kernel/"+i]
for i in casper_cp:
	casper_srcs+=["casper/cp/"+i]
for i in casper_util:
	casper_srcs+=["casper/util/"+i]
if env['lp']:
	for i in casper_lp:
		casper_srcs+=["casper/lp/"+i]

def defineLibrary(env):
	casper_objs=[]

	casper_objs += versionObjs[env['ID']]
	
	for i in casper_srcs:
		casper_objs+=env.Object(env['PREFIX']+"/"+i)

	return env.Library(target=env['PREFIX']+"/casper/casper",source=casper_objs)
	
libcasper=defineLibrary(env)	
Alias('library',libcasper)

if env['debug_level']==0 and env['optimize_level']==3:
	rlibcasper=libcasper
else:
	rlibcasper=defineLibrary(renv)

if env['debug_level']==3 and env['optimize_level']==0:
	dlibcasper=libcasper
else:
	dlibcasper=defineLibrary(denv)

############ support for building examples ############

example_libs=['casper']
libpath = '/casper'
	
example_libs+=confCommonEnv['LIBS']
	
def defineExamples(env):
	example_targets=[]
	for i in example_srcs:
		extra_libs = []
		if extra_example_libs.has_key(i):
			extra_libs = extra_example_libs[i]
		example_targets += env.Program(env['PREFIX']+'/'+i,
						 LIBPATH=[env['PREFIX']+libpath]+confCommonEnv['LIBPATH'],
						   LIBS=example_libs+extra_libs)
	return example_targets

example_targets=defineExamples(env)	
Alias('examples',example_targets)

dexample_targets = defineExamples(denv)
rexample_targets = defineExamples(renv)

############ support for building minicasper library ############

minicasper_srcs=[]
for i in minicasper:
	minicasper_srcs+=["minicasper/"+i]

minicasper_objs=[]

#add casper sources
minicasper_objs+=versionObjs[env['ID']]

for i in casper_srcs:
	minicasper_objs+=env.Object(env['PREFIX']+"/"+i)
	
for i in minicasper_srcs:
	minicasper_objs+=env.Object(env['PREFIX']+"/"+i,
				CPPPATH=['#','../../phd/src/'])

#add external (phd) sources (TEMPORARY)
for i in minicasper_phd:
	minicasper_objs += env.Object('../../phd/src/'+i,
				CPPPATH=['#','../../phd/src/'])

libminicasper=env.Library(target=env['PREFIX']+"/minicasper/minicasper",source=minicasper_objs)

Alias('minicasper',libminicasper)

### support for building bindings ***

# bindings_cpp library

gen_casper_preds = env.Command('bindings/cpp/casperpreds.h',None,'python bindings/cpp/genpreds.py > $TARGET')

casperbind_cpp_srcs = []
for i in casperbind_cpp:
	casperbind_cpp_srcs+=["bindings/cpp/"+i]

casperbind_cpp_objs=[]
for i in casperbind_cpp_srcs:
	casperbind_cpp_objs += env.Object(env['PREFIX']+"/"+i,
									CPPPATH=['#'])
	
libcasperbind_cpp=env.Library(target=env['PREFIX']+"/bindings/casperbind_cpp",source=casperbind_cpp_objs)

Alias('casperbind_cpp',[gen_casper_preds,libcasperbind_cpp])

# bindings_cpp tests

casperbind_cpp_tests_srcs = []
for i in casperbind_cpp_tests:
	casperbind_cpp_tests_srcs+=["bindings/cpp/"+i]

casperbind_cpp_tests_targets=[]
for i in casperbind_cpp_tests_srcs:
	casperbind_cpp_tests_targets += env.Program(env['PREFIX']+"/"+i,
										CPPPATH=['#'],
										LIBPATH=[env['PREFIX']+'bindings/cpp/'],
						   				LIBS=libcasperbind_cpp)

Alias('casperbind_cpp_tests',casperbind_cpp_tests_targets)

# bindings_xcsp exec

casperbind_xcsp_srcs = []
for i in casperbind_xcsp:
	casperbind_xcsp_srcs+=["bindings/xcsp/"+i]


casperbind_xcsp_objs=[]
for i in casperbind_xcsp_srcs:
	casperbind_xcsp_objs += env.Object(env['PREFIX']+"/"+i,
									CPPPATH=['#src','/usr/include/libxml++-2.6',
												 '/usr/include/glibmm-2.4',
												 '/usr/include/glib-2.0',
												 '/usr/lib/glib-2.0/include',
												 '/usr/lib/glibmm-2.4/include/',
												 '/usr/lib/libxml++-2.6/include/'])

# TODO: find correct include and link paths automatically  
casperbind_xcsp_target = env.Program(env['PREFIX']+"/bindings/xcsp/casper-xcsp",
										source=casperbind_xcsp_objs,
										LIBS=libcasperbind_cpp+['xml++-2.6'])

Alias('casperbind_xcsp',casperbind_xcsp_target)


### support for building fzn-casper ###

casperbind_fzn_target_libpath = confCommonEnv['LIBPATH']
casperbind_fzn_target_libs = confCommonEnv['LIBS']+[libcasperbind_cpp]+[libcasper]
							     
casperbind_fzn_target = env.Program(env['PREFIX']+'/bindings/fzn/fzn-casper',
									[env['PREFIX']+'/bindings/fzn/parser.yy',
									  env['PREFIX']+'/bindings/fzn/lexer.ll',
									  env['PREFIX']+'/bindings/fzn/driver.cpp',
									  env['PREFIX']+'/bindings/fzn/main.cpp'],
									  CPPPATH=['#src','#src/bindings/fzn','#src/bindings/cpp',
									  '#build/'+MODE+'/bindings/fzn'],
									  LIBS=casperbind_fzn_target_libs,
									  LIBPATH=casperbind_fzn_target_libpath)

Alias('casperbind_fzn',casperbind_fzn_target)

### support for building py-casper ###

pycasper_ifaces=['main.i']

pycasper_objs=[]
for i in pycasper_ifaces:
	pycasper_objs += env.SharedObject(env['PREFIX']+"/bindings/py/"+i,
									 CCFLAGS=' -fPIC -Wall -std=c++0x',
									 CPPPATH=['#src','/usr/include/python2.6'],
									 SWIGFLAGS = '-c++ -python -Isrc -Wall');

pycasper_target_libpath = confCommonEnv['LIBPATH']
pycasper_target_libs = confCommonEnv['LIBS']+[libcasperbind_cpp]+[libcasper]

pycasper_lib=env.SharedLibrary(target=env['PREFIX']+'/bindings/py/casper',
							   source=pycasper_objs,SHLIBPREFIX='_',
							   LIBPATH=pycasper_target_libpath,
							   LIBS=pycasper_target_libs)

Alias('casperbind_py',pycasper_lib)
							   
### support for building minicasper-cspxml ***

# if we are cross-compiling
if compiler=='crossmingw':
	libxml2_incpath = '#libxml2/include'
	libxml2_libpath = '#libxml2/lib'
	libiconv_incpath = '#iconv/include'
	libiconv_libpath = '#iconv/lib'
	minicasper_cspxml_libs=['minicasper','libxml2']
else:
	libxml2_incpath = '/usr/include/libxml2'
	libxml2_libpath = '/usr/lib'
	libiconv_incpath = '/usr/include'
	libiconv_libpath = '/usr/lib'
	minicasper_cspxml_libs=['minicasper','xml2']


minicasper_cspxml_libs+=confCommonEnv['LIBS']

minicasper_cspxml_libpath = [env['PREFIX']+'/minicasper',\
							libxml2_libpath,libiconv_libpath]+\
							confCommonEnv['LIBPATH'] 
minicasper_cspxml_objs=[]

# saucy src
minicasper_cspxml_objs += env.Object(env['PREFIX']+"casper/thirdparty/saucy-1.1/saucy.cpp",
										 CPPPATH=['#src'])
	
for i in minicasper_cspxml:
	minicasper_cspxml_objs += env.Object(env['PREFIX']+"minicasper/bindings/xml/"+i,
										 CPPPATH=['#src',libxml2_incpath,libiconv_incpath])
										 
minicasper_cspxml_exe=env.Program(target=env['PREFIX']+'minicasper/bindings/xml/minicasper-cspxml',
							   source=minicasper_cspxml_objs,
							   LIBPATH=minicasper_cspxml_libpath,
							   LIBS=minicasper_cspxml_libs)

Alias('minicasper-cspxml',minicasper_cspxml_exe)

############ general environment information ############

#benchmark_target=env.Program(env['PREFIX']+'other/benchmark/benchmark.cpp',
#							LIBPATH=[env['PREFIX']+'/casper'],
#							LIBS=example_libs+['boost_filesystem'])

import platform
platform_info = platform.uname()
SYSFLAGS = '-DBUILD_FLAGS=\'\"'+str(build_flags)+'\"\' \
			-DCASPER_REVISION='+str(casper_revision)

############ support for testing ###############

import subprocess

getversionExec="build/release/examples/kernel/getversion"

def getVersionInfo(arg):
	p = subprocess.Popen([getversionExec,arg],shell=False,stdout=subprocess.PIPE)
	(sout,serr) = p.communicate()
	return sout.replace('\r','').replace('\n','')
	
benchExec = "/home/marco/projects/benchmark/benchmark"

def runTests(env,target,source):
	benchArgs = ["--product-name=CaSPER",\
				"--product-version="+getVersionInfo("--version"),\
				"--product-revision="+getVersionInfo("--revision"),\
				"--buildenv-compiler="+getVersionInfo("--compiler"),\
				"--buildenv-stdlib="+getVersionInfo("--stdlib"),\
				"--buildenv-flags="+getVersionInfo("--flags"),\
				"--timeout=30",\
				"--memout=900000000",\
				"--sample-count=3"]
	if GetOption('notes')!=None:
		benchArgs += ["--product-note="+GetOption('notes')]
	cmd = [source[0].abspath]+benchArgs
	if not subprocess.call(cmd):
		import testcmp
		(couteq,cerreq,tgavg,mgavg,favg) = testcmp.compare("test/BenchmarkResults.xml.correct","test/BenchmarkResults.xml")
		if not couteq:
			print "FAILED: different stdout!"
		if not cerreq:
			print "FAILED: different stderr!"
		print "avg time ratio of newfile/oldfile:",tgavg
		print "avg mem ratio of newfile/oldfile:",mgavg
		print "ratio of problems solved faster in newfile:",favg
		
testCmd = Command("tests.passed",benchExec,runTests)
Depends(testCmd,example_targets+[File(benchExec)])

testing_target = Alias("test",testCmd)


############ support for creating msvc solution ############


def defineMSVSLibProject(env,prefix):
	return env.MSVSProject(target = prefix+'library' + env['MSVSPROJECTSUFFIX'],
	                          srcs = msvcproj_lib_srcs,
	                          incs = [],
	                          localincs = library_headers,
	                          resources = [],
	                          misc = [],
	                          buildtarget = Flatten([rlibcasper,dlibcasper]),
							  variant = ['Release','Debug'],
							  auto_build_solution=0)
				
def defineMSVSExampleProject(env,prefix,src,trg):							
	return env.MSVSProject(target = (prefix+'example-'+src.replace("/","-"))[:-4] + env['MSVSPROJECTSUFFIX'],
	                        srcs = "../src/examples/"+src,
	                        incs = [],
	                        localincs = [],
	                        resources = [],
	                        misc = [],
	                        buildtarget = trg,
							variant = ['Release','Debug'],
						    auto_build_solution=0)
				  							  
if env.has_key('MSVSPROJECTCOM'):
	msvcproj_lib_srcs=[]
	for i in casper_srcs:
		msvcproj_lib_srcs+=["../src/"+i]

	msvcproj_lib_target_prefix = defineMSVSLibProject(env,'msvs-ide/')
	msvcproj_lib_target_noprefix = defineMSVSLibProject(env,'')

	msvcproj_example_targets_prefix=[]
	msvcproj_example_targets_noprefix=[]
							  							  
	for i,j,k in zip(examples,rexample_targets,dexample_targets):
		msvcproj_example_targets_prefix.append(defineMSVSExampleProject(env,'msvs-ide/',i,Flatten([j,k])))
		msvcproj_example_targets_noprefix.append(defineMSVSExampleProject(env,'',i,Flatten([j,k])))
	
	msvcsolution_target=env.MSVSSolution(target = 'msvs-ide/CaSPER' + env['MSVSSOLUTIONSUFFIX'],
					 projects = [msvcproj_lib_target_noprefix,msvcproj_example_targets_noprefix],
					variant = ['Release','Debug']) 
									
	Alias("msvcsolution",[msvcproj_lib_target_prefix,msvcproj_example_targets_prefix,msvcsolution_target])

### installing ###

common_install_target = []
for i in extra_files_info:
	common_install_target.append(env.InstallAs(target=env['install_prefix']+'/share/casper/'+i,source = i))

lib_install_target = []
lib_install_target += common_install_target
for i in library_headers:
	lib_install_target.append(env.InstallAs(target = env['install_prefix']+'/include/'+i[4:],source = i))
lib_install_target.append(env.Install(target=env['install_prefix']+'/lib/',source = libcasper))

for i in Flatten(example_targets):
	(dir1,filename) = os.path.split(str(i))
	(dir2,eval) = os.path.split(dir1)
	lib_install_target.append(env.InstallAs(target=env['install_prefix']+'/bin/casper-'+eval+"-"+filename,source = i))
	lib_install_target.append(env.InstallAs(target=env['install_prefix']+'/share/casper/examples/'+eval+"-"+filename,source = i))

cspxml_install_target = []
cspxml_install_target += common_install_target
cspxml_install_target.append(env.InstallAs(target=env['install_prefix']+'/bin/casper-cspxml',source = minicasper_cspxml_exe))

if compiler=='crossmingw':	# windows requires libxml dlls
	cspxml_install_target.append(env.Install(target=env['install_prefix']+'/bin/',source = '#libxml2/bin/libxml2.dll'))
	cspxml_install_target.append(env.Install(target=env['install_prefix']+'/bin/',source = '#iconv/bin/iconv.dll'))
	cspxml_install_target.append(env.Install(target=env['install_prefix']+'/bin/',source = '#zlib/bin/zlib1.dll'))
else:
	cspxml_install_target.append(env.Install(target=env['install_prefix']+'/bin/',source = '/usr/lib/libxml2.so'))
	cspxml_install_target.append(env.Install(target=env['install_prefix']+'/bin/',source = '/usr/lib/libz.so'))
	
Alias('library-install',lib_install_target)	
Alias('cspxml-install',cspxml_install_target)
              


### packaging ###

dist_sources.append(File('REVISION'))

package_libsrc_dist = env.Package( 
			 source			= dist_sources,
			 NAME           = 'casper-src',
             SUMMARY		= casper_summary,
             DESCRIPTION    = casper_description,
             VERSION		= casper_version,
             LICENSE		= casper_license,
             VENDOR			= casper_vendor,
			 X_MSI_LANGUAGE = x_msi_language,
             X_MSI_LICENSE_TEXT = x_msi_license_text,
             PACKAGEVERSION =  0,
             X_RPM_GROUP    = 'Application/fu',
             SOURCE_URL     = casper_src_url)
         

package_libbin_dist = env.Package( 
			 source			= lib_install_target,
			 NAME           = 'casper',
             SUMMARY		= casper_summary,
             DESCRIPTION    = casper_description,
             VERSION		= casper_version,
             LICENSE		= casper_license,
             VENDOR			= casper_vendor,
			 X_MSI_LANGUAGE = x_msi_language,
             X_MSI_LICENSE_TEXT = x_msi_license_text,
             PACKAGEVERSION =  0,
             X_RPM_GROUP    = 'Application/fu',
             SOURCE_URL     = casper_src_url)

package_cspxmlbin_dist = env.Package( 
			 source			= cspxml_install_target,
			 NAME           = 'casper-cspxml',
             SUMMARY		= casper_summary,
             DESCRIPTION    = casper_description,
             VERSION		= casper_cspxml_version,
             LICENSE		= casper_license,
             VENDOR			= casper_vendor,
			 X_MSI_LANGUAGE = x_msi_language,
             X_MSI_LICENSE_TEXT = x_msi_license_text,
             PACKAGEVERSION =  0,
             X_RPM_GROUP    = 'Application/fu',
             SOURCE_URL     = casper_src_url)

