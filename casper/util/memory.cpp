/**************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2003-2008 - Marco Correia <marco.v.correia@gmail.com>                 *
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

#include <casper/util/memory.h>
#include <typeinfo>

namespace Casper {
namespace Util {


StdMem	stdHeap;

DynamicHeap::DynamicHeap(uint initSize,double factor) : initSize(initSize),
														factor(factor),
				  		maxBulks(static_cast<uint>(::log(4E9)/::log(factor)))
{
	curBulk = 0;
	// fill bulk sizes
	try {
		bulkSize = new size_t[maxBulks];
	}
	catch (std::bad_alloc&)
	{	throw Exception::HeapOverflow(this,"DynamicHeap");	}
	bulkSize[0] = initSize;
	try {
		mem = new char*[maxBulks];
	}
	catch (std::bad_alloc&)
	{	throw Exception::HeapOverflow(this,"DynamicHeap");	}
	mem[curBulk] = new char[bulkSize[0]];
	for (uint i = 1; i < maxBulks; i++)
	{
		bulkSize[i] = static_cast<uint>(bulkSize[i-1]*factor);
		mem[i] = NULL;
	}
	ptr = mem[curBulk];
}

DynamicHeap::~DynamicHeap()
{
	for (uint i = 0; i < maxBulks; i++)
		if (mem[i]!=NULL)
			delete[] mem[i];
	delete[] mem;
	delete[] bulkSize;
}


RDynamicHeap::RDynamicHeap(uint initSize,double factor) :
						initSize(initSize),
						factor(factor),
						logFactor(::log(factor)),
						logInitSizeOverLogFactor(::log(initSize)/logFactor),
				  		maxBulks(static_cast<uint>(::log(4E9)/::log(factor)))
{
	curBulk = 0;
	try {
	bulkSize = new size_t[maxBulks];
	}
	catch (std::bad_alloc&)
	{	throw Exception::HeapOverflow(this,"RDynamicHeap");	}

	bulkSize[0] = initSize;

	try{
	mem = new char*[maxBulks];
	mem[curBulk] = new char[bulkSize[0]];
	}
	catch (std::bad_alloc&)
	{	throw Exception::HeapOverflow(this,"RDynamicHeap");	}

	ptr = mem[curBulk];
	for (uint i = 1; i < maxBulks; i++)
	{
		bulkSize[i] = static_cast<uint>(bulkSize[i-1]*factor);
		mem[i] = NULL;
	}
	pFrame = static_cast<Frame*>(
				static_cast<void*>(mem[curBulk]-sizeof(Frame)));
}

RDynamicHeap::~RDynamicHeap()
{
	for (uint i = 0; i < maxBulks; i++)
		if (mem[i]!=NULL)
			delete[] mem[i];
	delete[] mem;
	delete[] bulkSize;
}

} // Util
} // Casper

std::ostream& operator<<(std::ostream& os, const Casper::Exception::HeapOverflow& e)
{
	os << "heap overflow : " << typeid(*e.pHeap).name()
		<< "@" << e.pHeap << " (max size is "
		<<  e.pHeap->maxSize() << " bytes)";
	return os;
}
