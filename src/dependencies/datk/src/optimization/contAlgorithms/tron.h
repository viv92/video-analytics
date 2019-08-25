/*

Copyright (C) Rishabh Iyer 2015

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

#ifndef CA_TRON
#define CA_TRON

#include "../contFunctions/ContinuousFunctions.h"
#include "../../representation/Vector.h"
#include "../../representation/VectorOperations.h"
#include "../../representation/Matrix.h"
#include "../../representation/MatrixOperations.h"

namespace datk {
	
Vector tron(const ContinuousFunctions& c, const Vector& x0, const int maxEval = 1000, 
const double TOL = 1e-3, int verbosity = 1);

}
#endif
