/*
 *	Header file for defining a modular function: f(X) = m(X) + c = \sum_{i \in X} m_i + c 
	Author: Rishabh Iyer
	Melodi Lab, University of Washington, Seattle
 *
 */
#include <iostream>
using namespace std;

#include "ModularFunctions.h"
namespace datk {
	ModularFunctions::ModularFunctions(const vector<double> weight, const double offset): SubmodularFunctions(weight.size()), weight(weight), offset(offset){}

	ModularFunctions::ModularFunctions(const ModularFunctions &m): SubmodularFunctions(m.n), weight(m.weight), offset(m.offset){}

	ModularFunctions::~ModularFunctions(){}

    ModularFunctions * ModularFunctions::clone() const{
		return new ModularFunctions(*this);
    }

	double ModularFunctions::eval(const Set& sset) const{
	// Evaluation of function valuation.
		double sum = offset;
		Set::const_iterator it;
		for( it = sset.begin(); it != sset.end(); ++it ) {
			sum+=weight[*it];
		}
	    return sum;
	}

	double ModularFunctions::evalFast(const Set& sset) const{
	// Evaluation of function valuation.
		return preCompute + offset;
	}

	double ModularFunctions::evalGainsadd(const Set& sset, int item) const{
		return weight[item];
	} 

	double ModularFunctions::evalGainsremove(const Set& sset, int item) const{
		return weight[item];
	} 

	double ModularFunctions::evalGainsaddFast(const Set& sset, int item) const{
		return weight[item];
	}

	double ModularFunctions::evalGainsremoveFast(const Set& sset, int item) const{
		return weight[item];

	}
	
	void ModularFunctions::updateStatisticsAdd(const Set& sset, int item) const{
		preCompute+=weight[item];
	}

	void ModularFunctions::updateStatisticsRemove(const Set& sset, int item) const{
		preCompute-=weight[item];
	}

	void ModularFunctions::setpreCompute(const Set& sset) const{
		clearpreCompute();
		Set::const_iterator it;
		for( it = sset.begin(); it != sset.end(); ++it ){
			updateStatisticsAdd(sset, *it);
		}
	}

	void ModularFunctions::clearpreCompute() const{
		preCompute = 0;
	}
}
