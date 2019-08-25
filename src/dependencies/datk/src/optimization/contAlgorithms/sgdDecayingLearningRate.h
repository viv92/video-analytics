/*

Copyright (C) John Halloran

 *	Stochastic gradient descent with a decaying learning rate.  The rate of decay is
        (t) ^ (-decayRate), where t is the number of considered minibatches.
	Solves the problem \min_x \phi(x), where \phi is a convex (or continuous) function.
	Anthor: John Halloran
	Melodi Lab, University of Washington, Seattle
 *
	Input: 	Continuous Function: c
		   	Initial starting point x0
			Number of training/data instances/samples numSamples
			step-size parameter (alpha)
			Number of samples to compute the gradient within an epoch miniBatchSize
			Tolerance (TOL)
			max number of epochs (maxEval)
			power dictating learning rate's decay decayRate
			Verbosity

	Output: Output on convergence (x)
 */

#ifndef CA_SGD_DECAYING_LEARNING_RATE
#define CA_SGD_DECAYING_LEARNING_RATE

#include "../contFunctions/ContinuousFunctions.h"
#include "../../representation/Vector.h"
#include "../../representation/VectorOperations.h"

namespace datk {
	
Vector sgdDecayingLearningRate(const ContinuousFunctions& c, const Vector& x0, const int numSamples,
			       const double alpha = 0.1, const int miniBatchSize = 1, 
			       const double TOL = 1e-3, const int maxEval = 1000, 
			       const double decayRate = 0.5, const int verbosity = 1);
}
#endif
