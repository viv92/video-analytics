/*

Copyright (C) Rishabh Iyer

 *	Gradient Descent for Unconstrained Convex Minimization with backtracking line search
	Solves the problem \min_x \phi(x), where \phi is a convex (or continuous) function.
	Anthor: Rishabh Iyer
	Melodi Lab, University of Washington, Sea ttle
 *
	Input: 	Continuous Function: c
		   	Initial starting point x0
			back-tracking parameter (gamma)
			max number of function evaluations (maxEvals)
			Tolerance (TOL)
			resetAlpha (whether to reset alpha at every iteration or not)
			verbosity

	Output: Output on convergence (x)
 */

#ifndef CMTK_LBFGS_OWL
#define CMTK_LBFGS_OWL

#include "../contFunctions/ContinuousFunctions.h"
#include "../../representation/Vector.h"
#include "../../representation/VectorOperations.h"
#include "../../representation/Matrix.h"
#include "../../representation/MatrixOperations.h"

namespace datk {
	
Vector lbfgsMinOwl(const ContinuousFunctions& c, const Vector& x0, double alpha = 1, const double gamma = 1e-4, 
const int maxEval = 1000, const int memory = 100, const double TOL = 1e-3, bool resetAlpha = true, bool useinputAlpha = false, int verbosity = 1);

}
#endif
