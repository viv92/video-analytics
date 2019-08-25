/*
 *	Implementation of the Majorization-Minimization Modular-Modular Algorithm for minimizing the difference between submodular functions.
	The problem is: \min_X f(X) - g(X), where f and g are submodular function.
	Uses the idea of iteratively minimizing upper bounds computed via subgradients and supergradients, and performs modular  at every round (Iyer-Bilmes, UAI-2012). It iteratively computes subgradients h_g^{X^t} and supergradients m^f_{X^t} at round t, and sets X^{t+1} = \min_X m^f_{X^t}(X) - h_g^{X^t}(X), which is a modular minimization!
	The parameter 'mode' in the algorithm gives the choice of the subgradient (0 is a random subgradient, and 1 (default) is an efficient heiristic for the subgradient).
	Author: Rishabh Iyer 
	Email: rkiyer@uw.edu
	Melodi Lab, University of Washington, Seattle
 *
 */

#include <unordered_set>
#include <queue>
#include <algorithm>
#include <iostream>
#include "stdio.h"
using namespace std;

#include "../utils/error.h"
#include "../utils/totalOrder.h"
#include "../functions/OperationsSubmodularFunctions.h"
#include "mminModMod.h"
#include "algoUtils.h"
namespace smtk {
	
template <class Container>
void modularmindiff(Container& optset, vector<double> mupper, vector<double> mlower, double lambda){
	optset.clear();
	for (int i = 0; i < mupper.size(); i++){
		if(mupper[i] - lambda*mlower[i] <= 0)
			optset.insert(i);
	}
	return;
}
template <class Container>
void mminModMod(SubmodularFunctions& f, SubmodularFunctions& g, Container& currset, double lambda, int verbosity, bool mode, Container startSet){
	DifferenceSubmodularFunctions h = f - lambda*g;
	std::vector<double> muppera;
	std::vector<double> mupperb;
	vector<double> subgradient;
	double moffseta = 0;
	double moffsetb = 0;
	muppera.assign(f.size(),0);
	mupperb.assign(f.size(),0);
	currset = startSet;
	Container newset;
	Container newseta;
	Container newsetb;
	while(1){
		if(mode == 1){
			getEfficientSubgradient(g, h, currset, subgradient);
		}
		else{
			getRandomSubgradient(g, currset, subgradient);
		}
		f.modularUpperBoundA(currset, muppera, moffseta);
		f.modularUpperBoundB(currset, mupperb, moffsetb);
		
		modularmindiff(newseta, muppera, subgradient, lambda);
		modularmindiff(newsetb, mupperb, subgradient, lambda);
		if (h.eval(newseta) < h.eval(newsetb))
			newset = newseta;
		else
			newset = newsetb;
		if(verbosity > 0) cout<<"Current Objective value is "<<h.eval(newset)<<"\n";
		if(h.eval(newset) - h.eval(currset) < -EPSILON){
			currset = newset;
		}
		else{
			break;
		}
	}
	return;
}

template void mminModMod(SubmodularFunctions& f, SubmodularFunctions& g, HashSet& currset, double lambda, int verbosity, bool mode, HashSet startSet);
}

