/*
 *	The randomized greedy algorithm for non-monotone submodular function maximization. 
	Solves the problem \max_{c(X) \leq B} f(X), where f is a non-monotone submodular function and k is the cardinality budget.
	Special case of this is the problem, \max_{|X| = k} f(X).
	Author: Rishabh Iyer
	Melodi Lab, University of Washington, Seattle

	Note: This algorithm is considerably slower than the accelerated greedy. It would be great if one could add acceleration tricks here.
 *
 */
#ifndef SMTK_RANDOMIZED_GREEDY
#define SMTK_RANDOMIZED_GREEDY
namespace smtk {
#define SMALLEST_NUMBER -1e70
#define LARGEST_NUMBER 1e70

std::unordered_set<int> randomizedGreedyMax(submodularFunctions& f, int k, std::unordered_set<int> startSet = std::unordered_set<int>(), int verbosity = 1);
}
#endif
