/*
 * fwddecl.h
 *
 *  Created on: Nov 4, 2011
 *      Author: marco
 */

#ifndef CASPER_CP_FWDDECL_H_
#define CASPER_CP_FWDDECL_H_

namespace Casper {
namespace CP {
template<class,class> struct Var;

}
}

// required forward declarations
template<class T,class D>
std::ostream& operator<<(std::ostream& s,const Casper::CP::Var<T,D>&);


#endif /* CASPER_CP_FWDDECL_H_ */
