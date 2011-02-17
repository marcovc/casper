/*
 * testparser.cpp
 *
 *  Created on: May 4, 2010
 *      Author: serge
 */
#include "driver.h"
#include "print.h"
#include <iostream>


int main(int argc, char **argv){

	FlatzincDriver d(atoi(argv[1]), atoi(argv[2]), atof(argv[3]),atof(argv[4]));
	d.parse(argv[5]);
	SymStream s(std::cout, d.index);

	std::cout << "Variables: " << std::endl;
	s << d.variables << std::endl << std::endl << std::endl;

	std::cout << "Constraints: " << std::endl;
	s << d.constraints << std::endl;


	return 0;
}

