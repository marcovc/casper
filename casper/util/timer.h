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

#ifndef _H_CASPER_KERNEL_TIMER
#define _H_CASPER_KERNEL_TIMER

#include <string>
#include <vector>
#include <iostream>
#include <iomanip>
#include <casper/kernel/common.h>
#include <assert.h>
#include <time.h>

namespace Casper {
namespace Util {


struct ITimer
{
	string name;
	double secs;
	ITimer(string name, double secs) : name(name),secs(secs) {}
	ITimer(const ITimer& s) : name(s.name),secs(s.secs) {}
	virtual void reset() = 0;
	virtual void pause() = 0;
	virtual void resume() = 0;
	virtual bool running() const = 0;
	double getSecs() const { return secs;	}
	virtual void print(std::ostream& os) const = 0;
	virtual ~ITimer() {}
};

#if 0 // FIXME: this was measuring CPU time
// wall timer may be defined here since time.h is standard
struct WallTimer : ITimer
{
	clock_t tic,tac;	// could be local to 'pause()' ...
	bool isRunning;
	WallTimer(string name,bool running = true) :
		ITimer(name,0),isRunning(false)
	{ reset(); if (running) resume(); }
	WallTimer(const WallTimer& s) : ITimer(s),tic(s.tic),tac(s.tac),
									isRunning(s.isRunning) {}

	void reset()
	{	isRunning = false;	secs = 0; }
	void pause()
	{
		tac = clock();
		secs += (tac-tic)/static_cast<double>(CLOCKS_PER_SEC);
 		isRunning = false;
	}
	void resume()
	{
		isRunning = true;
		tic = clock();
	}
	bool running() const {	return isRunning;	}
	void print(std::ostream& os) const;
};

#endif
} // Util
} // Casper


#if defined(CASPER_NO_RT) and defined(__MACH__) 	// FIXME: not all compilers support this

#include <mach/mach_time.h>
#include <sys/time.h>

namespace Casper {
namespace Util {


#define CLOCK_MONOTONIC 1
#define CLOCK_PROCESS_CPUTIME_ID 1

static inline int clock_gettime(int, struct timespec *tp)

{
	static mach_timebase_info_data_t mtid;
	static struct timeval tv;
	static uint64_t first_mat;
	uint64_t elapsed_ns;

	if(!mtid.denom) {
		mach_timebase_info(&mtid);
		gettimeofday(&tv, NULL);
		first_mat = mach_absolute_time();
	}

	elapsed_ns = (mach_absolute_time() - first_mat) * mtid.numer / mtid.denom;
	tp->tv_sec = tv.tv_sec + elapsed_ns / 1000000000;
	tp->tv_nsec = tv.tv_usec*1000 + elapsed_ns % 1000000000;
	if(tp->tv_nsec >= 1000000000) {
		tp->tv_sec++;
		tp->tv_nsec -= 1000000000;
	}

	return 0;
}
} // Util
} // Casper

#endif

#if defined(CASPER_NO_RT) and defined(_WIN32)	// FIXME: not all compilers support this


#include <tchar.h>
#include <windows.h>

namespace Casper {
namespace Util {

struct CPUTimer : ITimer
{
	LARGE_INTEGER tic,tac,freq;
	bool isRunning;
	CPUTimer(string name,bool running = true) : ITimer(name,0),isRunning(false)
	{ reset(); if (running) resume();	}
	CPUTimer(const CPUTimer& s) : ITimer(s),tic(s.tic),tac(s.tac),
								freq(s.freq),isRunning(s.isRunning) {}
	void reset()
	{	isRunning = false;	secs = 0;	}
	void pause()
	{
		QueryPerformanceCounter(&tac);
		QueryPerformanceFrequency((LARGE_INTEGER *)&freq);
		secs += ((tac.QuadPart - tic.QuadPart) * 1.0 / freq.QuadPart);
		isRunning = false;
	}
	void resume()
	{
		isRunning = true;
		SetThreadAffinityMask(GetCurrentThread(), 1);
		QueryPerformanceCounter(&tic);
	}
	bool running() const {	return isRunning;	}
	void print(std::ostream& os) const;
};

// FIXME
/*struct WallClockTimer : CPUTimer
{
	WallClockTimer(string name,bool running = true) : CPUTimer(name,running) {}
	WallClockTimer(const WallClockTimer& s) : CPUTimer(s) {}
	void print(std::ostream& os) const;
};*/

} // Util
} // Casper
#else

