/*
 *	Implementation of the Majorization-Minimization Submodular-Supermodular Algorithm for minimizing the difference between submodular functions.
	The problem is: \min_X f(X) - \lambda g(X), where f and g are submodular function. \lambda controls the tradeoff between f and g!
	Uses the idea of iteratively minimizing upper bounds computed via subgradients, and performs submodular minimization at every round (Iyer-Bilmes, UAI-2012, Narasimhan-Bilmes, UAI-2005). It iteratively computes subgradients h_g^{X^t} at round t, and sets X^{t+1} = \min_X f(X) - h_g^{X^t}(X), which is a submodular minimization!
	The parameter 'mode' in the algorithm gives the choice of the subgradient (0 is a random subgradient, and 1 (default) is an efficient heiristic for the subgradient).
	Author: Rishabh Iyer 
	Email: rkiyer@uw.edu
	Melodi Lab, University of Washington, Seattle
 *
 */
#include <unordered_set>
#include <iostream>
#include <algorithm>
#include <queue>
#include "stdio.h"
using namespace std;

#include "../utils/totalOrder.h"
#include "lazyGreedyMaxKnapsack.h"
#include "minNormPoint.h"
#include "mminDS.h"
#include "mminModMod.h"
#include "mminSupSub.h"
#include "mminSubSup.h"


namespace smtk {
	
template <class Container>
void mminDS(SubmodularFunctions& f, SubmodularFunctions& g, Container& bestSet, double lambda, int verbosity){
	DifferenceSubmodularFunctions h  = f - lambda*g;
	// Modular-Modular Procedure
	Container mminModModSet;
	Container mminSupSubSet;
	Container mminSubSupSet;
	mminModMod(f, g, mminModModSet, lambda, verbosity, 1);
	cout<<"ModMod is f(X) = "<< f.eval(mminModModSet)<<", g(X) = " << g.eval(mminModModSet)<<", and h(X) = " << h.eval(mminModModSet)<<"\n";
	
	// Supermodular-Submodular Procedure
	mminSupSub(f, g, mminSupSubSet, lambda, verbosity, 0);
	cout<<"SupSub is f(X) = "<< f.eval(mminSupSubSet)<<", g(X) = " << g.eval(mminSupSubSet)<<", and h(X) = " << h.eval(mminSupSubSet)<<"\n";

	// Submodular-Supermodular Procedure
	mminSubSup(f, g, mminSubSupSet, lambda, verbosity, 1);
	cout<<"SubSup is f(X) = "<< f.eval(mminSubSupSet)<<", g(X) = " << g.eval(mminSubSupSet)<<", and h(X) = " << h.eval(mminSubSupSet)<<"\n";
	
	bestSet = mminModModSet;
	if (h.eval(bestSet) < h.eval(mminSupSubSet))
		bestSet = mminSupSubSet;
	if (h.eval(bestSet)  < h.eval(mminSubSupSet))
		bestSet = mminSubSupSet;
	return;
}

template void mminDS(SubmodularFunctions& f, SubmodularFunctions& g, HashSet& minSet, double lambda, int verbosity);
}