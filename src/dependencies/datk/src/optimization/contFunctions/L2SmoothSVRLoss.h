/*
  CMTK: Convex Optimization And Machine Learning ToolKit
  *	Smooth SVM Loss with L2 regularization
  Author: Rishabh Iyer
  Melodi Lab, University of Washington, Seattle
  *
  */

#ifndef L2SMOOTHSVR_LOSS_H
#define L2SMOOTHSVR_LOSS_H

#include "../../representation/Vector.h"
#include "../../representation/Matrix.h"
#include "../../representation/VectorOperations.h"
#include "ContinuousFunctions.h"
namespace datk {
  template <class Feature>
  class L2SmoothSVRLoss: public ContinuousFunctions{
  protected:
    std::vector<Feature>& features; // size of features is number of trainins examples (n)
    Vector& y; // size of y is number of training examples (n)
	double p; 
	double lambda;
  public:
    L2SmoothSVRLoss(int numFeatures, std::vector<Feature>& features, Vector& y, double lambda, double p = 0.1);
    L2SmoothSVRLoss(const L2SmoothSVRLoss& c); // copy constructor
		
    ~L2SmoothSVRLoss();
		
    double eval(const Vector& x) const; // functionEval
    Vector evalGradient(const Vector& x) const; // gradientEval
    void eval(const Vector& x, double& f, Vector& gradient) const; // combined function and gradient eval
    Vector evalStochasticGradient(const Vector& x, std::vector<int>& miniBatch) const; // stochastic gradient
    void evalStochastic(const Vector& x, double& f, Vector& g, std::vector<int>& miniBatch) const; // stochastic evaluation
  };
	
}
#endif