namespace Casper {
namespace Util {


struct CPUTimer : ITimer
{
	struct timespec tic,tac;	// could be local to 'pause()' ...
	bool isRunning;
	CPUTimer(string name,bool running = true) : ITimer(name,0),isRunning(false)
	{ reset(); if (running) resume(); }
	CPUTimer(const CPUTimer& s) : ITimer(s),tic(s.tic),tac(s.tac),
								isRunning(s.isRunning) {}

	void reset()
	{	isRunning = false;	secs = 0; }
	void pause()
	{
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tac);
		secs += (((double)tac.tv_sec)+((double)tac.tv_nsec)*1E-9) -
 				(((double)tic.tv_sec)+((double)tic.tv_nsec)*1E-9);
 		isRunning = false;
	}
	void resume()
	{
		isRunning = true;
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tic);
	}
	bool running() const {	return isRunning;	}
	void print(std::ostream& os) const;
};

struct WallTimer : ITimer
{
	struct timespec tic,tac;	// could be local to 'pause()' ...
	bool isRunning;
	WallTimer(string name,bool running = true) : ITimer(name,0),isRunning(false)
	{ reset(); if (running) resume(); }
	WallTimer(const WallTimer& s) : ITimer(s),tic(s.tic),tac(s.tac),
								isRunning(s.isRunning) {}

	void reset()
	{	isRunning = false;	secs = 0; }
	void pause()
	{
		clock_gettime(CLOCK_MONOTONIC, &tac);
		secs += (((double)tac.tv_sec)+((double)tac.tv_nsec)*1E-9) -
 				(((double)tic.tv_sec)+((double)tic.tv_nsec)*1E-9);
 		isRunning = false;
	}
	void resume()
	{
		isRunning = true;
		clock_gettime(CLOCK_MONOTONIC, &tic);
	}
	bool running() const {	return isRunning;	}
	void print(std::ostream& os) const;
};
} // Util
} // Casper

#endif

namespace Casper {
namespace Util {

/// \deprecated Replace by one of CPUTimer or WallTimer
typedef CPUTimer Timer;

/*
struct Timer : Util::PImplIdiom<ITimer>
{
	typedef Timer						Self;
	typedef Util::PImplIdiom<ITimer> 	Super;
	Timer(ITimer*	f) : Super(f) {}
	Timer(string name,bool running = true) :
			Super(new DefaultTimer(name,running)) {}

	void reset() { Super::pImpl->reset(); }
	void pause() { Super::pImpl->pause(); }
	void resume() { Super::pImpl->resume(); }
	double getSecs() { return Super::pImpl->getSecs(); }
	bool running() const { return Super::pImpl->running(); }
};*/


/*
struct Timers
{
	std::vector<Timer>	ts;
	Timers() { newTimer("total",true); }
	Timer newTimer(string name, bool running = true)
	{
		ts.push_back(Timer(name));
		if (running)
			ts[ts.size()-1].resume();
		return ts[ts.size()-1];
	}
	void displayTimers()
	{
		ts[0].pause();
		cout << "------ Timers (secs) -------" << endl;
		for (unsigned int i = 0; i < ts.size(); ++i)
			cout << ts[i]
				 << " (" << ts[i].getSecs()*100.0/ts[0].getSecs() << "%)"
				 << endl;
	}
};*/

} // Util
} // Casper

std::ostream& operator<<(std::ostream& os, const Casper::Util::ITimer& t);

#endif /*_H_CASPER_KERNEL_TIMER*/
