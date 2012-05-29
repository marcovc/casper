 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2005-2011 - Marco Correia <marco.v.correia@gmail.com>                 *
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

#ifndef _H_CASPER_KERNEL_ENVIRONMENT
#define _H_CASPER_KERNEL_ENVIRONMENT

#include <casper/kernel/state/state.h>
//#include <casper/kernel/reversible/reversible.h>
#include <casper/util/debug.h>

//using namespace std;

namespace Casper {


// FIXME: setup macro that enables domain update counting

struct EnvStats
{
	Util::CPUTimer		cpuTimer;

	EnvStats() : cpuTimer("CPU time"){}

	const Util::CPUTimer& getCPUTimer() const {	return cpuTimer;	}
	Util::CPUTimer& getCPUTimer()  {	return cpuTimer;	}
};

struct Env
{
	Env(){}

	State& getState() { return state;	}
	const State& getState() const { return state;	}

	operator State&() { return state; }
	operator const State&() const { return state; }

	EnvStats& getStats() {	return stats;	}
	const EnvStats& getStats() const {	return stats;	}

	Util::IHeap& getHeap() { return state;	}
	const Util::IHeap& getHeap() const { return state;	}

	operator Util::IHeap&() { return state;	}
	operator const Util::IHeap&() const { return state;	}

	/*
#ifdef CASPER_LOG
	void log(const void* obj, const std::string& strObj, const Util::Logger::EventTag& tag)
	{ 	logger.log(obj,strObj,tag); }
#else
	void log(const void*, const std::string&, const Util::Logger::EventTag&)
	{}
#endif*/
	protected:
	State 			state;
	EnvStats		stats;
};



};

std::ostream& operator<<(std::ostream& os, const Casper::EnvStats& s);

#endif
