 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2006-2011 - Marco Correia <marco.v.correia@gmail.com>                 *
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

#ifndef _H_CASPER_KERNEL_NOTIFIABLE
#define _H_CASPER_KERNEL_NOTIFIABLE

namespace Casper {

struct INotifier;

/**
 * Interface for notifiable objects. A notifiable object receives
 * notifications, which consist on calls to the notify() method with
 * an Event parameter.
 * \sa INotifier
 */
struct INotifiable
{
	/** Called by notifiers to signal an update. The \p event parameter
	  *  carries information on the type of update. */
	virtual bool notify() = 0;
	virtual ~INotifiable() {}
};

// general notifiable, just ignores the event
struct Notifiable : INotifiable
{
	bool notify() {	return true;	}
	~Notifiable() {}
};

};

#endif
