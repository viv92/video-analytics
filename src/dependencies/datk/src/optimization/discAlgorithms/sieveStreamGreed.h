/*
	Copyright (C) Kai Wei 2015
 *	The algorithm implemented here is generalized version of the sieve streaming greedy algorithm!
    Randomly order the whole data set, and then access the items in a sequential manner, we also extend to the minibatch streaming setting. 
	In each iteration, when a minibatch of size m comes in, we choose a set of d items using the greedy algorithm.  
	Solves the problem \max_{|X| \leq B} f(X), where f is a submodular function.
	Anthor: Kai Wei (kaiwei@uw.edu)
 *
 */
#ifndef SIEVESTREAM_GREED
#define SIEVESTREAM_GREED

#include "../discFunctions/SetFunctions.h"
#include "../discFunctions/SubmodularFunctions.h"
#include "../discFunctions/FacilityLocationSparse.h"

namespace datk {
	
void sieveStreamGreedyMiniBatch(SubmodularFunctions& f, double k, Set& GreedySet, int m, int verbosity, double eps);
void lazyGreedyMaxConstrainedSieve(SubmodularFunctions& f, double budget, Set& selectedSet, Set& ConstrainedSet, int verbosity, double nu_over_2, double& func_val);

}
#endif
