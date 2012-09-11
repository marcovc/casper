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

/**
 * \file
 * StdStack.
 */

#ifndef _H_CASPER_KERNEL_CONTAINER_STDSTACK
#define _H_CASPER_KERNEL_CONTAINER_STDSTACK

#include <casper/util/memory.h>

namespace Casper {
namespace Util {


/**
 * 	StdStack.
 * 	\ingroup Containers
 **/
template<class T>
struct StdStack
{
        struct 	Node;
        typedef Node* PNode;
        struct Node
        {
                T data;
                PNode next;
                Node(const T& data, PNode next) : data(data),next(next) {}
        };

		IHeap&	 heap;
        PNode    mTop;

        StdStack(IHeap& heap = stdHeap()) : heap(heap),mTop(NULL) {}

        void push(const T& data)
        {	mTop = new (heap) Node(data,mTop);	}

        void pop()
        {
        	assert(!empty());
        	PNode aux(mTop);
        	mTop = mTop->next;
        	aux->~Node();
        	if (!heap.doesGC())
        		heap.deallocate(aux);
        }

        bool empty() const 		{ 	return mTop == NULL; 	}
        const T& top() const 	{ 	assert(!empty()); return mTop->data;		}
        T& top()  	{ 	assert(!empty()); return mTop->data;		}

        void clear()
        {
        	if (heap.doesGC())
        		mTop = NULL;
        	else
        		while (!empty())
        			pop();
        }
};

} // Util
} // Casper

#endif // _H_CASPER_KERNEL_CONTAINER_STDSTACK

