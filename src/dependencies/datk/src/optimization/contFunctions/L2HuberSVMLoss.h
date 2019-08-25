/*
	Copyright (C) Rishabh Iyer 2015
	Data Analytics ToolKit 
 *	Smooth SVM Loss with L2 regularization
	Author: Rishabh Iyer
	Melodi Lab, University of Washington, Seattle
 *
 */

#ifndef L2_HUBER_SVM_LOSS_H
#define L2_HUBER_SVM_LOSS_H

#include "../../representation/Vector.h"
#include "../../representation/Matrix.h"
#include "../../representation/VectorOperations.h"
#include "ContinuousFunctions.h"
namespace datk {

  template <class Feature>
	class L2HuberSVMLoss: public ContinuousFunctions{
	protected:
		std::vector<Feature>& features; // size of features is number of trainins examples (n)
		Vector& y; // size of y is number of training examples (n)
		double thresh;
		double lambda; // regularization coefficient for L2 regularization
	public:
		L2HuberSVMLoss(int numFeatures, std::vector<Feature>& features, Vector& y, double thresh, double lambda);
        L2HuberSVMLoss(const L2HuberSVMLoss& c); // copy constructor
		
		~L2HuberSVMLoss();
		
		double eval(const Vector& x) const; // functionEval
		Vector evalGradient(const Vector& x) const; // gradientEval
		void eval(const Vector& x, double& f, Vector& gradient) const; // combined function and gradient eval
		Vector evalStochasticGradient(const Vector& x, std::vector<int>& miniBatch) const; // stochastic gradient
		void evalStochastic(const Vector& x, double& f, Vector& g, std::vector<int>& miniBatch) const; // stochastic evaluation
	};
	
}
#endif
