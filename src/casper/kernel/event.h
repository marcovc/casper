 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2006-2008 - Marco Correia <marco.v.correia@gmail.com>                 *
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

#ifndef _H_CASPER_KERNEL_EVENT
#define _H_CASPER_KERNEL_EVENT

#include <casper/kernel/common.h>

/**
 * \file
 * Event objects.
 */

namespace casper {

/// A variable domain as become ground.
#define CASPER_EV_VAL 0

/// A bound of variable domain as been updated.
#define CASPER_EV_BND 1

/// A variable domain as been updated.
#define CASPER_EV_DOM 2

/// ?
#define CASPER_EV_UPD 3

/// A variable domain as been created.
#define CASPER_EV_NEW 4

/// Dummy event
#define CASPER_EV_DUM 5

/// The current number of builtin events.
#define CASPER_EV_MAX 6

/**
 * 	Interface for event objects. Only requires that derived objects have
 *  an unique identifier.
 */
struct IEvent
{
	/// Returns the unique identifier for the type of event.
	virtual UInt id() const = 0;
	virtual ~IEvent() {}
};

/// Handler for event objects. See IEvent for complete documentation.
struct Event : detail::PImplIdiom<IEvent>
{
	typedef Event							Self;
	typedef detail::PImplIdiom<IEvent> 		Super;

	Event(IEvent*	f) : Super(f) {}
	UInt id() const {return Super::pImpl->id(); }
};

/// Represents an event signaling the creation of a variable domain.
struct NewEvent : IEvent
{	UInt id() const { return CASPER_EV_NEW;} };

/// Represents an event signaling the groundness of a variable domain.
struct ValEvent : IEvent
{	UInt id() const { return CASPER_EV_VAL;} };

/// Represents an event signaling a bound update of a variable domain.
struct BndEvent : IEvent
{	UInt id() const { return CASPER_EV_BND;} };

/// Represents an event signaling an update of a variable domain.
struct DomEvent : IEvent
{	UInt id() const { return CASPER_EV_DOM;} };

/// Represents an event signaling ???
struct UpdEvent : IEvent
{	UInt id() const { return CASPER_EV_UPD;} };

/// Represents an event signaling ???
struct DumEvent : IEvent
{	UInt id() const { return CASPER_EV_DUM;} };

};

#endif /*_H_CASPER_KERNEL_EVENT*/
