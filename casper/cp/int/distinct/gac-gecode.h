
/**
 *	\file
 *
 *  This file was adapted from the Gecode library [\ref gecode],
 *  copyright Christian Schulte, 2003.
 */

/*
GECODE LICENSE AGREEMENT

This software and its documentation are copyrighted by the
individual authors as listed in each file. The following
terms apply to all files associated with the software unless
explicitly disclaimed in individual files.

The authors hereby grant permission to use, copy, modify,
distribute, and license this software and its documentation
for any purpose, provided that existing copyright notices
are retained in all copies and that this notice is included
verbatim in any distributions. No written agreement,
license, or royalty fee is required for any of the
authorized uses. Modifications to this software may be
copyrighted by their authors and need not follow the
licensing terms described here, provided that the new terms
are clearly indicated on the first page of each file where
they apply.

IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE LIABLE TO
ANY PARTY FOR DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR
CONSEQUENTIAL DAMAGES ARISING OUT OF THE USE OF THIS
SOFTWARE, ITS DOCUMENTATION, OR ANY DERIVATIVES THEREOF,
EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.

THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
PURPOSE, AND NON-INFRINGEMENT.  THIS SOFTWARE AND ITS
DOCUMENTATION ARE PROVIDED ON AN "AS IS" BASIS, AND THE
AUTHORS AND DISTRIBUTORS HAVE NO OBLIGATION TO PROVIDE
MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR
MODIFICATIONS.
*/

/*
 *  Main authors:
 *     Christian Schulte <schulte@gecode.org>
 *
 *  Copyright:
 *     Christian Schulte, 2003
 *
 *  This file is also part of Gecode, the generic constraint
 *  development environment:
 *     http://www.gecode.org
 */

/*
 *  Adapted to the CaSPER library by
 *  Marco Correia <mvc@di.fct.unl.pt>
 */

#ifdef USE_GECODE_DOM_ALLDIFF

#include <casper/cp/filter/dom.h>

namespace Casper {
namespace CP {
namespace Detail {

template <class T>
class CombPtrFlag
{
	private:
	ptrdiff_t cpf;
	public:
	CombPtrFlag(T* p1, T* p2);
	void init(T* p1, T* p2);
	T* ptr(T* p) const;
	int is_set(void) const;
	void set(void);
	void unset(void);
};

class BiLink
{
	private:
	BiLink* _prev; BiLink* _next;
	public:
	BiLink(void);

	BiLink* prev(void) const; void prev(BiLink*);
	BiLink* next(void) const; void next(BiLink*);

	void add(BiLink*);
	void unlink(void);

	void mark(void);
	bool marked(void) const;

	bool empty(void) const;
};

template <class View> class Edge;

template <class View>
class Node : public BiLink
{
	public:
	unsigned int pre, low, comp;
	Node(void);
	Edge<View>* edge_fst(void) const;
	Edge<View>* edge_lst(void) const;

	static void* operator new(size_t, void*);
};

template <class View>
class ValNode : public Node<View>
{
	protected:
	const int _val; Edge<View>* _matching;
	public:
	ValNode(int);
	int val(void) const;
	void matching(Edge<View>*);
	Edge<View>* matching(void) const;
};

template <class View>
class VarNode : public Node<View>
{
	protected:
	typedef typename View::Elem	ViewElem;
	Edge<View>* _val_edges; ViewElem& _var;
	public:
	VarNode(ViewElem&);
	Edge<View>*  val_edges(void) const;
	Edge<View>** val_edges_ref(void);

	ViewElem& var(void);
};

template <class View>
class Edge : public BiLink
{
	protected:
	Edge<View>*              _next_edge;
	CombPtrFlag<Node<View> > sd;
	public:
	Edge(Node<View>*, Node<View>*);

	Node<View>* dst(Node<View>*) const;

	VarNode<View>* var(ValNode<View>*) const;
	ValNode<View>* val(VarNode<View>*) const;

	bool used(Node<View>*) const;
	void use(void);
	void free(void);

	void revert(Node<View>*);

	Edge<View>*  next_edge(void) const;
	Edge<View>** next_edge_ref(void);

