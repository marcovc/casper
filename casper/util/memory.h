 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2003-2011 - Marco Correia <marco.v.correia@gmail.com>                 *
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

#ifndef _H_CASPER_KERNEL_MEMORY
#define _H_CASPER_KERNEL_MEMORY

#include <cstddef>
#include <assert.h>
#include <math.h>
#include <sstream>

#ifdef _MSC_VER
#pragma warning(disable: 4290) 
#endif

//#include <casper/kernel/common.h>
//#include <casper/kernel/debug.h>
#include <casper/util/exception.h>

#include <signal.h>	// temp

#if defined(_MSC_VER) | defined(__APPLE__) | defined(SWIG)
typedef unsigned int uint;
#endif

namespace Casper {

namespace Util {

struct IHeap
{
	IHeap()  {}
	virtual void*	allocate(size_t) = 0;
	virtual void	deallocate(void*) = 0;
	virtual size_t	maxSize() const = 0;
	virtual size_t	maxAllocated() const = 0;
	virtual bool	doesGC() const = 0;

	virtual ~IHeap() {}
};
} // Util

namespace Exception {
struct HeapOverflow : EMemOut
{
	HeapOverflow(Util::IHeap* pHeap,const std::string& heapname) :
		pHeap(pHeap),heapname(heapname)  {}
	virtual ~HeapOverflow() throw() {}
	virtual const char * 	what  () const throw ()
	{
		std::ostringstream s;
		s << heapname << " : maxSize()=" << pHeap->maxSize();
		return s.str().c_str();
	}
	Util::IHeap* pHeap;
	std::string heapname;
};
} // Exception

namespace Util {
/**
 * 	Just a wrapper around standard C++ heap (with new/delete)
 */
struct StdMem : IHeap
{
	StdMem()
	{}
	void*	allocate(size_t s) throw(Exception::HeapOverflow)
	{
		void* ret;
		try {
			ret = new char[s];
		}
		catch (std::bad_alloc&)
		{
			throw Exception::HeapOverflow(this,"StdMem");
		}
		//Debug::log(this,"allocated %d bytes @ %p",s,ret);
		return ret;
	}
	void	deallocate(void* v)
	{
		//Debug::log(this,"deallocating %p",v);
		delete[] (char*)v;
	}
	bool	doesGC() const {	return false;	}

	size_t maxSize() const { return 0; }
	size_t maxAllocated() const {	return 0;	}

	~StdMem()
	{}
};

/**
 *	Static memory allocated from the heap. Will NOT grow and does not allow
 *	deallocation.
 */
struct StaticHeap : IHeap
{
	unsigned int uMaxSize;
	char* 	ptr;
	char*	mem;

	StaticHeap(uint maxSize = static_cast<uint>(64e6)) : uMaxSize(maxSize)
	{
		try {
			mem = new char[uMaxSize];
		}
		catch (std::bad_alloc&)
		{
			throw Exception::HeapOverflow(this,"StaticHeap");
		}
		ptr = mem;
	}
	void*	allocate(size_t s) throw(Exception::HeapOverflow)
	{
		if ((ptr-mem) + s > uMaxSize)
			throw Exception::HeapOverflow(this,"StaticHeap");
		void* ret=ptr;
		ptr+=s;
		//Debug::log(this,"allocated %d bytes @ %p",s,ret);
		return ret;
	}

	void	reset()	{	ptr = mem;	}

	void	deallocate(void*)
	{	/* deallocation is done on destruction */	}

	bool	doesGC() const {	return true;	}

	size_t	maxSize() const	{	return uMaxSize;	}
	size_t	maxAllocated() const {	return ptr-mem;	}

	~StaticHeap()
	{
		delete[] mem;
	}
};

/**
 *	Dynamic memory allocated from the heap. Will grow
 *	according to the geometric progression initSize*factor^[0..inf]. Does not
 *	allow deallocation.
 */
struct DynamicHeap : IHeap
{
	const uint 		initSize;
	const double 	factor;
	const uint 		maxBulks;
	uint 	curBulk;
	size_t* bulkSize;
	char**	mem;
	char* 	ptr;
	DynamicHeap(uint initSize = 1024,
			    double factor = 1.5);
	inline void*	allocate(size_t s) throw(Exception::HeapOverflow);
	void	deallocate(void*)
	{	/* deallocation is done on destruction */	}

