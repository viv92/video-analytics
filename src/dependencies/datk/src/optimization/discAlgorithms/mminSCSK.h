/*
	Copyright (C) Rishabh Iyer 2015

 *	Implementation of the Majorization-Minimization Supergradient Algorithm for Submodular Cost Submodular Knapsack (SCSK) Problem.
	The problem is: \max_{X: f(X) \leq B} g(X), where f and g are monotone submodular function.
	Uses the idea of iteratively minimizing upper bounds computed via supergradients (Iyer-Jegelka-Bilmes, ICML-2013)
	Author: Rishabh Iyer
	Melodi Lab, University of Washington, Seattle
 *
 */
#ifndef MMINSCSK_INCLUDED
#define MMINSCSK_INCLUDED

#include "../discFunctions/SetFunctions.h"
#include "../discFunctions/SubmodularFunctions.h"
#include "../../representation/Set.h"

namespace datk {
void mminSCSK(SubmodularFunctions& f, SubmodularFunctions& g, Set& currset, double budget, int verbosity = 1, Set startSet = Set());


}
#endif


