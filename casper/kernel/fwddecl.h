/*
 * fwddecl.h
 *
 *  Created on: Nov 4, 2011
 *      Author: marco
 */

#ifndef CASPER_KERNEL_FWDDECL_H_
#define CASPER_KERNEL_FWDDECL_H_

namespace Casper {
template<class> struct Par;
}

// required forward declarations
template<class T>
std::ostream& operator<<(std::ostream& s,const Casper::Par<T>&);


#endif /* CASPER_KERNEL_FWDDECL_H_ */
