/*
    Copyright (C) Rishabh Iyer 2015

 *	Combining two submodular functions h(X) = \sum_{i = 1}^k \lambda_i f_i(X), for two submodular functions f and g.
	Melodi Lab, University of Washington, Seattle
 *
 */

#include <iostream>
#include <assert.h>
using namespace std;

#include "CombineSubmodularFunctions.h"

namespace datk {
	CombineSubmodularFunctions::CombineSubmodularFunctions(int n, vector<SubmodularFunctions*> fvec_in, vector<double>& lambda): SubmodularFunctions(n), lambda(lambda){
		assert(lambda.size() == fvec_in.size());
		fvec = vector<SubmodularFunctions*>();
		for (int i = 0; i < fvec_in.size(); i++)
		{
			fvec.push_back(fvec_in[i]->clone());
		}
	}
	CombineSubmodularFunctions::CombineSubmodularFunctions(const CombineSubmodularFunctions &c): SubmodularFunctions(c), lambda(c.lambda){
		fvec = vector<SubmodularFunctions*>();
		for (int i = 0; i < c.fvec.size(); i++)
		{
			fvec.push_back(c.fvec[i]->clone());
		}
	}

    CombineSubmodularFunctions* CombineSubmodularFunctions::clone() const{
    	return new CombineSubmodularFunctions(*this);
    }

	CombineSubmodularFunctions::~CombineSubmodularFunctions(){
		for (int i =0; i< fvec.size();i++){
		     delete (fvec[i]);
		} 
		fvec.clear();	
	}
	
	double CombineSubmodularFunctions::eval(const Set& sset) const{
		double sumtot = 0;
		for (int i = 0; i < fvec.size(); i++){
			sumtot += fvec[i]->eval(sset);
		}
		return sumtot;
	}

	double CombineSubmodularFunctions::evalFast(const Set& sset) const{
		double sumtot = 0;
		for (int i = 0; i < fvec.size(); i++){
			sumtot += lambda[i]*fvec[i]->evalFast(sset);
		}
		return sumtot;
	}

	double CombineSubmodularFunctions::evalGainsadd(const Set& sset, int item) const{
		double gainsadd = 0;
		for (int i = 0; i < fvec.size(); i++){
			gainsadd += lambda[i]*fvec[i]->evalGainsadd(sset, item);
		}
		return gainsadd;
	}

	double CombineSubmodularFunctions::evalGainsremove(const Set& sset, int item) const{
		double gainsremove = 0;
		for (int i = 0; i < fvec.size(); i++){
			gainsremove += lambda[i]*fvec[i]->evalGainsremove(sset, item);
		}
		return gainsremove;
	}

	double CombineSubmodularFunctions::evalGainsaddFast(const Set& sset, int item) const{
		double gainsadd = 0;
		for (int i = 0; i < fvec.size(); i++){
			gainsadd += lambda[i]*fvec[i]->evalGainsaddFast(sset, item);
		}
		return gainsadd;
	}

	double CombineSubmodularFunctions::evalGainsremoveFast(const Set& sset, int item) const{
		double gainsadd = 0;
		for (int i = 0; i < fvec.size(); i++){
			gainsadd += lambda[i]*fvec[i]->evalGainsremoveFast(sset, item);
		}
		return gainsadd;
	}

	void CombineSubmodularFunctions::updateStatisticsAdd(const Set& sset, int item) const{
		for (int i = 0; i < fvec.size(); i++){
			fvec[i]->updateStatisticsAdd(sset, item);
		}
	}

	void CombineSubmodularFunctions::updateStatisticsRemove(const Set& sset, int item) const{
		for (int i = 0; i < fvec.size(); i++){
			fvec[i]->updateStatisticsRemove(sset, item);
		}
	}

	void CombineSubmodularFunctions::clearpreCompute() const{
		for (int i = 0; i < fvec.size(); i++){
			fvec[i]->clearpreCompute();
		}
	}

	void CombineSubmodularFunctions::setpreCompute(const Set& sset) const{
		for (int i = 0; i < fvec.size(); i++){
			fvec[i]->setpreCompute(sset);
		}
	}
}