	bool	doesGC() const {	return true;	}

	size_t	maxSize() const
	{
		return initSize*(1-::pow(factor,static_cast<double>(maxBulks+1)))/
				(1-factor);
	}
	size_t	maxAllocated() const
	{
		return initSize*(1-::pow(factor,static_cast<double>(curBulk+1)))/
				(1-factor);
	}
	~DynamicHeap();
};

void*	DynamicHeap::allocate(size_t s) throw(Exception::HeapOverflow)
{
	if (ptr+s > mem[curBulk]+bulkSize[curBulk])
	{
		++curBulk;
		while (s > bulkSize[curBulk])
			++curBulk;
		try {
		mem[curBulk] = new char[bulkSize[curBulk]];
		}
		catch (std::bad_alloc&)
		{	throw Exception::HeapOverflow(this,"DynamicHeap");	}
		ptr = mem[curBulk];
	}
	void* ret=ptr;
	ptr+=s;
	return ret;
}

struct IRHeap : IHeap
{
	virtual void signalCPCreate() = 0;
	virtual void signalCPRestore() = 0;
	virtual void signalCPRemove() = 0;

};

/**
 *	Reversible static memory allocated from the heap. Will NOT grow and
 *	does not allow deallocation. It will reuse allocated space if that
 *	space was allocated on a state which was undone.
 */
struct RStaticHeap : IRHeap
{
	unsigned int uMaxSize;
	char* 	ptr;
	char*	mem;
	char* 	last_ptr;

	RStaticHeap(uint maxSize = static_cast<uint>(350E6)) : uMaxSize(maxSize)
	{
		try {
		mem = new char[uMaxSize];
		}
		catch (std::bad_alloc&)
		{	throw Exception::HeapOverflow(this,"RStaticHeap");	}
		ptr = mem;
		last_ptr = mem-sizeof(char*);
	}
	void*	allocate(size_t s) throw(Exception::HeapOverflow)
	{
		assert( (ptr-mem) + s < uMaxSize);
		if ((ptr-mem) + s > uMaxSize)
			throw Exception::HeapOverflow(this,"RStaticHeap");
		void* ret=ptr;
		ptr+=s;
		//Debug::log(this,"allocated %d bytes @ %p",s,ret);
		return ret;
	}

	void	deallocate(void*)
	{	/* deallocation is done on destruction */	}

	bool	doesGC() const {	return true;	}

	void signalCPCreate()
	{
		void* v = allocate(sizeof(void*));
		*(void**)v = (void*)last_ptr;
		last_ptr = (char*)v;
	}
	void signalCPRestore()
	{
		ptr = last_ptr + sizeof(void*);
		assert(ptr>=mem);
	}
	void signalCPRemove()
	{
		last_ptr = (*(char**)last_ptr);
		assert(ptr>=mem);
	}
	size_t	maxSize() const	{	return uMaxSize;	}
	size_t	maxAllocated() const	{	return ptr-mem;	}

	~RStaticHeap()
	{
		delete[] mem;
	}
};

/**
 *	Reversible dynamic memory allocated from the heap. Will grow
 *	according to the geometric progression initSize*factor^[0..inf].
 *	Does not allow deallocation. It will reuse allocated space if that
 *	space was allocated on a state which was undone.
 */
struct RDynamicHeap : IRHeap
{
	typedef uint BulkIdx;
	const uint initSize;
	const double factor;
	const double logFactor;			// for avoiding computation every time
	const double logInitSizeOverLogFactor;	// for avoiding computation every time
	const uint maxBulks;
	BulkIdx curBulk;
	size_t* bulkSize;
	char**	mem;
	char*	ptr;
	struct Frame {
		Frame* 	pLastFrame;
		BulkIdx	idxCurBulk;
	};
	Frame*	pFrame;

	RDynamicHeap(uint initSize = 32 * 1024,
			     double factor = 2);

	inline void*	allocate(size_t s) throw(Exception::HeapOverflow);

	void	deallocate(void*)
	{	/* deallocation is done on destruction */	}

