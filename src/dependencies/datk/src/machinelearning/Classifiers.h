/*
	Copyright (C) Rishabh Iyer 2015
	CMTK: Convex Optimization And Machine Learning ToolKit
 *	Abstract base class for Classifiers
	Author: Rishabh Iyer
	Melodi Lab, University of Washington, Seattle
 *
 */

#ifndef CLASSIFIER_H
#define CLASSIFIER_H

#include "../representation/Vector.h"
#include "../representation/Matrix.h"
#include "../representation/VectorOperations.h"
#include "../representation/MatrixOperations.h"
#include "../representation/DenseFeature.h"
#include "../representation/SparseFeature.h"

#include <vector>
using namespace std;

namespace datk {
  template <class Feature>
	class Classifiers{
	protected:
		int m;
		int n;
	public:
		Classifiers();
		Classifiers(int m, int n);
        Classifiers(const Classifiers& c); // copy constructor
		virtual ~Classifiers();

		virtual void train() = 0; // train

		virtual int saveModel(const char* model) = 0; // save the model
		virtual int loadModel(const char* model) = 0; // load the model
    virtual int saveJsonModel(const char* model) = 0; // save the model
    virtual int loadJsonModel(const char* model) = 0; // load the model

		virtual double predict(const Feature& testFeature) = 0;
		virtual double predict(const Feature& testFeature, double& val) = 0;
		virtual void predictProbability(const Feature& testFeature, Vector& prob) = 0;

		double operator()(const Feature& testFeature);

		int size(); // number of features or dimension size (m)
		int length(); // number of training examples
	};

}
#endif
