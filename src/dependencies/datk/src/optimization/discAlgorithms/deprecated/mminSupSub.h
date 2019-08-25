/*
 *	Implementation of the Majorization-Minimization Supermodular-Submodular Algorithm for minimizing the difference between submodular functions.
	The problem is: \min_X f(X) - \lambda g(X), where f and g are submodular function. \lambda controls the tradeoff between f and g!
	Uses the idea of iteratively minimizing upper bounds computed via supergradients, and performs submodular maximization at every round (Iyer-Bilmes, UAI-2012, Narasimhan-Bilmes, UAI-2005). It iteratively computes subgradients h_g^{X^t} at round t, and sets X^{t+1} = \min_X f(X) - h_g^{X^t}(X), which is a submodular minimization!
	The parameter 'mode' in the algorithm controls the choice of the submodular maximization algorithm. mode = 0 (default) is the bidirectional greedy algorithm, and mode = 1 is the local search algorithm.
	Author: Rishabh Iyer 
	Email: rkiyer@uw.edu
	Melodi Lab, University of Washington, Seattle
 *
 */
#ifndef __MMINSupSub_INCLUDED__
#define __MMINSupSub_INCLUDED__

#include "../functions/SetFunctions.h"
#include "../functions/SubmodularFunctions.h"
#include "../sets/Set.h"
#include "../sets/HashSet.h"
namespace smtk {
	
template <class Container>
void mminSupSub(SubmodularFunctions& f, SubmodularFunctions& g, Container& currset, double lambda, int verbosity = 1, bool mode = 0, Container startSet = Container());

extern template void mminSupSub(SubmodularFunctions& f, SubmodularFunctions& g, HashSet& currset, double lambda, int verbosity = 1, bool mode = 0, HashSet startSet = HashSet());

}
#endif
