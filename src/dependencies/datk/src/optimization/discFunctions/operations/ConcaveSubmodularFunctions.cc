/*
 *	Concave over monotone submodular functions h(X) = \psi(f(X)), for a submodular functions f, and a concave function \psi.
	Melodi Lab, University of Washington, Seattle
 *
 */

#include <iostream>
#include <math.h>
#include "../../utils/error.h"
#include "../../utils/totalOrder.h"
using namespace std;

#include "ConcaveSubmodularFunctions.h"

namespace datk {
ConcaveSubmodularFunctions::ConcaveSubmodularFunctions(const SubmodularFunctions& in_f, const int type, const double thresh, const double exponent): 
SubmodularFunctions(in_f.size()), type(type), thresh(thresh), exponent(exponent) {
	f = in_f.clone();
	preCompute = 0;
}

ConcaveSubmodularFunctions::ConcaveSubmodularFunctions(const ConcaveSubmodularFunctions& c): 
SubmodularFunctions(c.n), type(c.type), thresh(c.thresh), exponent(c.exponent){
	f = c.f->clone();
	preCompute = 0;
}

ConcaveSubmodularFunctions* ConcaveSubmodularFunctions::clone() const
{
	return new ConcaveSubmodularFunctions(*this);
}

ConcaveSubmodularFunctions::~ConcaveSubmodularFunctions(){
	delete f;
}

double ConcaveSubmodularFunctions::concaveFunction(double K) const{
	switch(type)
	{
		case 1 :
			return sqrt(K);
		case 2 :
			return (1-1/(K+1));
		case 3 :
			return log(K);
		case 4 : 
			return std::min(K, thresh);
		case 5:
			return pow(K, exponent); 
		default :
			return sqrt(K);
	}
}

double ConcaveSubmodularFunctions::eval(const Set& sset) const{
	return concaveFunction(f->eval(sset));
}

double ConcaveSubmodularFunctions::evalFast(const Set& sset) const{
	return concaveFunction(preCompute);
}

double ConcaveSubmodularFunctions::evalGainsadd(const Set& sset, int item) const{
	double fval = f->eval(sset);
	double fgains = f->evalGainsadd(sset, item);
	return concaveFunction(fval + fgains) - concaveFunction(fval);
}

double ConcaveSubmodularFunctions::evalGainsremove(const Set& sset, int item) const{
	double fval = f->eval(sset);
	double fgains = f->evalGainsremove(sset, item);
	return concaveFunction(fval) - concaveFunction(fval - fgains);
}

double ConcaveSubmodularFunctions::evalGainsaddFast(const Set& sset, int item) const{
	double fval = preCompute;
	double fgains = f->evalGainsaddFast(sset, item);
	return concaveFunction(fval + fgains) - concaveFunction(fval);
}

double ConcaveSubmodularFunctions::evalGainsremoveFast(const Set& sset, int item) const{
	double fval = preCompute;
	double fgains = f->evalGainsremoveFast(sset, item);
	return concaveFunction(fval) - concaveFunction(fval - fgains);
}

void ConcaveSubmodularFunctions::updateStatisticsAdd(const Set& sset, int item) const{
	f->updateStatisticsAdd(sset, item);
	preCompute = f->evalFast(sset);
}

void ConcaveSubmodularFunctions::updateStatisticsRemove(const Set& sset, int item) const{
	f->updateStatisticsRemove(sset, item);
	preCompute = f->evalFast(sset);
}

void ConcaveSubmodularFunctions::clearpreCompute() const{
	f->clearpreCompute();
	preCompute = 0;
}

void ConcaveSubmodularFunctions::setpreCompute(const Set& sset) const{
	f->setpreCompute(sset);
	preCompute = f->evalFast(sset);
}
}
