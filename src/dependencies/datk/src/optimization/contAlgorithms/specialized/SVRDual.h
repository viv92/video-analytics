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

#ifndef CA_SVR_Dual
#define CA_SVR_Dual

#include "../../../representation/Vector.h"
#include "../../../representation/VectorOperations.h"
#include "../../../representation/SparseFeature.h"


namespace datk {
	
	Vector SVRDual(std::vector<SparseFeature>& features, Vector& y, int solver_type, double lambda, double p, double eps, int max_iter);

}
#endif
