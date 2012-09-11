/***************************************************************************
 *   Copyright (C) 2007/2007 by Marco Correia                              *
 *   mvc@di.fct.unl.pt                                                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/


#ifndef _H_CASPER_UTIL_DEBUG
#define _H_CASPER_UTIL_DEBUG


//#include <casper/kernel/common.h>
#define CASPER_HASH_STD
//#include <casper/util/container/stdhashmap.h>
#include <casper/util/container/stdlist.h>
#include <casper/util/container/stdpair.h>
#include <casper/util/container/stdstack.h>
#include <casper/util/container/stdvector.h>

#include <casper/util/timer.h>

#include <unordered_map>
#include <list>
#include <vector>
#include <algorithm>

#include <unistd.h>

namespace Casper {
namespace Util {

#ifdef CASPER_LOG

#define CASPER_LOGGER_MAX_EVENT_STACK_DEPTH 100

struct Logger
{
/*	enum SingleEventTag
	{
		solverFail,
		filterSchedIterate,
		solverNotify,
		goalSchedIterate,
		solverPost
	};*/

	struct Event {
/*		virtual bool operator==(const Event& s) const
		{	return false;}
*/
		virtual std::string getInstance() const = 0;
		virtual std::string getType() const = 0;
		virtual std::string getClass() const = 0;
		virtual ~Event() {}
	};

	struct IntervalEvent : Event {
/*		virtual bool operator==(const Event& s) const
		{	return false;}
*/	};

	struct Callback {
		virtual void operator()(const Event&) = 0;
	};

	struct EventMatcher {
		virtual bool operator()(const Event&) const = 0;
		virtual EventMatcher* clone() const = 0;
	};

	struct AndMatcher : EventMatcher {
		EventMatcher* e1;
		EventMatcher* e2;
		AndMatcher(const EventMatcher& e1, const EventMatcher& e2) : e1(e1.clone()),e2(e2.clone()) {}
		~AndMatcher() {	delete e1; delete e2;	}
		EventMatcher* clone() const
		{ return new AndMatcher(*e1,*e2); }
		bool operator()(const Event& event) const
		{	return (*e1)(event) and (*e2)(event);	}
	};

	Logger() : intervalEventStack(CASPER_LOGGER_MAX_EVENT_STACK_DEPTH),
				curStackDepth(-1) {}
	void log(const Event& ev);
	// warning: ev must exist between logBegin and logEnd
	void logBegin(const IntervalEvent& ev) { intervalEventStack[++curStackDepth] = &ev; }
	void logEnd() { --curStackDepth; }
	void registerCallback(const EventMatcher& match, Callback& callback);
	void setInstrumentBit() { onLog = true; }
	void unsetInstrumentBit() { onLog = false; }
	const IntervalEvent* getCurEvent() const
	{ if (curStackDepth<0) return NULL; else return intervalEventStack[curStackDepth]; }
	typedef StdPair<EventMatcher*,Callback* > CallbackMapItem;
	typedef StdList<CallbackMapItem> CallbackMap;
	typedef StdVector<const IntervalEvent*>	IntervalEventStack;
	CallbackMap callbacks;
	bool onLog;
	IntervalEventStack intervalEventStack;
	int curStackDepth;
};

extern Logger logger;

struct ClassEventGenerator
{
	struct MethodEvent : Logger::IntervalEvent
	{
		MethodEvent(const void* pObj, const std::string& type, const std::string& clss, const std::string& method) :
			pObj(pObj),type(type),clss(clss),method(method) {}
		std::string getInstance() const
		{
			std::ostringstream oss;
			oss << getType() << "@" << pObj;
			return oss.str();
		}
		std::string getType() const
		{	return type + "::" + method; }
		std::string getClass() const
		{	return clss + "::" + method; }

		const void* pObj;
		const std::string type;
		const std::string clss;
		const std::string method;
	};

	void setInstrumentBit() { logger.setInstrumentBit(); }
	void unsetInstrumentBit() { logger.unsetInstrumentBit(); }
};
#endif


struct Alarm
{
	struct Callback {
		virtual void operator()(int signum) = 0;
	};

	static void attach(Callback* c)
	{
		callbacks.push_back(c);
	}

	static void start(uint usecs, bool repeat = false)
	{
		signal(SIGALRM,Alarm::sigHandler);
		if (!repeat)
			ualarm(usecs,0);
		else
			ualarm(usecs,usecs);
	}
	static void stop()
	{
		ualarm(0,0);
		signal(SIGALRM,SIG_DFL);
	}
	static void sigHandler(int signum)
	{
		for (auto it = callbacks.begin(); it != callbacks.end(); ++it)
			(**it)(signum);
	}