	void signalCPCreate()
	{
		Frame* v = static_cast<Frame*>(allocate(sizeof(Frame)));
		v->pLastFrame = pFrame;
		v->idxCurBulk = curBulk;
		pFrame = v;
	}
	void signalCPRestore()
	{
		ptr = static_cast<char*>(static_cast<void*>(pFrame)) +
					sizeof(Frame);
		curBulk = pFrame->idxCurBulk;
		assert(ptr>=mem[curBulk]);
	}
	void signalCPRemove()
	{
		pFrame = pFrame->pLastFrame;
		assert(ptr>=mem[curBulk]);
	}

	bool	doesGC() const {	return true;	}

	size_t	maxSize() const
	{
		return initSize*(1-::pow(factor,static_cast<double>(maxBulks+1)))/
					(1-factor);
	}
	size_t	maxAllocated() const
	{
		return initSize*(1-::pow(factor,static_cast<double>(curBulk+1)))/
						(1-factor);
	}

	~RDynamicHeap();
};

inline void*	RDynamicHeap::allocate(size_t s) throw(Exception::HeapOverflow)
{
	if (ptr+s > mem[curBulk]+bulkSize[curBulk])
	{
		++curBulk;
#if 0
		while (s > bulkSize[curBulk])
			++curBulk;
#else
		if (s >= initSize)	// FIXME: why isn't this s>=bulksize[curBulk] ?
			curBulk = std::max(curBulk,
					 static_cast<uint>(::ceil(::log(static_cast<double>(s))/logFactor-logInitSizeOverLogFactor)));
#endif
		try {
		if (mem[curBulk] == NULL)
			mem[curBulk] = new char[bulkSize[curBulk]];
		}
		catch (std::bad_alloc&)
		{	throw Exception::HeapOverflow(this,"RDynamicHeap");	}

		ptr = mem[curBulk];
	}
	void* ret=ptr;
	ptr+=s;
	return ret;
}

/*
struct Heap : Util::PImplIdiom<IHeap>
{
	typedef Heap					Self;
	typedef Util::PImplIdiom<IHeap> 		Super;
	Heap() : Super(new RStaticHeap()) {}
	Heap(IHeap* h) : Super(h) {}
	//Heap(const Heap& h) : Super(h.getPImpl()) {}

	void*	allocate(size_t s)	{ assert(Super::pImpl); return Super::pImpl->allocate(s);	}
	void	deallocate(void* p) { assert(Super::pImpl); return Super::pImpl->deallocate(p);	}
	size_t	maxSize() const			{ assert(Super::pImpl); return Super::pImpl->maxSize(); 	}
	size_t	maxAllocated() const	{ assert(Super::pImpl); return Super::pImpl->maxAllocated(); 	}
	bool	doesGC() const 		{ assert(Super::pImpl); return Super::pImpl->doesGC(); 	}
};
*/

inline StdMem& stdHeap()
{
	static StdMem m;
	return m;
}

} // Util
} // Casper

//std::ostream& operator<<(std::ostream& os, const Casper::RStaticHeap& h);

#if 0
// don't know yet which is correct
#ifdef _MSC_VER
	#define HEAP_SIG	Casper::Heap&
#else
	#define HEAP_SIG	Casper::Heap
#endif

inline void * operator new (size_t size, HEAP_SIG heap)
{	assert(heap.getPImpl()!=NULL); return heap.getPImpl()->allocate(size);}

inline void * operator new[] (size_t size, HEAP_SIG heap)
{	assert(heap.getPImpl()!=NULL); return heap.getPImpl()->allocate(size);}

inline void operator delete (void * p, HEAP_SIG heap)
{	assert(heap.getPImpl()!=NULL); heap.deallocate(p);	}
#else

inline void * operator new (size_t size, Casper::Util::IHeap& heap)
{	return heap.allocate(size);}

inline void * operator new[] (size_t size, Casper::Util::IHeap& heap)
{	return heap.allocate(size);}

inline void operator delete (void * p, Casper::Util::IHeap& heap)
{	heap.deallocate(p);	}

#endif

std::ostream& operator<<(std::ostream& os, const Casper::Exception::HeapOverflow& e);


#endif

