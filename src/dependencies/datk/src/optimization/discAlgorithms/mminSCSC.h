/*
	Copyright (C) Rishabh Iyer 2015

 *	Implementation of the Majorization-Minimization Supergradient Algorithm for Submodular Cost Submodular Knapsack (SCSK) Problem.
	The problem is: \max_{X: g(X) \geq C} f(X), where f and g are monotone submodular function.
	Uses the idea of iteratively minimizing upper bounds computed via supergradients (Iyer-Jegelka-Bilmes, ICML-2013)
	Author: Rishabh Iyer
	Melodi Lab, University of Washington, Seattle
 *
 */
#ifndef MMINSCSC_INCLUDED
#define MMINSCSC_INCLUDED

#include "../discFunctions/SetFunctions.h"
#include "../discFunctions/SubmodularFunctions.h"
#include "../../representation/Set.h"

namespace datk {
	void mminSCSC(SubmodularFunctions& f, SubmodularFunctions& g, Set& currset, double cover, int verbosity = 1, Set startSet = Set());


}
#endif


