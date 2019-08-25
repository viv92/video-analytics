/*
	Copyright (C) Rishabh Iyer 2015

 *	Implementation of the Majorization-Minimization Supergradient Algorithm for submodular minimization subject to combinatorial constraints.
	Solve \min_{X: |X| \in \mathcal C} f(X), where f is a monotone submodular function, and \mathcal C is a solvable class of constraints.
	Currently this is implemented for the case of size constrained submodular minimization - \min_{X: |X| \geq k} f(X), where f is a monotone submodular function.
	
	Uses the idea of iteratively minimizing upper bounds computed via supergradients (Iyer-Jegelka-Bilmes, ICML-2013)
	Author: Rishabh Iyer
 *
TODO: Generalize this to other combinatorial constraints.
 */
#include <iostream>
#include<vector>
#include <algorithm>
#include <unordered_set>
#include <stdarg.h>
#include "stdio.h"

using namespace std;

#include "../../utils/sort.h"
#include "../../utils/totalOrder.h"
#include "../../utils/error.h"
#include "mminCC.h"

namespace datk {
	void modularmin(Set& optset, vector<double> mupper, double k){
	std::vector<size_t> indexmu;
	std::vector<double> sortedmu;
	sort(mupper, sortedmu, indexmu);
	optset.clear();
	for (int i = 0; i < k; ++i) {optset.insert(indexmu[i]);}
	return;
}

void mminCC(SubmodularFunctions& f, Set& currSet, double k, int verbosity, const Set& startSet){
	std::vector<double> muppera;
	std::vector<double> mupperb;
	double moffseta = 0;
	double moffsetb = 0;
	muppera.assign(f.size(),0);
	mupperb.assign(f.size(),0);
	currSet = startSet;
	Set newSet;
	Set newSeta;
	Set newSetb;
	int iter = 0;
	double preVal = 0;
	double currVal = 0;
	//std::vector<double> lastgains;
	//lastgains.assign(f.n, 0);
	//for(int i = 0; i < f.n; i++) lastgains[i] = f.evalGainsremoveFast(f.groundSet, i);
	while(1){
		f.modularUpperBoundA(currSet, muppera, moffseta);
		f.modularUpperBoundB(currSet, mupperb, moffsetb);
		modularmin(newSeta, muppera, k);
		modularmin(newSetb, mupperb, k);		
		if(f.eval(newSeta) < f.eval(newSetb))
			newSet = newSeta;
		else
			newSet = newSetb;	
		preVal = f.eval(currSet);
		currVal = f.eval(newSet);
		if(verbosity > 0) cout<<"Current value is "<< currVal<<" in iteration "<<iter<<"\n"<<flush;
		if (iter == 0){
			currSet = newSet;
		}
		else if (preVal - currVal > 0){
			currSet = newSet;
		}
		else{
			break;
		}
		iter++;
	}
	return;
}
}