	static std::list<Callback*>	callbacks;
};

#ifdef CASPER_PROFILE


struct Profiler : Alarm::Callback
{

	typedef std::unordered_map<const Logger::IntervalEvent*,uint> Counts;
	typedef StdPair<std::string,uint> EventCount;
	typedef StdList<EventCount> EventCountList;

	void print(std::ostream& os, const EventCountList& l)
	{
		std::vector<StdPair<std::string,uint> > sl(l.begin(),l.end());
		std::sort(sl.begin(),sl.end(),
				[](const EventCount& i, const EventCount& j)
				{ return i.second > j.second or
					    (i.second==j.second and i.first<j.first); });
		for (auto it = sl.begin(); it != sl.end(); ++it)
			os << std::fixed << std::internal << std::setprecision(2) << std::setw(6) << (it->second*100.0/(countTotal-countOnLog)) << "%"
			   << " " << std::right << std::setw(10) << it->second
			   << " " << std::left << it->first << std::endl;
	}

	void print(std::ostream& os)
	{
		EventCountList listByInstance;
		for (auto mit = counts.begin(); mit != counts.end(); ++mit)
		{
			auto lit = listByInstance.begin();
			for ( ; lit != listByInstance.end(); ++lit)
				if (lit->first == mit->first->getInstance())
					break;
			if (lit!=listByInstance.end())
				lit->second += mit->second;
			else
				listByInstance.pushBack(EventCount(mit->first->getInstance(),mit->second));
		}
		listByInstance.pushBack(EventCount("<unknown>",countOnUnknown));

		EventCountList listByType;
		for (auto mit = counts.begin(); mit != counts.end(); ++mit)
		{
			auto lit = listByType.begin();
			for ( ; lit != listByType.end(); ++lit)
				if (lit->first == mit->first->getType())
					break;
			if (lit!=listByType.end())
				lit->second += mit->second;
			else
				listByType.pushBack(EventCount(mit->first->getType(),mit->second));
		}
		listByType.pushBack(EventCount("<unknown>",countOnUnknown));

		EventCountList listByClass;
		for (auto mit = counts.begin(); mit != counts.end(); ++mit)
		{
			auto lit = listByClass.begin();
			for ( ; lit != listByClass.end(); ++lit)
				if (lit->first == mit->first->getClass())
					break;
			if (lit!=listByClass.end())
				lit->second += mit->second;
			else
				listByClass.pushBack(EventCount(mit->first->getClass(),mit->second));
		}
		listByClass.pushBack(EventCount("<unknown>",countOnUnknown));

		os << "-----------------------------------\n";
		os << "Time spent on each event class    :\n";
		print(os,listByClass);
		os << "-----------------------------------\n";
		os << "Time spent on each event type     :\n";
		print(os,listByType);
		os << "-----------------------------------\n";
		os << "Time spent on each event instance :\n";
		print(os,listByInstance);
		os << "-----------------------------------\n";
		os << std::fixed << std::internal << std::setprecision(2) << std::setw(6) << "100.00%"
		   << " " << std::right << std::setw(10) << (countTotal-countOnLog)
		   << " " << std::left << "Total "
		   << "(log is " <<  (countOnLog*100.0/countTotal) << "% of total)" << std::endl;
	}

	Profiler() :
		countTotal(0),
		countOnLog(0),
		countOnUnknown(0)
	{
		Alarm::attach(this);
	}

	~Profiler()
	{
	}

	void operator()(int signum)
	{
		++countTotal;
		if (logger.onLog)
		{
			++countOnLog;
			return;
		}
		const Logger::IntervalEvent* pev = logger.getCurEvent();
		if (pev == NULL)
		{
			++countOnUnknown;
			return;
		}
		auto mit = counts.find(pev);
		if (mit == counts.end())
			counts[pev] = 1;
		else
			++mit->second;
	}
	uint countTotal;
	uint countOnLog;
	uint countOnUnknown;
	Counts counts;
};

extern Profiler profiler;

#endif


} // Util
} // Casper

//std::ostream& operator<<(std::ostream& s,const Casper::Util::Logger::Event& ev);
//std::ostream& operator<<(std::ostream& s,const Casper::Util::LogEventStats& ev);

#endif

