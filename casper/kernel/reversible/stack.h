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
 * Reversible stack.
 */

#ifndef _H_CASPER_KERNEL_CONTAINER_STACK
#define _H_CASPER_KERNEL_CONTAINER_STACK

#include <casper/kernel/reversible/reversible.h>

namespace Casper {

/**
 *  Reversible stack.
 * 	\ingroup Containers
 **/
template<class T>
struct Stack
{
        struct 	Node;
        typedef Node* PNode;
        struct Node
        {
                T data;
                PNode next;
                Node(const T& data, PNode next) : data(data),next(next) {}
        };

		State&	state;
        Reversible<PNode>    mTop;

        Stack(State& state) : state(state),mTop(state,PNode(NULL)) {}

        void push(const T& data)
        {	mTop = PNode(new (state.getHeap()) Node(data,mTop));	}

        void pop()
        {	mTop = mTop.get()->next;	}

        bool empty() const 		{ 	return mTop == static_cast<PNode>(NULL); 	}
        const T& top() const 	{ 	return mTop->data;		}
        T& top()  	{ 	return mTop.get()->data;		}

        void clear() { mTop = NULL; }
};

} // Casper

#endif // _H_CASPER_KERNEL_CONTAINER_STACK

