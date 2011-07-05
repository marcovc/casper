/***************************************************************************
 *   Copyright (C) 2011/2011 by Marco Correia                              *
 *   mvc@di.fct.unl.pt                               					   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

#include <casper/cp/real.h>
#include <casper/cp/solver.h>
#include <casper/cp/view/chk.h>

using namespace Casper;
using namespace Casper::CP;

using namespace std;

int main()
{
	Solver solver;

	const double r = 0; // FIXME: given
	const double lambda = 0; // FIXME: given

	Var<double> chl(solver,0.1,50);
	Var<double> nppm(solver,0,50);
	Var<double> cdom(solver,0,5);

	const double a_chl_star = 0; // FIXME: in table
	BndExpr<double> a_chl(solver,chl * a_chl_star);

	BndExpr<double> b_chl(solver,0.3 * pow(chl,0.62)*(550/lambda));

	const double a_cdom_440 = 0; // FIXME: in table
	BndExpr<double> a_cdom(solver,a_cdom_440*exp(-0.017*(lambda-440)));

	const double a_nppm_star = 0.04;
	BndExpr<double> a_nppm(solver,nppm*a_nppm_star*exp(-0.0123*(lambda-440)));

	const double b_nppm_star_555 = 0.51;
	BndExpr<double> b_nppm(solver,nppm*b_nppm_star_555*pow(lambda/555,-0.5));

	const double aw = 0; // FIXME: in table

	BndExpr<double> a(solver,aw + a_chl + a_nppm + a_cdom);

	const double b_tilde = 0.0183;
	const double bw = 0; // FIXME: in table

	BndExpr<double> b(solver,b_tilde * (bw + b_chl + b_nppm));

	solver.post( r == 0.044 * b / (a+b) );

	cout << solver.valid() << endl;
}
