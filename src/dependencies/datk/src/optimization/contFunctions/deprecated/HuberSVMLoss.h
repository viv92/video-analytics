/*
	CMTK: Convex Optimization And Machine Learning ToolKit
 *	Smooth SVM Loss with L2 regularization
	Author: Rishabh Iyer
	Melodi Lab, University of Washington, Seattle
 *
 */

#ifndef HUBER_SVM_LOSS_H
#define HUBER_SVM_LOSS_H

#include "../datarep/Vector.h"
#include "../datarep/Matrix.h"
#include "../datarep/VectorOperations.h"
#include "ContinuousFunctions.h"
namespace cmtk {

  template <class Feature>
	class HuberSVMLoss: public ContinuousFunctions{
	protected:
		std::vector<Feature>& features; // size of features is number of trainins examples (n)
		Vector& y; // size of y is number of training examples (n)
		double thresh;
	public:
		HuberSVMLoss(int numFeatures, std::vector<Feature>& features, Vector& y, double thresh);
        HuberSVMLoss(const HuberSVMLoss& c); // copy constructor
		
		~HuberSVMLoss();
		
		double eval(const Vector& x) const; // functionEval
		Vector evalGradient(const Vector& x) const; // gradientEval
		void eval(const Vector& x, double& f, Vector& gradient) const; // combined function and gradient eval
		Vector evalStochasticGradient(const Vector& x, std::vector<int>& miniBatch) const; // stochastic gradient
		void evalStochastic(const Vector& x, double& f, Vector& g, std::vector<int>& miniBatch) const; // stochastic evaluation
	};
	
}
#endif
