/*
	Copyright (C) Rishabh Iyer 2015

 *	The algorithm implemented here is the forward greedy algorithm, and has a 1 - 1/e guarantee for monotone submodular maximization!
	It greedily adds elements untill it violates the budget (and the gain is non-negative). This is an accelerated version with priority queue (Minoux 1976).
	Solves the problem \max_{|X| \leq B} f(X), where f is a submodular function.
	Anthor: Rishabh Iyer
	Based on an implementation by Hui Lin. 
	Melodi Lab, University of Washington, Seattle

	Input: Submodular Function: f
		budget: b
		GroundSet (in case it is not [n])
	Output: GreedySet: greedySet

	Suggested choice of the Set datastructure: VectorSet or Set

 *
 */
#ifndef LAZY_GREEDY
#define LAZY_GREEDY

#include "../discFunctions/SetFunctions.h"
#include "../discFunctions/SubmodularFunctions.h"
#include "../../representation/Set.h"
namespace datk {
	
	void lazyGreedyMax(const SubmodularFunctions& f, double budget, Set& greedySet, int verbosity = 1, bool precomputeSet = false,
bool equalityConstraint = false, Set groundSet = Set(), bool grSetn = true); 
}
#endif
