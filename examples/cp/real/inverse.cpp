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
#include <casper/cp/goal/labeling.h>

#include <casper/cp/int.h>

#include <gsl/gsl_fit.h>

#include <map>

#include <list>

#include <fstream>

#include <math.h>


using namespace Casper;
using namespace Casper::CP;

using namespace std;

namespace Casper {

NEW_REL_1(myPow,MyPow)

namespace CP {

template<class Expr1,class Eval>
struct BndViewRel1<MyPow,Expr1,Eval>
{
	static const double ex;

	BndViewRel1(Store& store, const Expr1& p1) :
		p1(store,p1) {}
	Eval min() const
	{
		assert(p1.min()>0);
		return Util::powLb<Eval>(p1.min(),ex);
	}
	Eval max() const
	{
		assert(p1.min()>0);
		return Util::powUb<Eval>(p1.max(),ex);
	}

	bool updateMin(const Eval& v)
	{
		assert(v>0);
		return p1.updateMin(Util::powLb<Eval>(v,Util::divLb<Eval>(1.0,ex)));

	}
	bool updateMax(const Eval& v)
	{
		assert(v>0);
		return p1.updateMax(Util::powUb<Eval>(v,Util::divUb<Eval>(1.0,ex)));
	}

	void range(Eval& l,Eval& u) const {	l = min(); u = max(); }
	bool updateRange(const Eval& l, const Eval& u)
	{	return updateMin(l) and updateMax(u);	}

	void attach(INotifiable* f) { 	p1.attach(f); }
	void detach(INotifiable* f) {	p1.detach(f);}
	Rel1<MyPow,Expr1> getObj()  const
	{ 	return Rel1<MyPow,Expr1>(p1.getObj());	}

	BndView<Eval,Expr1>	p1;
};

template<class Expr1,class Eval>
const double BndViewRel1<MyPow,Expr1,Eval>::ex = 0.62;

} // CP
} // Casper

void getAChlSpecificData(const char* fname, map<int,double>& ma, map<int,double>& mb)
{
	ifstream f(fname);
	while (f.good())
	{
		int w;
		double a,b,r;
		f >> w >> a >> b >> r >> ws;
		ma[w] = a;
		mb[w] = b;
	}
	f.close();
}

void getH2OData(const char* fname, map<int,double>& m)
{
	ifstream f(fname);
	while (f.good())
	{
		int w;
		double v;
		f >> w >> v >> ws;
		m[w] = v;
	}
	f.close();
}

double linearInterpol(double xp, double xn, double yp, double yn, double x)
{
	double m = (yn-yp)/(xn-xp);
	double b = yp-m*xp;
	return m*x+b;
}
/*
double expInterpol(map<int,double> t, map<int,double> r, int x, double chl)
{
	const int window = 4;
	double ax,ay,sx,sy;
	auto eit = t.upper_bound(x);
	auto it = eit;
	for (int i = 0; i < window/2; ++i)
		--it;
	double xx[window];
	double yy[window];
	for (int i = 0; i < window; ++i)
	{
		xx[i] = it->first;
		yy[i] = it->second;
		++it;
	}
	double c0,c1;
	gsl_fit_linear(xx,1,yy,1,window,&c0,&c1,NULL,NULL,NULL,NULL);
	double A = c1*x+c0;

	double rr[window];
	eit = r.upper_bound(x);
	it = eit;
	for (int i = 0; i < window; ++i)
	{
		rr[i] = it->second;
		++it;
	}
	gsl_fit_linear(xx,1,rr,1,window,&c0,&c1,NULL,NULL,NULL,NULL);
	double B = c1*x+c0;

	return chl*A*::pow(chl,-B);
}*/

struct MyTermCond
{
	MyTermCond(double s, const BndExpr<bool> entailed) : s(s),entailed(entailed) {}
	template<class Eval,class View>
	bool operator()(const DomArrayView<Eval,View>& v) const
	{
		if (entailed.min()==entailed.max())
		{
			assert(entailed.min()==true);
			return true;
		}
		for (uint i = 0; i < v.size(); i++)
			if (v[i]->max()-v[i]->min() > s)
				return false;
		return true;
	}
	const double s;
	BndExpr<bool> entailed;
};

double gaussPrimitive(double x, double m, double s)
{
	return -s*erf((m-x)/(sqrt(2.0)*s))/(2*s);
}


struct Solution
{
	double chlMin,chlMax,nppmMin,nppmMax,cdomMin,cdomMax;
	vector<double> rMin;
	vector<double> rMax;
	double vol;
};


