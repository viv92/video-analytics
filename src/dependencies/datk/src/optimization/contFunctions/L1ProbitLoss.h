/*
	Copyright (C) Rishabh Iyer 2015
	Data Analytics ToolKit 
 *	Probit Loss with L2 regularization
	Author: Rishabh Iyer
	Melodi Lab, University of Washington, Seattle
 *
 */

#ifndef L1_PROBIT_H
#define L1_PROBIT_H

#include "../../representation/Vector.h"
#include "../../representation/Matrix.h"
#include "../../representation/VectorOperations.h"
#include "ContinuousFunctions.h"
#include "../../utils/utils.h"

namespace datk {

  template <class Feature>
	class L1ProbitLoss: public ContinuousFunctions{
	protected:
		std::vector<Feature>& features; // size of features is number of trainins examples (n)
		Vector& y; // size of y is number of training examples (n)
		double lambda;
	public:
		L1ProbitLoss(int numFeatures, std::vector<Feature>& features, Vector& y, double lambda);
        L1ProbitLoss(const L1ProbitLoss& c); // copy constructor
		
		~L1ProbitLoss();
		
		double eval(const Vector& x) const; // functionEval
		Vector evalGradient(const Vector& x) const; // gradientEval
		void eval(const Vector& x, double& f, Vector& gradient) const; // combined function and gradient eval
		Vector evalStochasticGradient(const Vector& x, std::vector<int>& miniBatch) const; // stochastic gradient
		void evalStochastic(const Vector& x, double& f, Vector& g, std::vector<int>& miniBatch) const; // stochastic evaluation
	};
	
}
#endif
