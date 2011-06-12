/**************************************************************************\
 *   This file is part of CaSPER.                                          *
 *                                                                         *
 *   Copyright:                                                            *
 *   2011-2011 - Marco Correia <marco.v.correia@gmail.com>           *
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
 

#include <casper/lp/linearexprrepr.h>
#include <casper/lp/var.h>

namespace Casper {
namespace LP {


LinearExprRepr::LinearExprRepr(const Var& var)
{
	vars.pushBack(var);
	coeffs.pushBack(1);
	free = 0;
}

LinearExprRepr::LinearExprRepr(const LinearExprRepr& e) : vars(e.vars), coeffs(e.coeffs),free(e.free)
{
	assert(vars.size()==coeffs.size());
}

LinearExprRepr::LinearExprRepr(double free) : free(free)
{}

const LinearExprRepr& LinearExprRepr::operator=(const Var& var)
{
	assert(vars.size()==coeffs.size());

	vars.clear();
	coeffs.clear();
	free = 0;

	vars.pushBack(var);
	coeffs.pushBack(1);
	return *this;
}

const LinearExprRepr& LinearExprRepr::operator=(double f)
{
	assert(vars.size()==coeffs.size());
	vars.clear();
	coeffs.clear();
	free = f;
	return *this;
}

const LinearExprRepr& LinearExprRepr::operator=(const LinearExprRepr& expr)
{
	assert(vars.size()==coeffs.size());

	vars = expr.vars;
	coeffs = expr.coeffs;
	free = expr.free;

	return *this;
}

const LinearExprRepr& LinearExprRepr::operator+=(const Var& var)
{
	assert(vars.size()==coeffs.size());

	// search for var in vars
	auto vit = vars.begin();
	auto cit = coeffs.begin();
	for ( ; vit != vars.end(); ++vit, ++cit)
		if (vit->id == var.id)
		{
			*cit += 1;
			return *this;
		}

	vars.pushBack(var);
	coeffs.pushBack(1);
	return *this;
}

const LinearExprRepr& LinearExprRepr::operator+=(double free)
{
	assert(vars.size()==coeffs.size());
	this->free += free;
	return *this;
}

const LinearExprRepr& LinearExprRepr::operator+=(const LinearExprRepr& expr)
{
	assert(vars.size()==coeffs.size());

	// search for var in vars
	auto vit1 = expr.vars.begin();
	auto cit1 = expr.coeffs.begin();
	for ( ; vit1 != expr.vars.end(); ++vit1, ++cit1)
	{
		auto vit2 = vars.begin();
		auto cit2 = coeffs.begin();

		for ( ; vit2 != vars.end(); ++vit2, ++cit2)
			if (vit2->id == vit1->id)
			{
				*cit2 += *cit1;
				break;
			}

		if (vit2 == vars.end())
		{
			vars.pushBack(*vit1);
			coeffs.pushBack(*cit1);
		}
	}

	free += expr.free;
	return *this;
}

const LinearExprRepr& LinearExprRepr::operator*=(const Var& var)
{
	assert(vars.size()==coeffs.size());
	if (vars.size()>0)
		throw Exception::NonLinear();
	vars.pushBack(var);
	coeffs.pushBack(free);
	free = 0;
	return *this;
}

const LinearExprRepr& LinearExprRepr::operator*=(double f)
{
	assert(vars.size()==coeffs.size());
	for (auto it = coeffs.begin(); it != coeffs.end(); ++it)
		*it *= f;
	free *= f;
	return *this;
}

const LinearExprRepr& LinearExprRepr::operator*=(const LinearExprRepr& expr)
{
	assert(vars.size()==coeffs.size());

	if (vars.size()==0)
	{
		for (auto it = expr.vars.begin(); it != expr.vars.end(); ++it)
			vars.pushBack(*it);
		for (auto it = expr.coeffs.begin(); it != expr.coeffs.end(); ++it)
			coeffs.pushBack(*it * free);
	}
	else
	if (expr.vars.size()==0)
		for (auto it = coeffs.begin(); it != coeffs.end(); ++it)
			*it *= expr.free;
	else
		throw Exception::NonLinear();

	free *= expr.free;
	return *this;
}

LinearExprRepr LinearExprRepr::operator+(const Var& var) const
{
	LinearExprRepr r(*this);
	r+=var;
	return r;
}

LinearExprRepr LinearExprRepr::operator+(double free) const
{
	LinearExprRepr r(*this);
	r+=free;
	return r;
}

LinearExprRepr LinearExprRepr::operator+(const LinearExprRepr& expr) const
{
	LinearExprRepr r(*this);
	r+=expr;
	return r;
}


LinearExprRepr LinearExprRepr::operator*(const Var& var) const
{
	LinearExprRepr r(*this);
	r*=var;
	return r;
}

LinearExprRepr LinearExprRepr::operator*(double free) const
{
	LinearExprRepr r(*this);
	r*=free;
	return r;
}

LinearExprRepr LinearExprRepr::operator*(const LinearExprRepr& expr) const
{
	LinearExprRepr r(*this);
	r*=expr;
	return r;
}

// maps to the above
const LinearExprRepr& LinearExprRepr::operator-=(const Var& var)
{	return (*this) += (-var);	}

const LinearExprRepr& LinearExprRepr::operator-=(double free)
{	return (*this) += (-free);	}

const LinearExprRepr& LinearExprRepr::operator-=(const LinearExprRepr& expr)
{	return (*this) += (-expr);	}

LinearExprRepr LinearExprRepr::operator-(const Var& var) const
{	return (*this) + (-var);	}

LinearExprRepr LinearExprRepr::operator-(double free) const
{	return (*this) + (-free);	}

LinearExprRepr LinearExprRepr::operator-(const LinearExprRepr& expr) const
{	return (*this) + (-expr);	}

const LinearExprRepr& LinearExprRepr::operator/=(double free)
{	return (*this) *= (1.0/free);	}

const LinearExprRepr& LinearExprRepr::operator/=(const LinearExprRepr& expr)
{	return (*this) *= (1.0/expr);	}

LinearExprRepr LinearExprRepr::operator/(double free) const
{	return (*this) * (1.0/free);	}

LinearExprRepr LinearExprRepr::operator/(const LinearExprRepr& expr) const
{	return (*this) * (1.0/expr);	}


LinearExprRepr operator-(const LinearExprRepr& expr)
{	return (-1.0) * expr;	}

LinearExprRepr operator/(double c, const LinearExprRepr& expr)
{
	if (expr.vars.size()>0)
		throw Exception::NonLinear();
	return c/expr.free;
}

LinearExprRepr operator*(double c, const LinearExprRepr& expr)
{	return expr * c; }

}
}

std::ostream& operator<<(std::ostream& os, const Casper::LP::LinearExprRepr& v)
{
	if (v.vars.size()>0)
	{
		auto vit = v.vars.begin();
		auto cit = v.coeffs.begin();
		os << *cit << "*" << "_" << vit->id;
		for ( ++vit, ++cit; vit != v.vars.end(); ++vit, ++cit)
			os << " + " << *cit << "*" << "_" << vit->id;
		os << " + ";
	}
	os << v.free;
	return os;
}