	Edge<View>* next(void) const;
	static void* operator new(size_t, void*);
};

#define forceinline	inline
// combptr.icc

template <class T>
forceinline
CombPtrFlag<T>::CombPtrFlag(T* p1, T* p2)
	: cpf(reinterpret_cast<ptrdiff_t>(p1) ^ reinterpret_cast<ptrdiff_t>(p2)) {}

template <class T>
forceinline T*
CombPtrFlag<T>::ptr(T* p) const {
	return reinterpret_cast<T*>((cpf&~1) ^ reinterpret_cast<ptrdiff_t>(p));
}

template <class T>
forceinline int
CombPtrFlag<T>::is_set(void) const {
	return static_cast<int>(cpf&1);
}

template <class T>
forceinline void
CombPtrFlag<T>::set(void) {
	cpf |= 1;
}

template <class T>
forceinline void
CombPtrFlag<T>::unset(void) {
  cpf &= ~1;
}

template <class T>
forceinline void
CombPtrFlag<T>::init(T* p1, T* p2) {
 cpf = reinterpret_cast<ptrdiff_t>(p1) ^ reinterpret_cast<ptrdiff_t>(p2);
}

// bilink

 forceinline
BiLink::BiLink(void) {
     _prev = this; _next = this;
}

forceinline BiLink*
BiLink::prev(void) const {
   return _prev;
}

forceinline BiLink*
BiLink::next(void) const {
	return _next;
}

forceinline void
BiLink::prev(BiLink* l) {
	_prev = l;
}

forceinline void
BiLink::next(BiLink* l) {
	_next = l;
}

forceinline void
BiLink::add(BiLink* l) {
     l->_prev = this; l->_next = _next;
     _next->_prev = l; _next = l;
}
forceinline void
BiLink::unlink(void) {
	BiLink* p = _prev; BiLink* n = _next;
	p->_next = n; n->_prev = p;
}

forceinline void
BiLink::mark(void) {
	_next = NULL;
}

forceinline bool
BiLink::marked(void) const {
	return _next == NULL;
}

forceinline bool
BiLink::empty(void) const {
	return _prev == this;
}

// node.icc

 /*
 * Nodes
*
*/

template <class View>
forceinline
Node<View>::Node(void)
	 : pre(0), low(0), comp(0) {}

template <class View>
forceinline Edge<View>*
Node<View>::edge_fst(void) const {
	return static_cast<Edge<View>*>(BiLink::next());
}

template <class View>
forceinline Edge<View>*
Node<View>::edge_lst(void) const {
	return static_cast<Edge<View>*>(static_cast<BiLink*>(const_cast<Node<View>*>(this)));
}

template <class View>
forceinline void*
Node<View>::operator new(size_t, void* p) {
	return p;
}

/*
* Value nodes
*
*/

template <class View>
forceinline
ValNode<View>::ValNode(int v)
	     : _val(v), _matching(NULL) {}

template <class View>
forceinline int
ValNode<View>::val(void) const {
	return _val;
}

template <class View>
forceinline void
ValNode<View>::matching(Edge<View>* e) {
     _matching = e;
}

template <class View>
forceinline Edge<View>*
ValNode<View>::matching(void) const {
	return _matching;
}

/*
* Var nodes
*
*/


template <class View>
forceinline
VarNode<View>::VarNode(typename View::Elem& x)
     : _var(x) {}

template <class View>
forceinline Edge<View>*
VarNode<View>::val_edges(void) const {
     return _val_edges;
}

template <class View>
forceinline Edge<View>**
VarNode<View>::val_edges_ref(void) {
	return &_val_edges;
}

template <class View>
forceinline typename View::Elem&
VarNode<View>::var(void)  {
	return _var;
}

// edge.icc

template <class View>
forceinline
Edge<View>::Edge(Node<View>* s, Node<View>* d)
     : sd(s,d) {
	s->add(this);
}

template <class View>
forceinline Node<View>*
Edge<View>::dst(Node<View>* s) const {
	return sd.ptr(s);
}

template <class View>
forceinline void
Edge<View>::revert(Node<View>* d) {
	unlink();
	d->add(this);
}

template <class View>
forceinline VarNode<View>*
Edge<View>::var(ValNode<View>* n) const {
	return static_cast<VarNode<View>*>(sd.ptr(n));
}

template <class View>
forceinline ValNode<View>*
Edge<View>::val(VarNode<View>* x) const {
	return static_cast<ValNode<View>*>(sd.ptr(x));
}

template <class View>
forceinline bool
Edge<View>::used(Node<View>* v) const {
	return sd.is_set() || (v->comp == sd.ptr(v)->comp);
}

template <class View>
forceinline void
Edge<View>::use(void) {
	sd.set();
}

template <class View>
forceinline void
	Edge<View>::free(void) {
	sd.unset();
	}

template <class View>
forceinline Edge<View>*
Edge<View>::next_edge(void) const {
	return _next_edge;
}

template <class View>
forceinline Edge<View>**
Edge<View>::next_edge_ref(void) {
	return &_next_edge;
}

template <class View>
forceinline Edge<View>*
Edge<View>::next(void) const {
	return static_cast<Edge<View>*>(BiLink::next());
}

template <class View>
forceinline void*
Edge<View>::operator new(size_t, void* p) {
	return p;
}

/**
  * \brief var-value graph for propagating domain-consistent distinct
  *
*/
template <class View>
class VarValGraph
{
	protected:
	Store& store;
    VarNode<View>** var; int n_var;
    ValNode<View>** val; int n_val;
    char* mem;
    unsigned int count;
    unsigned int cnt0;
    unsigned int cnt1;
    Util::StdStack<Node<View>*> n_s;

