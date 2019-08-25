/*
	CMTK: Convex Optimization And Machine Learning ToolKit
 *	Smooth SVM Loss with L2 regularization
	Author: Rishabh Iyer
	Melodi Lab, University of Washington, Seattle
 *
*/

#include<iostream>
#include <math.h>
using namespace std;

#include "L2SmoothSVMLoss.h"
#include "../../representation/VectorOperations.h"
#include <assert.h>
#define EPSILON 1e-6
#define MAX 1e2
namespace datk {	
  template <class Feature>
	L2SmoothSVMLoss<Feature>::L2SmoothSVMLoss(int m, std::vector<Feature>& features, Vector& y, double lambda): 
	ContinuousFunctions(true, m, features.size()), features(features), y(y), lambda(lambda)
	{
		if (n > 0)
			assert(features[0].numFeatures == m);
		assert(features.size() == y.size());
	}
	
  template <class Feature>
	L2SmoothSVMLoss<Feature>::L2SmoothSVMLoss(const L2SmoothSVMLoss& l) : 
	ContinuousFunctions(true, l.m, l.n), features(l.features), y(l.y), lambda(l.lambda) {}

  template <class Feature>
    L2SmoothSVMLoss<Feature>::~L2SmoothSVMLoss(){}
	
  template <class Feature>
	double L2SmoothSVMLoss<Feature>::eval(const Vector& x) const{
		assert(x.size() == m);
		double sum = 0.5*lambda*(x*x);
		for (int i = 0; i < n; i++){
			double preval = y[i]*(x*features[i]);
			if (1 - preval>= 0){
				sum += (1 - preval)*(1 - preval);
			}
		}
		return sum;
	}
	
  template <class Feature>
	Vector L2SmoothSVMLoss<Feature>::evalGradient(const Vector& x) const{
		assert(x.size() == m);
		Vector g = lambda*x;
		for (int i = 0; i < n; i++){
			double preval = y[i]*(x*features[i]);
			if (1 - preval>= 0){
				g -= 2*features[i]*(1 - preval)*y[i];
			}
		}
		return g;
	}

  template <class Feature>
	void L2SmoothSVMLoss<Feature>::eval(const Vector& x, double& f, Vector& g) const{
		assert(x.size() == m);
		g = lambda*x;
		f = 0.5*lambda*(x*x);
		for (int i = 0; i < n; i++){
			double preval = y[i]*(x*features[i]);
			if (1 - preval>= 0){
				f += (1 - preval)*(1 - preval);
				g -= 2*features[i]*(1 - preval)*y[i];
			}
		}
		return;
	}
	
  template <class Feature>
	Vector L2SmoothSVMLoss<Feature>::evalStochasticGradient(const Vector& x, std::vector<int>& miniBatch) const{
		assert(x.size() == m);
		Vector g = lambda*x;
		for (vector<int>::iterator it = miniBatch.begin(); it != miniBatch.end(); it++){
			double preval = y[*it]*(x*features[*it]);
			if (1 - preval>= 0){
				g -= 2*features[*it]*(1 - preval)*y[*it];
			}
		}
		return g;
	}

  template <class Feature>
	void L2SmoothSVMLoss<Feature>::evalStochastic(const Vector& x, double& f, Vector& g, std::vector<int>& miniBatch) const{
		assert(x.size() == m);
		g = lambda*x;
		f = 0.5*lambda*(x*x);
		double val;
		for (vector<int>::iterator it = miniBatch.begin(); it != miniBatch.end(); it++){
			double preval = y[*it]*(x*features[*it]);
			if (1 - preval>= 0){
				f += (1 - preval)*(1 - preval);
				g -= 2*features[*it]*(1 - preval)*y[*it];
			}
		}
		return;
	}
		
  template class L2SmoothSVMLoss<SparseFeature>;
  template class L2SmoothSVMLoss<DenseFeature>;
	
		
}
