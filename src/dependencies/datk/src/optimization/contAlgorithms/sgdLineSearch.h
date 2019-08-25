/*

Copyright (C) John Halloran

 *	Gradient Descent for Unconstrained Convex Minimization with constant step size
	Solves the problem \min_x \phi(x), where \phi is a convex (or continuous) function.
	Anthor: Rishabh Iyer
	Melodi Lab, University of Washington, Seattle
 *
	Input: 	Continuous Function: c
		   	Initial starting point x0
			step-size parameter (alpha)
			max number of iterations (maxiter)
			Tolerance (TOL)
			Verbosity

	Output: Output on convergence (x)
 */

#ifndef CA_SGD_LINE_SEARCH
#define CA_SGD_LINE_SEARCH

#include "../contFunctions/ContinuousFunctions.h"
#include "../../representation/Vector.h"
#include "../../representation/VectorOperations.h"

namespace datk {
	
Vector sgdLineSearch(const ContinuousFunctions& c, const Vector& x0, const int numSamples,
		     double alpha = 0.1, const int miniBatchSize = 1, 
		     const double TOL = 1e-3, const int maxEval = 1000, 
		     const double gamma = 1e-4, const bool resetAlpha = true, const bool useinputAlpha = false,
		     const int verbosity = 1);
}
#endif
