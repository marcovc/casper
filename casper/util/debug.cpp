/**************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2007-2011 - Marco Correia <marco.v.correia@gmail.com>                 *
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

#include <casper/util/debug.h>

namespace Casper {
namespace Util {


std::string Logger::eventTagToString[] = {
		"solverFail",
		"filterSchedIterate",
		"filterExecuteBegin",
		"filterExecuteEnd",
		"solverNotify",
		"goalSchedIterate",
		"solverPost"
};


void Logger::log(const void* obj, std::string strObj, const EventTag& tag)
{
	Event ev(obj,strObj,tag);
	for (auto it = callbacks.begin(); it != callbacks.end(); ++it)
		if ((*it->first)(ev))
			(*it->second)(ev);
}

void Logger::registerCallback(const EventMatcher& match, Callback& callback)
{
	callbacks.pushBack(Casper::pair(match.clone(),&callback));
}

void LogEventStats::operator()(const Logger::Event& ev)
{
	bool found = false;
	for (auto fromIt = stats.begin(); fromIt != stats.end(); ++fromIt)
	{
		fromIt->timer.pause();
		auto toIt = fromIt->toEvents.begin();
		for ( ; toIt != fromIt->toEvents.end(); ++toIt)
			if (toIt->toEvent == ev)
			{
				++toIt->nbTriggers;
				toIt->accSecs += fromIt->timer.secs;
				toIt->maxSecs = std::max(toIt->maxSecs,fromIt->timer.secs);
				toIt->minSecs = std::min(toIt->minSecs,fromIt->timer.secs);
				break;
			}
		if (toIt == fromIt->toEvents.end())
			fromIt->toEvents.pushBack(ToEvent(ev,1,fromIt->timer.secs));
		if (fromIt->fromEvent == ev)
		{
			found = true;
			fromIt->timer.reset();
		}
		fromIt->timer.resume();
	}
	if (!found)
		stats.pushBack(FromEvent(ev));
}


} // Util
} // Casper

std::ostream& operator<<(std::ostream& s,const Casper::Util::Logger::Event& ev)
{
#if 1
	s << "<event pObj='" << ev.pObj
	  << "' strObj='" << ev.strObj
	  <<"' tag='" << Casper::Util::Logger::eventTagToString[ev.tag] << "'/>";
	return s;
#else
	s << "[" << ev.pObj << "," << ev.strObj << "," << Casper::Util::Logger::eventTagToString[ev.tag] << "]";
	return s;
#endif
}


std::ostream& operator<<(std::ostream& os, const Casper::Util::LogEventStats& e)
{
#if 1
	os << "<eventsStats>";
	for (auto fromIt = e.stats.begin(); fromIt != e.stats.end(); ++fromIt)
	{
		os << "<from>" << fromIt->fromEvent;
		for (auto toIt = fromIt->toEvents.begin(); toIt != fromIt->toEvents.end(); ++toIt)
		{
			os << "<to>" << toIt->toEvent;
			os << "<nbTriggers value='" << toIt->nbTriggers << "'/>"
			   << "<accDiffTime value='" << toIt->accSecs << "'/>"
			   << "<avgDiffTime value='" << (toIt->accSecs/static_cast<double>(toIt->nbTriggers))  << "'/>"
			   << "<maxDiffTime value='" << toIt->maxSecs  << "'/>"
			   << "<minDiffTime value='" << toIt->minSecs  << "'/>";
			os << "</to>";
		}
		os << "</from>";
	}
	os << "</eventsStats>";
	return os;
#else
	os << "-------------------------------------------------------------------\n"
		<< "Event Statistics:\n"
	   << "-------------------------------------------------------------------\n"
	   << "T=Number of triggers,DT=Total time diff,DA=Average time diff       \n"
	   << "DM=Max time diff,Dm=Min time diff\n"
	   << "-------------------------------------------------------------------\n";
	for (auto fromIt = e.stats.begin(); fromIt != e.stats.end(); ++fromIt)
		for (auto toIt = fromIt->toEvents.begin(); toIt != fromIt->toEvents.end(); ++toIt)
			os << fromIt->fromEvent <<  " -- " << toIt->toEvent
				<< "\t: T=" << toIt->nbTriggers
			    << ",DT=" << toIt->accSecs
			    << ",DA=" << (toIt->accSecs/static_cast<double>(toIt->nbTriggers))
				<< ",DM=" << toIt->maxSecs
				<< ",Dm=" << toIt->minSecs
			    << std::endl;
	return os;
#endif
}
