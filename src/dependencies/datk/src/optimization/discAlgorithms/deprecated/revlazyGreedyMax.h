/*
 *	The reverse greedy algorithm starting with the groundset and greedily removing elements until there is no benefit in removing elements any more. 	
	This is an accelerated version with priority queue (Minoux 1976).
	Solves the problem \max_{X \subseteq V} f(X), where f is a non-monotone submodular function.
	Anthor: Rishabh Iyer
	Melodi Lab, University of Washington, Seattle

	Input: Submodular Function: f
		budget: b
	Output: GreedySet: greedySet

	This algorithm proceeds by greedily removing elements from the the set greedySet, and updates itself till the greedy algorithm converges
 *
 */

#include "../functions/SetFunctions.h"
#include "../functions/SubmodularFunctions.h"
#include "../sets/Set.h"
#include "../sets/HashSet.h"

#ifndef SMTK_REV_LAZY_GREEDY
#define SMTK_REV_LAZY_GREEDY

namespace smtk {
void revlazyGreedyMax(SubmodularFunctions& f, Set& greedySet, int verbosity=1, bool precomputeSet = false);
}

#endif
