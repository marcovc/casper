 /*************************************************************************\
 *   This file is part of CaSPER (http://proteina.di.fct.unl.pt/casper).   *
 *                                                                         *
 *   Copyright:                                                            *
 *   2008-2011 - Marco Correia <marco.v.correia@gmail.com>                 *
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

#ifndef CASPER_KERNEL_CONTAINER_STDTRIE_H_
#define CASPER_KERNEL_CONTAINER_STDTRIE_H_

namespace Casper {
namespace Util {


template<class T>
struct StdTrie
{
	struct NodeI
	{
		NodeI(NodeI* pParent) :
			pParent(pParent),
			pLChild(NULL), pRSibling(NULL) {}
		NodeI*	pParent;
		NodeI*	pLChild;
		NodeI*	pRSibling;
	};

	struct Node : NodeI
	{
		Node(const T& value, NodeI* pParent) :
			NodeI(pParent),value(value) {}
		const T value;
	};

	typedef NodeI*			PNode;
	typedef StdArray<PNode>	Levels;

	StdTrie(uint levels) :
		levels(stdHeap,levels,static_cast<NodeI*>(NULL)),
		pDelim(new (stdHeap) NodeI(static_cast<NodeI*>(NULL))) {}
	StdTrie(IHeap& h,uint levels) :
		levels(h,levels,static_cast<NodeI*>(NULL)),
		pDelim(new (h) NodeI(static_cast<NodeI*>(NULL))) {}


	bool isDelim(PNode p) const
	{	return !p or p->pParent == pDelim;	}

	const T& value(PNode p) const
	{	return static_cast<Node*>(p)->value;	}

	uint maxDepth() const
	{	return levels.size();	}

	template<class InputIterator>
	void insert(InputIterator b, InputIterator e)
	{
		//std::cout << "inserting ";
		//std::copy(b,e,std::ostream_iterator<int>(std::cout," "));
		//std::cout << std::endl;
		uint curLev = 0;
		PNode pLastNode = NULL;
		PNode pCurNode = levels[0];

		// skip common prefix
		while (b != e)
		{
			for ( ; !isDelim(pCurNode) ; pCurNode = pCurNode->pRSibling)
				if (value(pCurNode) == *b)
					break;
			if (isDelim(pCurNode))
				break;
			pLastNode = pCurNode;
			pCurNode = pCurNode->pLChild;
			++curLev;
			++b;
		}

		// store the rest in a new branch
		while (b != e)
		{
			pCurNode = new (levels.getHeap()) Node(*b,pLastNode);
			if (!pLastNode)
			{
				assert(curLev == 0);
				PNode p = levels[curLev];
				if (!isDelim(p) and value(p) < *b)
				{
					while (  !isDelim(p->pRSibling) and
							 value(p->pRSibling) < *b )
						p = p->pRSibling;
					pCurNode->pRSibling = p->pRSibling;
					p->pRSibling = pCurNode;
				}
				else
				{
					pCurNode->pRSibling = p;
					levels[curLev] = pCurNode;
				}
			}
			else
			if (!pLastNode->pLChild)
			{
				if (levels[curLev])
				{
					PNode pDelimNode = new (levels.getHeap()) NodeI(pDelim);
					pDelimNode->pRSibling = levels[curLev];
					pCurNode->pRSibling = pDelimNode;
				}
				levels[curLev] = pCurNode;
				pLastNode->pLChild = pCurNode;
			}
			else
			{
				PNode p = levels[curLev];

				if (levels[curLev] == pLastNode->pLChild and
					 value(levels[curLev]) >= *b)
				{
					pCurNode->pRSibling = levels[curLev];
					levels[curLev] = pCurNode;
				}
				else
				{
					if (p != pLastNode->pLChild)
						while ( p->pRSibling != pLastNode->pLChild )
							p = p->pRSibling;

					while (  !isDelim(p->pRSibling) and
							 value(p->pRSibling) < *b )
						p = p->pRSibling;
					pCurNode->pRSibling = p->pRSibling;
					p->pRSibling = pCurNode;
				}
				if (pCurNode->pRSibling == pLastNode->pLChild)
					pLastNode->pLChild = pCurNode;
			}
			pLastNode = pCurNode;
			++b;
			++curLev;
		}
	}
	
	void insert(const StdList<T>& list)
	{	insert(list.begin(),list.end());	}

	Levels		levels;
	const PNode	pDelim;
};

} // Util
} // Casper


/**
	Writes the contents of the StdTrie to an ostream (in graphviz format)
*/
template<typename T>
std::ostream& operator<<(std::ostream& o, const Casper::Util::StdTrie<T>& s)
{
	typedef Casper::Util::StdTrie<T> Tr;
	o << "strict digraph { " << std::endl;
	o << "root" << std::endl;
	for (typename Tr::PNode p = s.levels[0]; p != NULL; p = p->pRSibling)
		if (!s.isDelim(p))
		{
			o << (uint)p << " [label="
			  << static_cast<typename Tr::Node*>(p)->value << "]\n";
			o << "root -> " << (uint)p << std::endl;
		}
	for (uint i = 1; i < s.levels.size(); i++)
		for (typename Tr::PNode p = s.levels[i]; p != NULL; p = p->pRSibling)
			if (!s.isDelim(p))
			{
				o << (uint)p->pParent << " -> " << (uint)p << std::endl;
				o << (uint)p << " [label="
				  << static_cast<typename Tr::Node*>(p)->value << "]\n";
			}
	o << "}" << std::endl;
	return o;
}

#endif /*CASPER_KERNEL_CONTAINER_TRIE_H_*/
