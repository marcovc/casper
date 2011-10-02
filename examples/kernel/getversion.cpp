/**************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2008-2011 - Marco Correia <marco.v.correia@gmail.com>                 *
 *                                                                         *
 *   Licensed under the Apache License, Version 2.0 (the "License");       *
 *   you may not use this file except in compliance with the License.      *
 *   You may obtain a copy of the License at                               *
 *            http://www.apache.org/licenses/LICENSE-2.0                   *
 *   Unless required by applicable law or agreed to in writing, software   *
 *   distributed under the License is distributed on an                    *
 *   "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,          *
 *   either express or implied.                                            *
 *   See the License for the specific language governing permissions and   *
 *   limitations under the License.                                        *
 \*************************************************************************/

#include <casper/version.h>
#include <iostream>
#include <string.h>

using namespace Casper;
using namespace std;

int main(int argc,char** argv)
{
	if (argc==1)
		cout << "CaSPER library build information :\n"
			 << "version  :\t" << getVersion() << endl
			 << "compiler :\t" << getBuildCompiler() << endl
			 << "stdlib   :\t" << getBuildStdlib() << endl
			 << "flags    :\t" << getBuildFlags() << endl
			 << "date     :\t" << getBuildDate()
			<< endl;
	else
	if (argc==2)
	{
		if (strcmp(argv[1],"--revision")==0)
			cout << getRevision() << endl;
		else
		if (strcmp(argv[1],"--version")==0)
			cout << getVersion() << endl;
		else
		if (strcmp(argv[1],"--major")==0)
			cout << getVersionMajor() << endl;
		else
		if (strcmp(argv[1],"--minor")==0)
			cout << getVersionMinor() << endl;
		else
		if (strcmp(argv[1],"--release")==0)
			cout << getVersionRelease() << endl;
		else
		if (strcmp(argv[1],"--compiler")==0)
			cout << getBuildCompiler() << endl;
		else
		if (strcmp(argv[1],"--stdlib")==0)
			cout << getBuildStdlib() << endl;
		else
		if (strcmp(argv[1],"--flags")==0)
			cout << getBuildFlags() << endl;
		else
		if (strcmp(argv[1],"--date")==0)
			cout << getBuildDate() << endl;
	}
	else
		cout << "usage: " << argv[0] << " [--major,--minor,--release,--revision,--compiler,--stdlib,--flags,--date]\n";
}

