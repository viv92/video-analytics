/*
 *	Implementation of the Majorization-Minimization Supermodular-Submodular Algorithm for minimizing the difference between submodular functions.
	The problem is: \min_X f(X) - \lambda g(X), where f and g are submodular function. \lambda controls the tradeoff between f and g!
	Uses the idea of iteratively minimizing upper bounds computed via supergradients, and performs submodular maximization at every round (Iyer-Bilmes, UAI-2012, Narasimhan-Bilmes, UAI-2005). It iteratively computes subgradients h_g^{X^t} at round t, and sets X^{t+1} = \min_X f(X) - h_g^{X^t}(X), which is a submodular minimization!
	The parameter 'mode' in the algorithm controls the choice of the submodular maximization algorithm. mode = 0 (default) is the bidirectional greedy algorithm, and mode = 1 is the local search algorithm.
	Author: Rishabh Iyer 
	Email: rkiyer@uw.edu
	Melodi Lab, University of Washington, Seattle
 *
 */

#include <iostream>
#include <algorithm>
#include <queue>
#include <algorithm>
#include "stdio.h"
using namespace std;

#include "../utils/error.h"
#include "../utils/totalOrder.h"
#include "../functions/DifferenceSubmodularModularFunctions.h"
#include "../functions/ModularFunctions.h"
#include "../functions/OperationsSubmodularFunctions.h"
#include "bidirGreedyMax.h"
#include "localSearchMax.h"
namespace smtk {
#define EPSILON 1e-10
#define LARGE_NUM 1e50

template <class Container>
void mminSupSub(SubmodularFunctions& f, SubmodularFunctions& g, Container& currset, double lambda, int verbosity, bool mode, Container startSet){
	DifferenceSubmodularFunctions h  = f - lambda*g;
	std::vector<double> muppera;
	std::vector<double> mupperb;
	double moffseta = 0;
	double moffsetb = 0;
	muppera.assign(f.size(),0);
	mupperb.assign(f.size(),0);
	currset = startSet;
	Container newset;
	Container newseta;
	Container newsetb;
	vector<double> defCosts(f.size(), 0);
	while(1){
		f.modularUpperBoundA(currset, muppera, moffseta);
		f.modularUpperBoundB(currset, mupperb, moffsetb);
		ModularFunctions mfa (muppera, moffseta);
		ModularFunctions mfb (mupperb, moffsetb);
		DifferenceSubmodularModularFunctions cfa  = lambda*g- mfa;
		DifferenceSubmodularModularFunctions cfb = lambda*g - mfb;
		if(mode == 0){
			totalOrder<int> initOrder(f.size());
			bidirGreedyMax(cfa, newseta, initOrder, 0);
			bidirGreedyMax(cfb, newsetb, initOrder, 0);
		}
		else if (mode == 1){
			newseta = currset;
			newsetb = currset;
			localSearchMax(cfa, newseta, 0);
			localSearchMax(cfb, newsetb, 0);
		}
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

template void mminSupSub(SubmodularFunctions& f, SubmodularFunctions& g, HashSet& currset, double lambda, int verbosity, bool mode, HashSet startSet);

}