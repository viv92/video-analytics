/*
	CMTK: Convex Optimization And Machine Learning ToolKit
 *	Abstract base class for Continuous Functions
	Author: Rishabh Iyer
	Melodi Lab, University of Washington, Seattle
 *
 */

#ifndef SUM_CONTINUOUS_FUNCTIONS_H
#define SUM_CONTINUOUS_FUNCTIONS_H

#include "../datarep/Vector.h"
#include "../datarep/Matrix.h"
#include "../datarep/VectorOperations.h"
#include "ContinuousFunctions.h"

namespace cmtk {

	class SumContinuousFunctions: public ContinuousFunctions{
	protected:
		ContinuousFunctions& c1;
		ContinuousFunctions& c2;
		double lambda;
	public:
		SumContinuousFunctions(ContinuousFunctions& c1, ContinuousFunctions& c2, double lambda);
        SumContinuousFunctions(const SumContinuousFunctions& sc); // copy constructor
		
		virtual ~SumContinuousFunctions();
		
		virtual double eval(const Vector& x) const; // functionEval
		virtual Vector evalGradient(const Vector& x) const; // gradientEval
		virtual void eval(const Vector& x, double& f, Vector& gradient) const; // combined function and gradient eval
		virtual Vector evalStochasticGradient(const Vector& x, std::vector<int> batch) const; // stochastic gradient	
		virtual void evalStochastic(const Vector& x, double& f, Vector& g, std::vector<int>& miniBatch) const; // stochastic combined evaluation
	};
	
}
#endif
