/*

	Copyright (C) Rishabh Iyer 2015
 *	The naive greedy algorithm for set function maximization. This algorithm has guarantees if the function is monotone submodular, under cardinality/knapsack or matroid constraints. In general, though, it is a useful heuristic, which seems to work well in practice.
	Solves the problem \max_{|X| \leq B} f(X), where f is a monotone submodular function.
	Author: Rishabh Iyer
	Melodi Lab, University of Washington, Seattle

	Note: This algorithm is considerably slower than the accelerated greedy, so if you have a reason to believe your function is submodular (or approximately so), 		please use the lazy greedy algorithm.

	Input: Set Function: f (not necessarily submodular)
		budget: b
		GroundSet (in case it is not [n])
	Output: GreedySet: greedySet
 *
 */
#ifndef __NAIVE_GREEDY_MAX_ORDERING_h
#define __NAIVE_GREEDY_MAX_ORDERING_h

#include "../discFunctions/SetFunctions.h"
#include "../../representation/Set.h"

namespace datk {

void naiveGreedyMaxOrdering(SetFunctions& f, Set& greedySet, std::vector<int>& greedyOrdering, int verbosity = 1);

}
#endif
