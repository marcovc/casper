/**************************************************************************\
 *   This file is part of CaSPER.                                          *
 *                                                                         *
 *   Copyright:                                                            *
 *   2011-2011 - Marco Correia <marco.v.correia@gmail.com>          	   *
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
 

#ifndef CASPER_KERNEL_STATE_H_
#define CASPER_KERNEL_STATE_H_

namespace casper {

struct State
{
	Counter countCPRestores;
	Counter countCPs;
	Counter maxLevel;
	Counter curLevel;

	/// Returns a unique identifier for the current state
	Counter stateID() const;

	/// Insert a check point (saves current state)
	void insertCP();
	/// Restore all changes occurred since last inserted check point
	void restoreCP();
	/// Removes last inserted check point
	void removeCP();
	void record(PTrailAgent);
	void notifyOnNextCP(INotifiable* f);

	/// Returns (reversible) heap
	Heap getHeap();
};

}

#endif /* CASPER_KERNEL_STATE_H_ */