int solve(vector<int>& lambda, vector<double>& rr,
		  map<int,double>& ma, map<int,double>& mb,
		  map<int,double>& aw, map<int,double>& bw)
{
	const int n = lambda.size();

	Solver solver;

	Var<double> chl(solver,0.1,50);
	Var<double> nppm(solver,0,50);
	Var<double> a_cdom_440(solver,0,5);
	VarArray<double> r(solver,n,0,50);

	//BndExpr<bool> entailed(solver,true);

	for (int i = 0; i < n; ++i)
	{
		assert(ma.find(lambda[i])!=ma.end());
//		const double mac = expInterpol(ma,r,lambda)
		assert(aw.find(lambda[i])!=aw.end());
		assert(bw.find(lambda[i])!=bw.end());

		cout << "lambda = " << lambda[i] << endl;
		cout << "ma = " << ma[lambda[i]] << endl;
		cout << "aw = " << aw[lambda[i]] << endl;
		cout << "bw = " << bw[lambda[i]] << endl;
		const double a_chl_star = ma[lambda[i]];
		const double a_nppm_star = 0.04;
		const double b_nppm_star_555 = 0.51;
		const double b_tilde = 0.0183;

		BndExpr<double> a_chl(solver,chl * a_chl_star);
		BndExpr<double> b_chl(solver,0.3 * myPow(chl)*(550.0/lambda[i]));
		BndExpr<double> a_cdom(solver,a_cdom_440*exp(-0.017*(lambda[i]-440)));
		BndExpr<double> a_nppm(solver,nppm*a_nppm_star*exp(-0.0123*(lambda[i]-440)));
		BndExpr<double> b_nppm(solver,nppm*b_nppm_star_555*pow(lambda[i]/555.0,-0.5));
		BndExpr<double> a(solver,aw[lambda[i]] + a_chl + a_nppm + a_cdom);
		BndExpr<double> b(solver,b_tilde * (bw[lambda[i]] + b_chl + b_nppm));

		double factor;
		if (lambda[i]==670)
			factor = 0.006 * 3;
		else
			factor = 0.005 * 3;
		solver.post( r[i] == 0.044 * b / (a+b) );
		solver.post( rr[i]*(1-factor) <= r[i] );
		solver.post( rr[i]*(1+factor) >= r[i] );
		//entailed = entailed and (rr[i]*(1-0.06) <= 0.044 * b / (a+b) ) and (rr[i]*(1+0.06) >= 0.044 * b / (a+b));
	}

//	solver.post( chl == 1.0 );
//	solver.post( nppm == 0.373971 );
//	solver.post( a_cdom_440 == 0.011946 );


	bool found = solver.valid();

	VarArray<double> lvars(solver,3);
	lvars[0] = chl;
	lvars[1] = nppm;
	lvars[2] = a_cdom_440;
//	lvars[3] = r;

	std::list<Solution> solutions;

	// consider this many r's
	const int nr = 3;

	uint nbSolutions = 0;
	double totalVol = 0;

	cout << "first prop: " << found << endl;
	//MyTermCond termCond(0.01,entailed);
	SizeTermCond termCond(0.01);
	found = found and solver.solve(label(solver,lvars,selectVarMaxDom(solver,lvars,termCond),selectHalfMin(solver,lvars)));
	while (found)
	{
//		cout << lvars << " " << entailed << endl;

		double vol = (chl.domain().max()-chl.domain().min())*
					 (nppm.domain().max()-nppm.domain().min())*
					 (a_cdom_440.domain().max()-a_cdom_440.domain().min());
		for (int i = 0; i < nr; ++i)
			vol *= r[i].domain().max()-r[i].domain().min();
		totalVol+=vol;


		vector<double> rMin(nr);
		vector<double> rMax(nr);
		for (int i = 0; i < nr; ++i)
		{
			rMax[i] = r[i].domain().max();
			rMin[i] = r[i].domain().min();
		}


		Solution solution = {chl.domain().min(),chl.domain().max(),
							  nppm.domain().min(),nppm.domain().max(),
							  a_cdom_440.domain().min(),a_cdom_440.domain().max(),
							  rMin,
							  rMax,
							  vol};


		solutions.push_back(solution);

		cout << nbSolutions++ << " " << lvars << endl;

//		cout << "a_chl " << a_chl << endl;
//		cout << "b_chl " << b_chl << endl;
//		cout << "a_cdom " << a_cdom << endl;
//		cout << "a_nppm " << a_nppm << endl;
//		cout << "b_nppm " << b_nppm << endl;
//		cout << "a " << a << endl;
//		cout << "b " << b << endl;
		found = solver.next();
	}

	cout << solver.getStats() << endl;

	// compute p(r) marginal distributions (two steps)
	vector<map<double,double> > pr_prior_dist(nr);

	// step 1: compute increments
	for (int i = 0; i < nr; ++i)
	{
		for (auto it = solutions.begin(); it != solutions.end(); ++it)
		{
			const double p = (it->vol/totalVol)/(it->rMax[i]-it->rMin[i]);
			auto sit = pr_prior_dist[i].lower_bound(it->rMin[i]);
			if (sit!=pr_prior_dist[i].end() and sit->first == it->rMin[i])
				sit->second += p;
			else
				pr_prior_dist[i][it->rMin[i]] = p;
			sit = pr_prior_dist[i].lower_bound(it->rMax[i]);
			if (sit!=pr_prior_dist[i].end() and sit->first == it->rMax[i])
				sit->second -= p;
			else
				pr_prior_dist[i][it->rMax[i]] = -p;
		}
	}
	// step 2: accumulate increments
	for (int i = 0; i < nr; ++i)
	{
		double acc = 0;
		double acc_check = 0;
		double prev_first = 0;
		double prev_second = 0;
		for (auto sit = pr_prior_dist[i].begin() ; sit != pr_prior_dist[i].end(); ++sit)
		{
			acc += sit->second;
			sit->second = acc;
			acc_check += prev_second * (sit->first - prev_first);
			prev_first = sit->first;
			prev_second = sit->second;
		}
		assert(std::abs(acc)<=0.0001);
		assert(std::abs(acc_check-1)<=0.0001);
	}

	double acc=0;
	for (auto it = solutions.begin(); it != solutions.end(); ++it)
	{
		// compute prior joint probability p(xyz,r)
		double pjoint_prior = it->vol/totalVol;

		// compute (integrate) marginal p(r) for current r
		double pr_prior = 1;
		for (int i = 0; i < nr; ++i)
		{
			double pri_prior = 0;
			auto sit = pr_prior_dist[i].lower_bound(it->rMin[i]);
			assert(sit != pr_prior_dist[i].end() and sit->first == it->rMin[i]);
			auto prev_sit = sit;
			++sit;
			for ( ; sit != pr_prior_dist[i].upper_bound(it->rMax[i]); ++sit, ++prev_sit)
				pri_prior += prev_sit->second * (sit->first-prev_sit->first);
			assert(pri_prior>=0 and pri_prior<=1);
			pr_prior *= pri_prior;
		}
		assert(pr_prior>=0 and pr_prior<=1);

		// compute conditional p(xyz|r)
		double pcond = pjoint_prior/pr_prior;
		cout << pjoint_prior << "/" << pr_prior << "=" << pcond << endl;
		assert(pcond>=0 and pcond <= 1);

		// compute p(r) assuming normal distribution
		double pr_post = 1;
		for (int i = 0; i < nr; ++i)
		{
			double mean = rr[i];
			double stddev = ((lambda[i]==670)?0.006:0.005) * rr[i];

			if (it->rMax[i]>it->rMin[i])
				pr_post *= gaussPrimitive(it->rMax[i],mean,stddev)-gaussPrimitive(it->rMin[i],mean,stddev);
			else
				pr_post *= exp(-pow(it->rMax[i]-mean,2.0)/(2*stddev*stddev))/(stddev*sqrt(2*M_PI));
		}

		// compute new joint distribuion
		double pjoint_post = pcond*pr_post;
		acc += pjoint_post;
	}
	cout << "acc = " << acc << endl;

	return 1;

}

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		cerr << "usage: " << argv[0] << " n lambda_1...lambda_n r_1...r_n achlspecific.dat h2oattn.dat h2oabsp.dat achl*_1...achl*_n\n";
		return 1;
	}

	int n = atoi(argv[1]);

	vector<int> lambda(n);
	for (int i = 0; i < n; ++i)
		lambda[i] = atoi(argv[i+2]);

	vector<double> rr(n);
	for (int i = 0; i < n; ++i)
		rr[i] = atof(argv[i+2+n]);

	map<int,double> ma;
	map<int,double> mb;
