/**************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2008-2008 - Marco Correia <marco.v.correia@gmail.com>                 *
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
#include <sstream>
#include <boost/config.hpp>

namespace casper {

unsigned int getVersionMajor()
{	return CASPER_VERSION_MAJOR;	}

unsigned int getVersionMinor()
{	return CASPER_VERSION_MINOR;	}

unsigned int getVersionRelease()
{	return CASPER_VERSION_RELEASE;	}

unsigned int getRevision()
{	return CASPER_REVISION;	}

std::string getVersion()
{
	std::stringstream ss;
	ss << CASPER_VERSION_MAJOR << "."
	   << CASPER_VERSION_MINOR << "."
	   << CASPER_VERSION_RELEASE;
	return ss.str();
}

std::string getVersionDetailed()
{
	std::stringstream ss;
	ss << getVersion()
	   << " (r" << getRevision() << ")"
	   << " compiled with " << getBuildCompiler()
	   << " on " << getBuildDate();
	return ss.str();
}

std::string getBuildCompiler()
{	return BOOST_COMPILER;	}

std::string getBuildStdlib()
{	return BOOST_STDLIB;	}

#define STR(x)   #x
#define XSTR(x)  STR(x)

std::string getBuildFlags()
{	return XSTR(CASPER_BUILDFLAGS);	}

std::string getBuildDate()
{	return std::string(__TIME__)+" on "+std::string(__DATE__) ;/*XSTR(CASPER_BUILDDATE);*/	}

}
