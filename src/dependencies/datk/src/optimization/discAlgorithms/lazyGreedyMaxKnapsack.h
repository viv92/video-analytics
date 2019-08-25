/*
	Copyright (C) Rishabh Iyer 2015
 *	The algorithm implemented here is the forward greedy algorithm, and has a 1 - 1/e guarantee for monotone submodular maximization!
	It greedily adds elements untill it violates the budget (and the gain is non-negative). This is an accelerated version with priority queue (Minoux 1976).
	Solves the problem \max_{c(X) \leq B} f(X), where f is a submodular function and c is a modular cost function.
	Special case of this is the problem, \max_{|X| \leq k} f(X).
	Anthor: Rishabh Iyer
	Based on an implementation by Hui Lin. 
	Melodi Lab, University of Washington, Seattle

	Input: Submodular Function: f
		budget: b
		GroundSet (in case it is not [n])	
	Output: GreedySet: greedySet
 *
 */
#ifndef LAZY_GREEDY_KNAPSACK
#define LAZY_GREEDY_KNAPSACK

#include "../discFunctions/SetFunctions.h"
#include "../discFunctions/SubmodularFunctions.h"
#include "../../representation/Set.h"

namespace datk {
void lazyGreedyMaxKnapsack(SubmodularFunctions& f, std::vector<double>& costList, double budget, Set& greedySet, 
int verbosity = 1, double alpha = 1, Set groundSet = Set(), bool grSetn = true); 
}
#endif
