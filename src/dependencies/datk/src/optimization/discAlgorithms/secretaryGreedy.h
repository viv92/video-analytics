/*
 *	The algorithm implemented here is generalized version of lazier than lazy greedy algorithm!
	In each iteration, it samples a random subset R of size m, and run greedy to choose d items and add into the selected set, and keep until the budget is filled. 
	Solves the problem \max_{|X| \leq B} f(X), where f is a submodular function.
	Anthor: Kai Wei (kaiwei@uw.edu)
	Based on an implementation by Hui Lin. 
	Melodi Lab, University of Washington, Seattle
 *
 */
#ifndef SECRETARY_BASED_GREED
#define SECRETARY_BASED_GREED

#include "../discFunctions/SetFunctions.h"
#include "../discFunctions/SubmodularFunctions.h"
#include "../../representation/Set.h"

namespace datk {
	
    
void lazyGreedyMaxConstrainedSetSecretary(SubmodularFunctions& f, double budget, Set& selectedSet, Set& ConstrainedSet, int verbosity, Set& RemainingSet);

void secretaryGreedyMiniBatch(SubmodularFunctions& f, double k, Set& GreedySet, int d, int verbosity);

void secretaryGreedyMiniBatchOutofCore(SubmodularFunctions& f, double k, Set& GreedySet, int d, int verbosity);

}
#endif
