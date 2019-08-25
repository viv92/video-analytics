/*
	Copyright (C) Rishabh Iyer, 2015

 *	Combining a submodular function f and a modular function m: h(X) = \lambda_1 f(X) + \lambda_2 m(X). This function is submodular as long as \lambda_1 >= 0.
	Melodi Lab, University of Washington, Seattle
 *
 */

#include <vector>
#include <algorithm>
#include <iostream>
#include <assert.h>
#include "../../utils/error.h"
#include "../../utils/totalOrder.h"
using namespace std;


#include "DifferenceSubmodularModularFunctions.h"
namespace datk {

	DifferenceSubmodularModularFunctions::DifferenceSubmodularModularFunctions(const SubmodularFunctions& in_f, const ModularFunctions& in_m): SubmodularFunctions(in_f.size()){
		f = in_f.clone();
		m = in_m.clone();
	}

	DifferenceSubmodularModularFunctions::DifferenceSubmodularModularFunctions(const DifferenceSubmodularModularFunctions &d): 
	SubmodularFunctions(d.size()){
		f = d.f->clone();
		m = d.m->clone();
	}
	
    DifferenceSubmodularModularFunctions* DifferenceSubmodularModularFunctions::clone() const {
    	return new DifferenceSubmodularModularFunctions(*this);
    }


	double DifferenceSubmodularModularFunctions::eval(const Set& sset) const{
		return f->eval(sset) - m->eval(sset);
	}

	double DifferenceSubmodularModularFunctions::evalFast(const Set& sset) const{
		return f->evalFast(sset) - m->evalFast(sset);
	}

	double DifferenceSubmodularModularFunctions::evalGainsadd(const Set& sset, int item) const{
		return f->evalGainsadd(sset, item) - m->evalGainsadd(sset, item);
	}

	double DifferenceSubmodularModularFunctions::evalGainsremove(const Set& sset, int item) const{
		return f->evalGainsremove(sset, item) - m->evalGainsremove(sset, item);
	}

	double DifferenceSubmodularModularFunctions::evalGainsaddFast(const Set& sset, int item) const{
		return f->evalGainsaddFast(sset, item) - m->evalGainsaddFast(sset, item);
	}

	double DifferenceSubmodularModularFunctions::evalGainsremoveFast(const Set& sset, int item) const{
		return f->evalGainsremoveFast(sset, item) - m->evalGainsremoveFast(sset, item);
	}

	void DifferenceSubmodularModularFunctions::updateStatisticsAdd(const Set& sset, int item) const{
		f->updateStatisticsAdd(sset, item);
		m->updateStatisticsAdd(sset, item);
	}

	void DifferenceSubmodularModularFunctions::updateStatisticsRemove(const Set& sset, int item) const{
		f->updateStatisticsRemove(sset, item);
		m->updateStatisticsRemove(sset, item);
	}

	void DifferenceSubmodularModularFunctions::clearpreCompute() const{
		f->clearpreCompute();
		m->clearpreCompute();
	}

	void DifferenceSubmodularModularFunctions::setpreCompute(const Set& sset) const{
		f->setpreCompute(sset);
		m->setpreCompute(sset);
	}
}
