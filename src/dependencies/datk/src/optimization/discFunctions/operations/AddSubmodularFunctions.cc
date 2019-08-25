/*

    Copyright (C) Rishabh Iyer 2015
 *	Combining two submodular functions h(X) = \sum_{i = 1}^k \lambda_i f_i(X), for two submodular functions f and g.
	Rishabh Iyer
	Melodi Lab, University of Washington, Seattle
 *
 */

#include <iostream>
#include <assert.h>
using namespace std;

#include "AddSubmodularFunctions.h"

namespace datk {
	AddSubmodularFunctions::AddSubmodularFunctions(const SubmodularFunctions& in_f, const SubmodularFunctions& in_g): SubmodularFunctions(in_f.size()){
		f = in_f.clone();
		g = in_g.clone();
		assert(f->size() == g->size());
	}
	AddSubmodularFunctions::AddSubmodularFunctions(const AddSubmodularFunctions &c): SubmodularFunctions(c){
		f = c.f->clone();
		g = c.g->clone();
	}

    AddSubmodularFunctions* AddSubmodularFunctions::clone() const{
    	return new AddSubmodularFunctions(*this);
    }


	double AddSubmodularFunctions::eval(const Set& sset) const{
		return f->eval(sset) + g->eval(sset);
	}

	double AddSubmodularFunctions::evalFast(const Set& sset) const{
		return f->evalFast(sset) + g->evalFast(sset);
	}

	double AddSubmodularFunctions::evalGainsadd(const Set& sset, int item) const{
		return f->evalGainsadd(sset, item) + g->evalGainsadd(sset, item);
	}

	double AddSubmodularFunctions::evalGainsremove(const Set& sset, int item) const{
		return f->evalGainsremove(sset, item) + g->evalGainsremove(sset, item);
		
	}

	double AddSubmodularFunctions::evalGainsaddFast(const Set& sset, int item) const{
		return f->evalGainsaddFast(sset, item) + g->evalGainsaddFast(sset, item);
	}

	double AddSubmodularFunctions::evalGainsremoveFast(const Set& sset, int item) const{
		return f->evalGainsremoveFast(sset, item) + g->evalGainsremoveFast(sset, item);
		
	}

	void AddSubmodularFunctions::updateStatisticsAdd(const Set& sset, int item) const{
			f->updateStatisticsAdd(sset, item);
			g->updateStatisticsAdd(sset, item);
	}

	void AddSubmodularFunctions::updateStatisticsRemove(const Set& sset, int item) const{
		f->updateStatisticsRemove(sset, item);
		g->updateStatisticsRemove(sset, item);
	}

	void AddSubmodularFunctions::clearpreCompute() const{
		f->clearpreCompute();
		g->clearpreCompute();
	}

	void AddSubmodularFunctions::setpreCompute(const Set& sset) const{
		f->setpreCompute(sset);
		g->setpreCompute(sset);
	}
}
