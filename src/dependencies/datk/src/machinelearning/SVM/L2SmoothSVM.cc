/*
	Copyright (C) Rishabh Iyer 2015
 *	L2 Regularized Logistic Regression (with both two class and multi-class classification)
	Author: Rishabh Iyer
	Melodi Lab, University of Washington, Seattle

    algtype: type of algorithm:
		0 (Dual Coordinate Descent from LIBLINEAR )
		1 (LBFGS)
		2 (Gradient Descent with Line Search),
		3 (Gradient Descent with Barzelie Borwein step size),
		4 (Conjugate Gradient),
		5 (Nesterov's optimal method),
		6 (Stochastic Gradient Descent with fixed step length)
		7 (Stochastic Gradient Descent with decaying step size)
		8 (Adaptive Gradient Algorithm (AdaGrad))
 *
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
using namespace std;

#include "L2SmoothSVM.h"
#include "../../optimization/contAlgorithms/contAlgorithms.h"
#include "../../optimization/contFunctions/L2SmoothSVMLoss.h"
#include "../../representation/Set.h"
#include "../../utils/json.hpp"
#include <assert.h>


#define EPSILON 1e-6
namespace datk {
	template <class Feature>
	L2SmoothSVM<Feature>::L2SmoothSVM(vector<Feature>& trainFeatures, Vector& y, int m, int n, int nClasses,
		double lambda, int algtype, int maxIter, double eps, int miniBatch, int lbfgsMemory): Classifiers<Feature>(m, n), trainFeatures(trainFeatures), y(y),
	nClasses(nClasses), lambda(lambda), algtype(algtype), maxIter(maxIter), eps(eps), miniBatch(miniBatch), lbfgsMemory(lbfgsMemory) {}

	template <class Feature>
	L2SmoothSVM<Feature>::L2SmoothSVM(const L2SmoothSVM<Feature>& c) : Classifiers<Feature>(c.m, c.n),
	trainFeatures(c.trainFeatures), y(c.y), nClasses(c.nClasses), lambda(c.lambda), algtype(c.algtype),
	maxIter(c.maxIter), eps(c.eps), miniBatch(c.miniBatch), lbfgsMemory(c.lbfgsMemory) {}

	template <class Feature>
	L2SmoothSVM<Feature>::~L2SmoothSVM(){}

	template <class Feature>
	void L2SmoothSVM<Feature>::train(){ // train L2 regularized logistic regression
		if (nClasses == 2){
			trainOne(y, w);
		}
		else{
			vector<Set> yMapping = vector<Set>(nClasses); // a reverse mapping for indices of a particular label
			for (int i = 0; i < n; i++)
				yMapping[y[i]].insert(i);
			wMany = vector<Vector>(nClasses, Vector(m));
			for (int i = 0; i < nClasses; i++)
			{
				Vector yOne(n, -1);
				for (Set::iterator it = yMapping[i].begin(); it != yMapping[i].end(); it++)
					yOne[*it] = 1;
				trainOne(yOne, wMany[i]);
			}
		}
	}

	template <class Feature>
	void L2SmoothSVM<Feature>::trainOne(Vector& yOne, Vector& wcurr){
	L2SmoothSVMLoss<Feature> ll(m, trainFeatures, yOne, lambda);
	if (algtype == 0){
		//cout<<"*******************************************************************\n";
		//cout<<"Training using Dual Coordinate Descent Algorithm...\n";
		wcurr = SVCDual(trainFeatures, y, 2, 1, 1e-3, 250);
	}
	else if (algtype == 1){
		//cout<<"*******************************************************************\n";
		//cout<<"Training using LBFGS...\n";
		wcurr = lbfgsMin(ll, Vector(m, 0), 1, 1e-4, maxIter, lbfgsMemory, eps);
	}
	else if (algtype == 2){
		cout<<"*******************************************************************\n";
		cout<<"Training using Gradient Descent with fixed step size...\n";
		wcurr = gdLineSearch(ll, Vector(m, 0), 1, 1e-5, maxIter, eps);
	}
	else if (algtype == 3){
		cout<<"*******************************************************************\n";
		cout<<"Training using Gradient Descent with Barzilia-Borwein Step Length\n";
		wcurr = gdBarzilaiBorwein(ll, Vector(m, 0), 1, 1e-5, maxIter, eps);
	}
	// gradientDescentBB(ss, Vector(m, 0), 1, 1e-4, 250);
	else if (algtype == 4){
		cout<<"*******************************************************************\n";
		cout<<"Training using Conjugate Gradient for Logistic Loss, press enter to continue...\n";
		wcurr = cg(ll, Vector(m, 0), 1, 1e-5, maxIter, eps);
	}
	else if (algtype == 5){
		cout<<"*******************************************************************\n";
		cout<<"Training using Nesterov's Method\n";
		wcurr = gdNesterov(ll, Vector(m, 0), 1, 1e-5, maxIter, eps);
	}
	else if (algtype == 6){
		cout<<"*******************************************************************\n";
		cout<<"Training using Stochastic Gradient Descent\n";
		wcurr = sgd(ll, Vector(m, 0), n, 1e-4, miniBatch, eps, maxIter);
	}
	else if (algtype == 7){
		cout<<"*******************************************************************\n";
		cout<<"Training using Stochastic Gradient Descent with decaying learning rate\n";
		wcurr = sgdDecayingLearningRate(ll, Vector(m, 0), n, 0.5*1e-1, miniBatch, eps, maxIter);
	}
	else if (algtype == 8){
		cout<<"*******************************************************************\n";
		cout<<"Training using Adaptive Gradient Algorithm\n";
		wcurr = sgdAdagrad(ll, Vector(m, 0), n, 1e-2, miniBatch, eps, maxIter);
	}
}

	// save the model
	template <class Feature>
	int L2SmoothSVM<Feature>::saveModel(const char* model){
		FILE *fp = fopen(model,"w");
		if(fp==NULL) return -1;

		fprintf(fp, "algtype %s\n", algtype);
		fprintf(fp, "nClasses %d\n", nClasses);
		fprintf(fp, "nFeatures %d\n", m);
		fprintf(fp, "n %d\n", n);
		fprintf(fp, "w\n");
		if (nClasses == 2){
			for(int i=0; i<w.size(); i++)
			{
				fprintf(fp, "%.16g ", w[i]);
			}
			fprintf(fp, "\n");
		}
		else{
			for (int i = 0; i < nClasses; i++){
				for (int j = 0; j < m; j++){
					fprintf(fp, "%.16g ", wMany[i][j]);
				}
			}
			fprintf(fp, "\n");
		}

		if (ferror(fp) != 0 || fclose(fp) != 0) return -1;
		else return 0;
	}

	template <class Feature>
	int L2SmoothSVM<Feature>::saveJsonModel(const char* model){

		nlohmann::json jsonData;
		jsonData["algtype"] = algtype;
		jsonData["nClasses"] = nClasses;
		jsonData["nFeatures"] = m;
		jsonData["n"] = n;
		jsonData["w"] = wMany;

		ofstream dataFile;
		dataFile.open(model);
		dataFile << jsonData.dump(4);
		dataFile.close();
		return 0;
	}

	//brief: Load an already saved model of the classifier
	template <class Feature>
	int L2SmoothSVM<Feature>::loadModel(const char* model){
		FILE *fp = fopen(model,"r");
		if(fp==NULL) return -1;

		char cmd[81];
		while(1)
		{
			fscanf(fp,"%80s",cmd);
			if(strcmp(cmd,"algtype")==0)
				fscanf(fp,"%d",&algtype);
			else if(strcmp(cmd,"nClasses")==0)
				fscanf(fp,"%d",&nClasses);
			else if(strcmp(cmd,"nFeatures")==0)
				fscanf(fp,"%d",&m);
			else if(strcmp(cmd,"n")==0)
				fscanf(fp,"%d",&n);
			else if(strcmp(cmd,"w")==0)
			{
				break;
			}
			else
			{
				fprintf(stderr,"unknown text in model file: [%s]\n",cmd);
				return -1;
			}
		}

		if(nClasses==2){
			w = Vector(m, 0);
			for (int i = 0; i < m; i++)
				fscanf(fp, "%lf ", &w[i]);
		}
		else{
			wMany = vector<Vector>(nClasses, Vector(m));
			for (int i = 0; i < nClasses; i++){
				for (int j = 0; j < m; j++){
					fscanf(fp, "%lf ", &wMany[i][j]);
				}
			}
		}
		if (ferror(fp) != 0 || fclose(fp) != 0) return -1;
		return 1;
	}

	template <class Feature>
	int L2SmoothSVM<Feature>::loadJsonModel(const char* model){

		//cout << "-------------------------------WEIGHTTS-------------------------" << endl << flush;
		nlohmann::json jsonData;
		ifstream dataFile;
    dataFile.open(model);
    dataFile >> jsonData;
		for (nlohmann::json::iterator theClass = jsonData.begin(); theClass != jsonData.end(); theClass++) {
			if ("algtype" == theClass.key()) {
				algtype = theClass.value();
				//cout << "algtype " << algtype << endl << flush;
			} else if ("nClasses" == theClass.key()) {
				nClasses = theClass.value();
				//cout << "nClasses " << nClasses << endl << flush;
			} else if ("nFeatures" == theClass.key()) {
				m = theClass.value();
				//cout << "nFeatures " << m << endl << flush;
			} else if ("n" == theClass.key()) {
				n = theClass.value();
				//cout << "n " << n << endl << flush;
			} else if ("w" == theClass.key()) {
				if (nClasses == 2) {
					w = Vector(m, 0);
					w.push_back(theClass.value());
				} else {
					wMany = vector<Vector>(nClasses, Vector(m));
					vector< vector<double> > tmp;
					int i = 0;
					for (nlohmann::json::iterator storedWeights = theClass.value().begin();
							storedWeights != theClass.value().end();
							storedWeights++) {
							tmp.push_back(*storedWeights);
							wMany[i] = tmp[i];
							i++;
						}
					}
			} else {
				return -1; //invalid data in datk weights json
			}
		}

		//Verifying cout
		/*cout << "w" << endl << flush;
		for (int i = 0; i < nClasses; i++) {
			for (int j = 0; j < m; j++) {
				cout << wMany[i][j] << " ";
			}
		}*/

		return 1;
	}

	template <class Feature>
	double L2SmoothSVM<Feature>::predict(const Feature& testFeature, double& val){
		// the assumption here is that train and test datasets have the same number of features
		if (nClasses == 2){
			val = featureProductCheck(w, testFeature);
			double argval = 0;
			if (val > 0)
				argval = 1;
			else
				argval = -1;
			return argval;
		}
		else{
			val = -1e30;
			double argval = 0;
			for (int j = 0; j < nClasses; j++){
				if (featureProductCheck(wMany[j], testFeature) > val){
					val = wMany[j]*testFeature;
					argval = j;
				}
			}
			return argval;
		}
	}

	template <class Feature>
	double L2SmoothSVM<Feature>::predict(const Feature& testFeature){
		// the assumption here is that train and test datasets have the same number of features
		double val;
		if (nClasses == 2){
			val = featureProductCheck(w, testFeature);
			double argval = 0;
			if (val > 0)
				argval = 1;
			else
				argval = -1;
			return argval;
		}
		else{
			val = -1e30;
			double argval = 0;
			for (int j = 0; j < nClasses; j++){
				if (featureProductCheck(wMany[j], testFeature) > val){
					val = wMany[j]*testFeature;
					argval = j;
				}
			}
			return argval;
		}
	}

	// prob is a vector. The assumption is that prob[0] corresponds to -1 and prob[1] corresponds to +1 in binary
	// classification.
	template <class Feature>
	void L2SmoothSVM<Feature>::predictProbability(const Feature& testFeature, Vector& prob){
		// the assumption here is that train and test datasets have the same number of features
		prob = Vector(nClasses, 0);
		double val;
		if (nClasses == 2){
			val = featureProductCheck(w, testFeature);
			prob[1] = 1/(1+exp(-val));
			prob[0] = 1 - prob[1];
		}
		else{
			double sum = 0;
			for (int j = 0; j < nClasses; j++){
				val = featureProductCheck(wMany[j], testFeature);
				prob[j] = 1/(1 + exp(-val));
				sum += prob[j];
			}
			for (int j = 0; j < nClasses; j++)
				prob[j] = prob[j]/sum;
		}
		return;
	}

  template class L2SmoothSVM<SparseFeature>;
}
