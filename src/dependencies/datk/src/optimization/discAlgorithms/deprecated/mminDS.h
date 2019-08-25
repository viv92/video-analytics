/*
 *	Implementation of the Majorization-Minimization Submodular-Supermodular Algorithm for minimizing the difference between submodular functions.
	The problem is: \min_X f(X) - \lambda g(X), where f and g are submodular function. \lambda controls the tradeoff between f and g!
	Uses the idea of iteratively minimizing upper bounds computed via subgradients, and performs submodular minimization at every round (Iyer-Bilmes, UAI-2012, Narasimhan-Bilmes, UAI-2005). It iteratively computes subgradients h_g^{X^t} at round t, and sets X^{t+1} = \min_X f(X) - h_g^{X^t}(X), which is a submodular minimization!
	The parameter 'mode' in the algorithm gives the choice of the subgradient (0 is a random subgradient, and 1 (default) is an efficient heiristic for the subgradient).
	Author: Rishabh Iyer 
	Email: rkiyer@uw.edu
	Melodi Lab, University of Washington, Seattle
 *
 */
#ifndef SMTK_MMIN_DS_h
#define SMTK_MMIN_DS_h

#include "../functions/SetFunctions.h"
#include "../functions/SubmodularFunctions.h"
#include "../functions/DifferenceSubmodularFunctions.h"
#include "../functions/OperationsSubmodularFunctions.h"
#include "../functions/ModularFunctions.h"

namespace smtk {
template <class Container>
void mminDS(SubmodularFunctions& f, SubmodularFunctions& g, Container& minSet, double lambda, int verbosity = 1);

extern template void mminDS(SubmodularFunctions& f, SubmodularFunctions& g, HashSet& minSet, double lambda, int verbosity);

}
#endif