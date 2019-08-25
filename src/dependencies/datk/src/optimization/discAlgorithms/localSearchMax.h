/*
	Copyright (C) Rishabh Iyer 2015

*	The Local Search Algorithm for non-monotone submodular Maximization by Feige et al (2007). This gives a 1/3 approximation for unconstrained maximization.
	This is an accelerated version with priority queue (Minoux 1976).
	Solves the problem \max_{X \subseteq V} f(X), where f is a non-monotone submodular function. 
	Special case of this is the problem, \max_{|X| \leq k} f(X).
	Anthor: Rishabh Iyer
	Melodi Lab, University of Washington, Seattle
 *
 */
#ifndef DA_LOCALSEARCH
#define DA_LOCALSEARCH

#include "../discFunctions/SetFunctions.h"
#include "../discFunctions/SubmodularFunctions.h"
#include "../../representation/Set.h"


namespace datk {
template <class Container>
void localSearchMax(SubmodularFunctions& f, Container& localSet, int verbosity = 1, Container groundSet = Container(), bool grSetn = true);

extern template void localSearchMax(SubmodularFunctions& f, Set& localSet, int verbosity = 1, Set groundSet = Set(), bool grSetn = true);

}

#endif
