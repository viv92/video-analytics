/*
	Copyright (C) Rishabh Iyer, 2015
 *	Difference of two submodular functions h(X) = \lambda_1 f(X) - \lambda_2 g(X), for two submodular functions f and g.
	Melodi Lab, University of Washington, Seattle
 *
 */
#include <iostream>
using namespace std;

#include "DifferenceSubmodularFunctions.h"

namespace datk {
DifferenceSubmodularFunctions::DifferenceSubmodularFunctions(const SubmodularFunctions& in_f, const SubmodularFunctions& in_g): 
SetFunctions(in_f.size()){
	f = in_f.clone();
	g = in_g.clone();
}

DifferenceSubmodularFunctions::DifferenceSubmodularFunctions(const DifferenceSubmodularFunctions &d): SetFunctions(d.n){
	f = d.f->clone();
	g = d.f->clone();
}

DifferenceSubmodularFunctions::~DifferenceSubmodularFunctions(){}

DifferenceSubmodularFunctions * DifferenceSubmodularFunctions::clone() const{
	return new DifferenceSubmodularFunctions(*this);
}


double DifferenceSubmodularFunctions::eval(const Set& sset) const{
	return f->eval(sset) - g->eval(sset);
}

double DifferenceSubmodularFunctions::evalFast(const Set& sset) const{
	return f->evalFast(sset) - g->evalFast(sset);
}

double DifferenceSubmodularFunctions::evalGainsadd(const Set& sset, int item) const{
	return f->evalGainsadd(sset, item) - g->evalGainsadd(sset, item);
}

double DifferenceSubmodularFunctions::evalGainsremove(const Set& sset, int item) const{
	return f->evalGainsremove(sset, item) - g->evalGainsremove(sset, item);
}

double DifferenceSubmodularFunctions::evalGainsaddFast(const Set& sset, int item) const{
	return f->evalGainsaddFast(sset, item) - g->evalGainsaddFast(sset, item);
}

double DifferenceSubmodularFunctions::evalGainsremoveFast(const Set& sset, int item) const{
	return f->evalGainsremoveFast(sset, item) - g->evalGainsremoveFast(sset, item);
}

void DifferenceSubmodularFunctions::updateStatisticsAdd(const Set& sset, int item) const{
	f->updateStatisticsAdd(sset, item);
	g->updateStatisticsAdd(sset, item);
}

void DifferenceSubmodularFunctions::updateStatisticsRemove(const Set& sset, int item) const{
	f->updateStatisticsRemove(sset, item);
	g->updateStatisticsRemove(sset, item);
}

void DifferenceSubmodularFunctions::clearpreCompute() const{
	f->clearpreCompute();
	g->clearpreCompute();
}

void DifferenceSubmodularFunctions::setpreCompute(const Set& sset) const{
	f->setpreCompute(sset);
	g->setpreCompute(sset);
}

void DifferenceSubmodularFunctions::resetData(std::vector<int> Rset){}

}
