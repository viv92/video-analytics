/*
 *	Implementation of the Minimum Norm Point Algorithm for Submodular Minimization.
	The problem is: \min_{X} f(X), where f is submodular.
	
	Author: Hui Lin (hlin@tsuri.ee.washington.edu)
	This was modified to fit into the framework of HSSFOTK.
	Melodi Lab, University of Washington, Seattle
 *
 */
#ifndef SMTK_MNP_INCLUDED
#define SMTK_MNP_INCLUDED

#include "../utils/totalOrder.h"
#include "../sets/Set.h"
#include "../sets/HashSet.h"
#include "../functions/SetFunctions.h"
#include "../functions/SubmodularFunctions.h"

namespace smtk {
template <class Container>
bool minNormPoint(SubmodularFunctions& f, Container& minSolSet, Container& maxSolSet, std::vector<double>& xstar, int verbosity = 1);

extern template bool minNormPoint(SubmodularFunctions& f, HashSet& minSolSet, HashSet& maxSolSet, std::vector<double>& xstar, int verbosity = 1);

}

#endif
