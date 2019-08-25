/*
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

#ifndef CA_L1_LR_PRIMAL
#define CA_L1_LR_PRIMAL

#include "../../../representation/Vector.h"
#include "../../../representation/SparseFeature.h"

namespace datk {
	
void L1LRPrimal(std::vector<SparseFeature>& features, Vector& y, Vector& x, double C, double eps);

}
#endif
