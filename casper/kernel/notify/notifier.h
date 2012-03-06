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

#ifndef _H_CASPER_KERNEL_NOTIFIER
#define _H_CASPER_KERNEL_NOTIFIER

#include <casper/kernel/notify/notifiable.h>
#include <assert.h>

namespace Casper {

/**
 *	Interface for notifier objects. Notifiers send notifications to
 *  registered notifiable objects.
 *  \sa INotifiable
 */
struct INotifier : INotifiable
{
	/// Register notifiable.
	virtual void attach(INotifiable*) = 0;
	/// Unregister notifiable.
	virtual void detach() = 0;
	virtual ~INotifier() {}
};

// general notifier. Holds a reference to external data for convenience.
struct Notifier : public INotifier
{
	typedef Notifier	Self;
	typedef INotifier Super;

	Notifier(const void* data = NULL) : mData(data),pParent(NULL) {}
	Notifier(const Notifier& t) : mData(t.mData) {}

	const void* data() const { return mData; }

	void attach(INotifiable* f)
	{	pParent = f;	}
	bool notify()
	{ assert(pParent != NULL); return pParent->notify(); }

	const void*		mData;
	INotifiable* 	pParent;
};

};

#endif /*_H_CASPER_KERNEL_NOTIFIER */
