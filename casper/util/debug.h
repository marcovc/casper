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

#include <casper/util/timer.h>

namespace Casper {
namespace Util {


struct Logger
{
	enum EventTag
	{
		solverFail,
		filterSchedIterate,
		filterExecuteBegin,
		filterExecuteEnd,
		solverNotify,
		goalSchedIterate,
		solverPost
	};

	static std::string eventTagToString[];

	struct Event {
		const void* pObj;
		std::string strObj;
		EventTag tag;
		Event(const void* pObj, std::string strObj, const EventTag& tag) :
			pObj(pObj),strObj(strObj),tag(tag) {}
		virtual bool operator==(const Event& s) const
		{	return pObj == s.pObj and strObj == s.strObj and tag == s.tag;	}
	};

	struct Callback {
		virtual void operator()(const Event&) = 0;
	};

	struct EventMatcher {
		virtual bool operator()(const Event&) const = 0;
		virtual EventMatcher* clone() const = 0;
	};

	struct ObjMatcher : EventMatcher {
		const void* pObj;
		ObjMatcher(const void* pObj) : pObj(pObj) {}
		EventMatcher* clone() const
		{ return new ObjMatcher(pObj); }
		bool operator()(const Event& event) const
		{	return event.pObj == pObj;	}
	};

	struct StrMatcher : EventMatcher {
		std::string str;
		StrMatcher(const std::string& str) : str(str) {}
		EventMatcher* clone() const
		{ return new StrMatcher(str); }
		bool operator()(const Event& event) const
		{	return event.strObj == str;	}
	};

	struct TagMatcher : EventMatcher {
		EventTag tag;
		TagMatcher(const EventTag& tag) : tag(tag) {}
		EventMatcher* clone() const
		{ return new TagMatcher(tag); }
		bool operator()(const Event& event) const
		{	return event.tag == tag;	}
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

	void log(const void* obj, std::string strObj, const EventTag& tag);
	void registerCallback(const EventMatcher& match, Callback& callback);

	typedef StdPair<EventMatcher*,Callback* > CallbackMapItem;
	typedef StdList<CallbackMapItem> CallbackMap;
	CallbackMap callbacks;
};



struct LogEventStats : Logger::Callback
{
	LogEventStats(Logger& logger,const Logger::EventMatcher& match) :
		logger(logger)
	{
		logger.registerCallback(match,*this);
	}

	void operator()(const Logger::Event& ev);

	Logger& logger;

	struct ToEvent {
		ToEvent(const Logger::Event& toEvent,uint nbTriggers,double	accSecs) :
			toEvent(toEvent),nbTriggers(nbTriggers),accSecs(accSecs),
			maxSecs(accSecs),minSecs(accSecs) {}
		Logger::Event	toEvent;
		uint			nbTriggers;
		double			accSecs;
		double			maxSecs;
		double			minSecs;
	};

	struct FromEvent {
		FromEvent(const Logger::Event& fromEvent) : fromEvent(fromEvent),
													timer("accTime",true) {}
		Logger::Event			fromEvent;
		CPUTimer				timer;
		StdList<ToEvent>	toEvents;
	};

	typedef StdList<FromEvent>			Stats;
	Stats	stats;
};

} // Util
} // Casper

std::ostream& operator<<(std::ostream& s,const Casper::Util::Logger::Event& ev);
std::ostream& operator<<(std::ostream& s,const Casper::Util::LogEventStats& ev);

#endif

