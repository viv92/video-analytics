/*
 *	The algorithm implemented here is greedily partition of the ground set into m blocks!
	In each iteration, choose a block with the minimum function value, and assign an item with the maximum marginal gain to this block. 
	Solves the problem \max_{\pi(V) = \{A_1,\dots,A_m\}} \min f(A_i), where f is a submodular function.
	Anthor: Kai Wei (kaiwei@uw.edu)
TODO: 
 *
 */
#ifndef DATA_PARTITION_GREEDY_UNIFORM
#define DATA_PARTITION_GREEDY_UNIFORM

#include "../../utils/error.h"
#include "../../utils/totalOrder.h"
#include "../discFunctions/SetFunctions.h"
#include "../discFunctions/SubmodularFunctions.h"
#include "../../representation/Set.h"

namespace datk {
    void dataPartitionGreedyUniform(SubmodularFunctions& f, std::vector<Set>& vecSets, int m_blocks, int verb);

}
#endif
