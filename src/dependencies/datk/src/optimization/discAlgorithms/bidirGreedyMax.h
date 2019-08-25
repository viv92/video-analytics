/*
	Copyright (C) Rishabh Iyer 2015
*	The Bi-directional Greedy Algorithm for non-monotone submodular Maximization by Buchbinder et al (2007). This gives a deterministic 1/3 approximation, and a randomized 1/2 approximation for unconstrained maximization.
	Solves the problem \max_{X \subseteq V} f(X), where f is a non-monotone submodular function. 
	Anthor: Rishabh Iyer
	Melodi Lab, University of Washington, Seattle
	The variable mode decides on the algorithm (randomized or deterministic greedy).
 *
 */
#ifndef DA_BIDIRGREEDY
#define DA_BIDIRGREEDY

#include "../../utils/totalOrder.h"
#include "../discFunctions/SetFunctions.h"
#include "../discFunctions/SubmodularFunctions.h"
#include "../../representation/Set.h"

namespace datk {
template <class Container>
void bidirGreedyMax(const SubmodularFunctions& f, Container& incSet, totalOrder<int> initialOrdering, int verbosity = 1, int mode = 1);

extern template void bidirGreedyMax<Set>(const SubmodularFunctions& f, Set& incSet, totalOrder<int> initialOrdering, int verbosity = 1, int mode = 1);
}
#endif
