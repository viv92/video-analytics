/*
 *	Header file for the Majorization-Minimization Modular-Modular Algorithm for minimizing the difference between submodular functions.
	The problem is: \min_X f(X) - g(X), where f and g are submodular function.
	Uses the idea of iteratively minimizing upper bounds computed via subgradients and supergradients, and performs modular  at every round (Iyer-Bilmes, UAI-2012). It iteratively computes subgradients h_g^{X^t} and supergradients m^f_{X^t} at round t, and sets X^{t+1} = \min_X m^f_{X^t}(X) - h_g^{X^t}(X), which is a modular minimization!
	The parameter 'mode' in the algorithm gives the choice of the subgradient (0 is a random subgradient, and 1 (default) is an efficient heiristic for the subgradient).
	Author: Rishabh Iyer 
	Email: rkiyer@uw.edu
	Melodi Lab, University of Washington, Seattle
 *
 */
#ifndef SMTK_MMINModMod_INCLUDED
#define SMTK_MMINModMod_INCLUDED

#include "../functions/SetFunctions.h"
#include "../functions/SubmodularFunctions.h"
#include "../functions/DifferenceSubmodularFunctions.h"

namespace smtk {
template <class Container>
void mminModMod(SubmodularFunctions& f, SubmodularFunctions& g, Container& sset, double lambda, int verbosity = 1, bool mode = 1, Container startSet = Container());

extern template void mminModMod(SubmodularFunctions& f, SubmodularFunctions& g, HashSet& sset, double lambda, int verbosity = 1, bool mode = 1, HashSet startSet = HashSet());

}
#endif


