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
#include "../functions/DifferenceSubmodularFunctions.h"
#include "../functions/OperationsSubmodularFunctions.h"
#include "../functions/ModularFunctions.h"
#include "../functions/DifferenceSubmodularModularFunctions.h"
#include "lazyGreedyMaxKnapsack.h"
#include "minNormPoint.h"
#include "algoUtils.h"

namespace smtk {

template <class Container>
void mminSubSup(SubmodularFunctions& f, SubmodularFunctions& g, Container& currset, double lambda, int verbosity, bool mode, Container startSet){
	DifferenceSubmodularFunctions h = f - lambda*g;
	currset = startSet;
	Container newset1 = currset;
	Container newset2 = currset;
	std::vector<double> xstar;
	Container groundSet = Container(f.size(), true);
	while(1){
		vector<double> subgradient;
		if(mode == 1)
			getEfficientSubgradient(g, f, currset, subgradient);
		else
			getRandomSubgradient(g, currset, subgradient);
		ModularFunctions mgh (subgradient, 0);
		for (int i = 0; i < f.size(); i++)
			subgradient[i] = subgradient[i]*lambda;
		DifferenceSubmodularModularFunctions cfg = f - mgh;
		minNormPoint(cfg, newset1, newset2, xstar, 1);
		if(verbosity > 0) cout<<"Current Objective value is "<<h.eval(newset1)<<"\n";
		if(h.eval(newset1) - h.eval(currset) < -EPSILON){
			currset = newset1;
		}
		else{
			break;
		}
	}
	return;
}

template void mminSubSup(SubmodularFunctions& f, SubmodularFunctions& g, HashSet& currset, double lambda, int verbosity, bool mode, HashSet startSet);

}