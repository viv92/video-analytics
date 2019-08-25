/*
	Copyright (C) Rishabh Iyer 2015

 *	Implementation of the Majorization-Minimization Supergradient Algorithm for submodular minimization subject to combinatorial constraints.
	Solve \min_{X: |X| \in \mathcal C} f(X), where f is a monotone submodular function, and \mathcal C is a solvable class of constraints.
	Currently this is implemented for the case of size constrained submodular minimization - \min_{X: |X| \geq k} f(X), where f is a monotone submodular function.
	
	Uses the idea of iteratively minimizing upper bounds computed via supergradients (Iyer-Jegelka-Bilmes, ICML-2013)
	Author: Rishabh Iyer
	Melodi Lab, University of Washington, Seattle
 *
TODO: Generalize this to other combinatorial constraints.
 */
#ifndef MMIN_INCLUDED
#define MMIN_INCLUDED

#include "../discFunctions/SetFunctions.h"
#include "../discFunctions/SubmodularFunctions.h"
#include "../../representation/Set.h"

namespace datk {
	void mminCC(SubmodularFunctions& f, Set& currSet, double k, int verbosity = 1, Set startSet = Set());
}
#endif