#if 0
	//	getAChlSpecificData(argv[2+2*n],ma,mb);
#else
	// fix ma values from parameters
	for (int i = 0; i < n; ++i)
		ma[lambda[i]] = atof(argv[2+2*n+3+i]);
#endif

	map<int,double> aw;
	map<int,double> bw;
	getH2OData(argv[2+2*n+1],aw);
	getH2OData(argv[2+2*n+2],bw);

	// interpolate constant data
	for (int i = 0; i < n; ++i)
	{
		if (aw.find(lambda[i])==aw.end())
		{
			auto itn = aw.upper_bound(lambda[i]);
			assert(itn != aw.begin() && itn!=aw.end());
			auto itp = itn;
			--itp;
			aw[lambda[i]] = linearInterpol(itp->first,itn->first,itp->second,itn->second,lambda[i]);
		}

		if (bw.find(lambda[i])==bw.end())
		{
			auto itn = bw.upper_bound(lambda[i]);
			assert(itn != bw.begin() && itn!=bw.end());
			auto itp = itn;
			--itp;
			bw[lambda[i]] = linearInterpol(itp->first,itn->first,itp->second,itn->second,lambda[i]);
		}
	}

	for (auto it = bw.begin(); it != bw.end(); ++it)
		it->second -= aw[it->first];

	solve(lambda,rr,ma,mb,aw,bw);
}
