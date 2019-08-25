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

 *
 */
#ifndef STREAM_GREEDY_H
#define STREAM_GREEDY_H

#include "../discFunctions/SetFunctions.h"
#include "../discFunctions/SubmodularFunctions.h"
#include "../../representation/Set.h"
namespace datk {
	
	void streamGreedy(const SetFunctions& f, double epsilon, Set& greedySet, std::vector<int>& stream, int verbosity = 1);
}
#endif
