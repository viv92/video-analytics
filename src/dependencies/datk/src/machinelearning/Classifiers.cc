/*
	Copyright (C) Rishabh Iyer 2015
 *	Abstract base class for Classifiers
	Author: Rishabh Iyer
	Melodi Lab, University of Washington, Seattle
 *
*/

#include<iostream>
using namespace std;

#include "Classifiers.h"
#define EPSILON 1e-6
namespace datk {
  template <class Feature>
	Classifiers<Feature>::Classifiers(){}
  template <class Feature>
	Classifiers<Feature>::Classifiers(int m, int n): m(m), n(n){}
  template <class Feature>
	Classifiers<Feature>::Classifiers(const Classifiers& c): m(c.m), n(c.n){}

  template <class Feature>
    Classifiers<Feature>::~Classifiers(){}

  template <class Feature>
	void Classifiers<Feature>::train(){} // train

  template <class Feature>
	int Classifiers<Feature>::saveModel(const char* model){}
  template <class Feature>
	int Classifiers<Feature>::loadModel(const char* model){}
  template <class Feature>
  int Classifiers<Feature>::saveJsonModel(const char* model){}
  template <class Feature>
  int Classifiers<Feature>::loadJsonModel(const char* model){}

  template <class Feature>
	double Classifiers<Feature>::predict(const Feature& testFeature){}
  template <class Feature>
	double Classifiers<Feature>::predict(const Feature& testFeature, double& val){}
  template <class Feature>
	void Classifiers<Feature>::predictProbability(const Feature& testFeature, Vector& prob){}

  template <class Feature>
	double Classifiers<Feature>::operator()(const Feature& testFeature)
	{
		return predict(testFeature);
	}

  template <class Feature>
	int Classifiers<Feature>::size(){ // number of features or dimension size
		return m;
	}

  template <class Feature>
	int Classifiers<Feature>::length(){ // number of convex functions adding up
		return n;
	}

  template class Classifiers<SparseFeature>;
  template class Classifiers<DenseFeature>;
}
