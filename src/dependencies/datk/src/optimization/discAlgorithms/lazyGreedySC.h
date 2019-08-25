/*
	Copyright (C) Rishabh Iyer 2015
 *	Header file for the accelerated greedy algorithm for submodular set cover (Minoux 1976, Wolsey 1982).
	Solves the problem \min_{f(X) \geq C} m(X), where f is a monotone submodular function and c is a modular cost function.
	This greedy algorithm has a log(n) approximation guarantee for this problem
	Author: Rishabh Iyer
 *
Input: Submodular Function: f
	Cost List: m
	cover: c
	GroundSet (in case it is not [n])
Output: GreedySet: greedySet
 */

#include "../discFunctions/SetFunctions.h"
#include "../discFunctions/SubmodularFunctions.h"
#include "../../representation/Set.h"

namespace datk {
void lazyGreedySC(SubmodularFunctions& f, std::vector<double>& costList, double cover, Set& greedySet, int verbosity = 1, 
double alpha = 1, Set groundSet = Set(), bool grSetn = true);
}