    typedef typename View::Elem	ViewDom;
  	typedef View	DomArray;
  	public:
    VarValGraph(Store& s,int, View&, const int*, int, unsigned int);
    VarValGraph(Store& s,int, View&, int, unsigned int, int);
    ~VarValGraph(void);

    size_t size;

    static VarValGraph* init(Store&,int,View&);

    bool initial_match(void);

    void mark(void);
    bool tell();

    bool overflow(void) const;

    bool sync(void);

  	protected:
    bool search_match(VarNode<View>*);
    bool match(VarNode<View>*);
    void scc(Node<View>*);
};


template <class View>
forceinline
VarValGraph<View>::VarValGraph(Store& s,int n_var0, View& x,
				    int n_val0,
				    unsigned int n_edges, int min)
    :  store(s),n_var(n_var0), n_val(n_val0),
       count(0), cnt0(0), cnt1(0), n_s(s/*,n_var+n_val*/)
{
	size_t edge_size  = sizeof(Edge<View>) * n_edges;
    size_t vars_size = sizeof(VarNode<View>) * n_var;
    size_t var_size  = sizeof(VarNode<View>*) * n_var;
    size_t vals_size  = sizeof(ValNode<View>) * n_val;
    size_t val_size   = sizeof(ValNode<View>*) * n_val;
    size_t size = edge_size +
      vars_size + var_size + vals_size + val_size;
    mem = reinterpret_cast<char*>(store.getEnv().getHeap().allocate(size));
    Edge<View>*     edges      = reinterpret_cast<Edge<View>*>(mem);
    VarNode<View>* var_nodes = reinterpret_cast<VarNode<View>*>
      (mem+edge_size);
    ValNode<View>*  val_nodes  = reinterpret_cast<ValNode<View>*>
      (mem+edge_size+vars_size);
    var = reinterpret_cast<VarNode<View>**>
      (mem+edge_size+vars_size+vals_size);
    val  = reinterpret_cast<ValNode<View>**>
      (mem+edge_size+vars_size+vals_size+var_size);

    // Init value nodes
    {
    	int v = min+n_val;
      	for (int i = n_val; i--; )
			val[i] = new (val_nodes + i) ValNode<View>(--v);
    }

    // Init var nodes and edges
    for (int i = n_var; i--; )
    {
    	var[i] = new (var_nodes + i) VarNode<View>(x[i]);
      	Edge<View>** edge_p = var[i]->val_edges_ref();
      	for (typename View::Elem::Dom::Iterator it = x[i]->begin();
      			it != x[i]->end(); ++it)
      	{
			*edge_p = new (edges++) Edge<View>(val_nodes+*it-min,var_nodes+i);
			edge_p = (*edge_p)->next_edge_ref();
      	}
      	*edge_p = NULL;
    }
}

template <class View>
forceinline
VarValGraph<View>::VarValGraph(Store& s, int n_var0, View& x,
					    const int* val_inf,
					    int n_val0, unsigned int n_edges)
    :  store(s),n_var(n_var0), n_val(n_val0),
       count(0), cnt0(0), cnt1(0), n_s(s/*,n_var+n_val*/)
{
	size_t edge_size  = sizeof(Edge<View>) * n_edges;
    size_t vars_size = sizeof(VarNode<View>) * n_var;
    size_t var_size  = sizeof(VarNode<View>*) * n_var;
    size_t vals_size  = sizeof(ValNode<View>) * n_val;
    size_t val_size   = sizeof(ValNode<View>*) * n_val;
    size_t size = edge_size +
      vars_size + var_size + vals_size + val_size;
    mem = reinterpret_cast<char*>(s.getEnv().getHeap().allocate(size));
    Edge<View>*     edges      = reinterpret_cast<Edge<View>*>(mem);
    VarNode<View>* var_nodes = reinterpret_cast<VarNode<View>*>
      (mem+edge_size);
    ValNode<View>*  val_nodes  = reinterpret_cast<ValNode<View>*>
      (mem+edge_size+vars_size);
    var = reinterpret_cast<VarNode<View>**>
      (mem+edge_size+vars_size+vals_size);
    val  = reinterpret_cast<ValNode<View>**>
      (mem+edge_size+vars_size+vals_size+var_size);

    // Init value nodes
    for (int i = n_val; i--; )
      val[i] = new (val_nodes + i) ValNode<View>(val_inf[i]);

    // Init var nodes
    for (int i = n_var; i--; )
    {
    	var[i] = new (var_nodes + i) VarNode<View>(x[i]);
      	Edge<View>** edge_p = var[i]->val_edges_ref();
      	int j = 0;
      	for (typename ViewDom::Iterator it = x[i]->begin();
      			it != x[i]->end(); ++it)
      	{
			while (val_inf[j] < *it)
	  			j++;
			*edge_p = new (edges++) Edge<View>(val_nodes+j,var_nodes+i);
			edge_p = (*edge_p)->next_edge_ref();
      	}
      	*edge_p = NULL;
    }
}

template <class View>
VarValGraph<View>*
VarValGraph<View>::init(Store& s,int n_var, View& x)
{
    // Find value information for construction of var value graph
    int min = x[n_var-1]->min();
    int max = x[n_var-1]->max();
    unsigned int size = x[n_var-1]->size();
    for (int i=n_var-1; i--; )
    {
    	size += x[i]->size();
      	min = std::min(min,x[i]->min());
      	max = std::max(max,x[i]->max());
    }

    unsigned int width = max-min+1;

    // Definitly not enough values
    if (width < static_cast<unsigned int>(n_var))
      return NULL;

    // Test whether the values are dense or sparse
    //if (static_cast<unsigned int>(n_var)*2 >= width)
      return new (s) VarValGraph<View>(s,n_var,x,static_cast<int>(width),size,min);
}

template <class View>
bool
VarValGraph<View>::search_match(VarNode<View>* x)
{
    x->comp = count;
    // Try to find matching edge cheaply: is there a free edge around?
    {
    	Edge<View>* e = x->val_edges();
      	// This holds true as domains are never empty
      	assert(e != NULL);
      	do
      	{
			if (!e->val(x)->matching())
			{
	  			e->revert(x); e->val(x)->matching(e);
	  			return true;
			}
			e = e->next_edge();
      	} while (e != NULL);
    }
    // No, find matching edge by augmenting path method
    {
    	Edge<View>* e = x->val_edges();
      	do
      	{
			ValNode<View>*  n = e->val(x);
			VarNode<View>* y = n->matching()->var(n);
			if ((y->comp < count) && search_match(y))
			{
	  			n->matching()->revert(n);
	  			e->revert(x); n->matching(e);
	  			return true;
			}
			e = e->next_edge();
      	} while (e != NULL);
    }
    return false;
}

template <class View>
forceinline bool
VarValGraph<View>::match(VarNode<View>* x)
{
    assert(x->edge_fst() == x->edge_lst());
    count++;
    return search_match(x);
}

template <class View>
bool
VarValGraph<View>::initial_match(void)
{
	for (int i = n_var; i--; )
    	if (!match(var[i]))
			return false;
    return true;
}

template <class View>
void
VarValGraph<View>::scc(Node<View>* w)
{
	w->pre = cnt0;
    w->low = cnt0;
    unsigned int min = cnt0++;
    n_s.push(w);
    for (Edge<View>* e = w->edge_fst(); e != w->edge_lst(); e = e->next())
    {
    	Node<View>* v = e->dst(w);
    	if (v->pre < count)
			scc(v);
      	if (v->low < min)
			min = v->low;
    }
    if (min < w->low)
    {
    	w->low = min;
      	return;
    }
    while (true)
    {
    	Node<View>* v = n_s.top();
    	n_s.pop();
      	v->comp = cnt1;
		v->low  = limits<uint>::max();
      	if (v==w)
      		break;
    }
    cnt1++;
}

template <class View>
forceinline void
VarValGraph<View>::mark(void)
{
    // Marks all edges as used that are on simple paths in the graph
    // that start from a free (unmatched node) by depth-first-search
    n_s.clear();
    // Insert all free nodes: they can be only value nodes as we
    // have a maximum matching covering all var nodes
    count++;
    for (int i = n_val; i--; )
      if (!val[i]->matching())
      {
		// Is it orphaned?
		if (val[i]->empty())
	  		val[i] = val[--n_val];
		else
		{
	  		val[i]->comp = count;
	  		n_s.push(val[i]);
		}
      }

    // Invariant: only value nodes are on the stack!
    while (!n_s.empty())
    {
    	ValNode<View>* n = static_cast<ValNode<View>*>(n_s.top());
    	n_s.pop();
      	for (Edge<View>* e = n->edge_fst(); e != n->edge_lst(); e = e->next())
      	{
			// Get the value node
			e->use();
			VarNode<View>* x = e->var(n);
			if (x->comp < count)
			{
	  			x->comp = count;
	  			assert(x->edge_fst()->next() == x->edge_lst());
	  			ValNode<View>* m = x->edge_fst()->val(x);
	  			x->edge_fst()->use();
	  			if (m->comp < count)
	  			{
	    			m->comp = count;
	    			n_s.push(m);
	  			}
			}
      	}
    }

    count++;
    cnt0 = count;
    cnt1 = count;
    n_s.clear();
    for (int i = n_var; i--; )
      if (var[i]->comp < count)
	scc(var[i]);
    count = cnt0+1;
}

template <class View>
forceinline bool
VarValGraph<View>::tell()
{
    // Tell constraints and also eliminate nodes and edges
	for (int i = n_var; i--; )
    	if (!var[i]->edge_fst()->used(var[i]))
    	{
			*var[i]->var() = var[i]->edge_fst()->val(var[i])->val();
			if (var[i]->var()->empty())
				return false;
			var[i]->edge_fst()->val(var[i])->matching(NULL);
			var[i] = var[--n_var];
      	}
      	else
      	{
      		//BatchRangeErase<typename Dom::Dom> be(stdHeap,var[i]->var());

			for (Edge<View>* e = var[i]->val_edges(); e!=NULL; e = e->next_edge())
	  			if (!e->used(var[i]))
	  				//be.erase(e->val(var[i])->val());
	   				if (!var[i]->var()->erase(e->val(var[i])->val()))
	   					return false;

	   		//if (!be.commit())
	   		//	return false;
	    }
	return true;
}

template <class View>
bool
VarValGraph<View>::sync(void)
{
	// Util::StdStack for var nodes to be rematched
	Util::StdArray<VarNode<View>*>	re(store,n_var);
    int n_re = 0;
    // Synchronize nodes
    for (int i = n_var; i--; )
    {
    	VarNode<View>* x = var[i];
      	if (x->var()->singleton())
      	{
			x->edge_fst()->val(x)->matching(NULL);
			for (Edge<View>* e = x->val_edges(); e != NULL; e = e->next_edge())
	  			e->unlink();
			var[i] = var[--n_var];
      	}
      	else
      	{
			typename ViewDom::Dom::Iterator it = x->var()->begin();
			Edge<View>*  m = x->edge_fst();      // Matching edge
			Edge<View>** p = x->val_edges_ref();
			Edge<View>*  e = *p;

			while (it != x->var()->end())
			{
				while (e->val(x)->val() < *it)
				{
					// Skip edge
	    			e->unlink(); e->mark();
	    			e = e->next_edge();
	    			*p = e;
				}
				//std::cout << n_re << " " << re << std::endl;
				//std::cout << x->var() << std::endl;
				assert(*it == e->val(x)->val());
				++it;
    			e->free();
    			p = e->next_edge_ref();
    			e = e->next_edge();
			}
			*p = NULL;
			while (e != NULL)
			{
	  			e->unlink(); e->mark();
	  			e = e->next_edge();
			}
			if (m->marked())
			{
	  			// Matching has been deleted!
	  			m->val(x)->matching(NULL);
	  			re[n_re++] = x;
			}
      	}
    }
    while (n_re--)
    	if (!match(re[n_re]))
			return false;
    return true;
}
} // Detail
} // CP
} // Casper

//#ifdef CASPER_PRECOMPILED
//#include <casper/cp/int/spexpr/explicit_postdom.h>
//#endif

namespace Casper {
namespace CP {


/**
 *	Enforces the
 *  all distinct constraint over a sequence of integer variables
 *  [\ref regin94].
 *
 *  \ingroup IntFilters
 */
template<class View>
struct DomFilterView1<Distinct,IntSeq,View> : IFilter
{
	DomFilterView1(Store& store, const View& v);

	struct VarDemon : INotifiable
	{
		VarDemon(DomFilterView1* pOwner,uint idx) : rOwner(*pOwner),
													idx(idx)
		{	rOwner.doms[idx]->attachOnDomain(this);	}
		bool notify()
		{	return rOwner.doms[idx]->ground() or rOwner.pParent->notify();}
		// tmp
		INotifiable* getFirst() { return rOwner.pParent; }

		DomFilterView1& rOwner;
		uint idx;
	};

	bool	execute();

	void 	attach(INotifiable* pParent);
	void 	detach();
	Cost 	cost() const {	return quadraticHi; }

	private:
	bool	init();
	//VarArray<int> vars;
	Store&						store;
	BacktrackSentinel 			btSentinel;
	typedef DomArrayView<int,View>	Doms;
	Doms						doms;
	Detail::VarValGraph<Doms>* 	vvg;
	INotifiable*				pParent;
};

template<class View>
DomFilterView1<Distinct,IntSeq,View>::DomFilterView1(Store& store, const View& v) :
		IFilter(store),
		store(store),
		btSentinel(store),
		doms(store,v),
		vvg(NULL)
{}

template<class View>
void DomFilterView1<Distinct,IntSeq,View>::attach(INotifiable* pParent)
{
	this->pParent = pParent;
	for (uint i = 0; i < doms.size(); i++)
		new (store) VarDemon(this,i);
		//vars[i].domain().attachOnDomain(this);


}

template<class View>
void DomFilterView1<Distinct,IntSeq,View>::detach()
{
	for (uint i = 0; i < doms.size(); i++)
		doms[i]->detachOnDomain(pParent);
}

template<class View>
bool DomFilterView1<Distinct,IntSeq,View>::init()
{
  	assert(vvg == NULL);
   	vvg = Detail::VarValGraph<Doms>::init(store,doms.size(),doms);
   	if ((vvg == NULL) || !vvg->initial_match())
     		return false;
     return true;
}


template<class View>
bool DomFilterView1<Distinct,IntSeq,View>::execute()
{
	#ifdef CASPER_LOG_OLD
	store.getEnv().log(this, "DomFilterView1<Distinct,IntSeq,View>", Util::Logger::filterExecuteBegin);
	#endif

	if (btSentinel.hasBacktracked())
		vvg = NULL;
	btSentinel.update();

	if (vvg == NULL)
	{
		if (!init())
      		return false;
	}
	else
	{
		if (!vvg->sync())
    		return false;
	}
    // propagate
 	vvg->mark();
  //  propTimer.resume();
    bool ret = vvg->tell();
 //	propTimer.pause();
    return ret;
}

template<class View>
struct PostDomFilter1<Distinct,Seq<int>,View>
{
	static bool post(Store& s,const View& v)
	{
		// Val(distinct) is required (not redundant in this implementation)
		// Bnd(distinct) seems to slow down sometimes.
		return postValFilter(s,distinct(v)) and
			   postBndFilter(s,distinct(v)) and
			   s.post(new (s) DomFilterView1<Distinct,Seq<int>,View>(s,v));
	}
};

} // CP
} // Casper

#endif /*USE_GECODE_DOM_ALLDIFF*/
