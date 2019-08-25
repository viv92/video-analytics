/*
	Copyright (C) Rishabh Iyer 2015
	Data Analytics ToolKit
 *	Implementation of the feature based functions (essentially sums of concave over modular functions): f(X) = \sum_{f \in F} w_f g(m_f(X)), where g is a concave function, {m_f}_{f \in F} are a set of feature scores, and f \in F are features.
	Author: Rishabh Iyer.
 *
 */
#include <iostream>
#include <cmath>
using namespace std;

#include "FeatureBasedFunctionsDense.h"
namespace datk {
	
	FeatureBasedFunctionsDense::FeatureBasedFunctionsDense(int n, const int type, vector<struct DenseFeature>& feats, vector<double>& featureWeights, const double thresh): 
	SubmodularFunctions(n), type(type), thresh(thresh), feats(feats), featureWeights(featureWeights){
		nFeatures = featureWeights.size();
		sizepreCompute = nFeatures;
		preCompute.resize(sizepreCompute); 
	}
    
    
    FeatureBasedFunctionsDense::FeatureBasedFunctionsDense(const FeatureBasedFunctionsDense & f) : 
	SubmodularFunctions(f), feats(f.feats), featureWeights(f.featureWeights), thresh(f.thresh), nFeatures(f.nFeatures), type(f.type){
        preCompute = f.preCompute;
        preComputeSet = f.preComputeSet;
        sizepreCompute = f.sizepreCompute;
        //cout << "Called copy constructor " << endl; 
    }

	FeatureBasedFunctionsDense::~FeatureBasedFunctionsDense(){
        //cout << "Call destructor of feature based function!" << endl;
        preCompute.clear();
        preComputeSet.clear();
        sizepreCompute = 0;
    }



    FeatureBasedFunctionsDense* FeatureBasedFunctionsDense::FeatureBasedFunctionsDense::clone() const{
        //cout << "called feature based function clone" << endl;
        return new FeatureBasedFunctionsDense(*this);
    }



	double FeatureBasedFunctionsDense::concaveFunction(double K) const{
		switch(type)
		{
			case 1 :
				return sqrt(K);
			case 2 :
				return (1-1/(K+1));
			case 3 :
				return log(1 + K);
			case 4 : 
				return std::min(K, thresh);
			default :
				return sqrt(K);
		}
	}

	double FeatureBasedFunctionsDense::eval(const Set& set) const{
	// Evaluation of function valuation.
		double sum = 0;
		vector<double> featurescoresset (nFeatures, 0);
		Set::const_iterator it;
		for( it = set.begin(); it != set.end(); ++it ){
			for (int j = 0; j < nFeatures; j++){
				featurescoresset[j] += feats[*it].featureVec[j];   
			}
		}
		for (int ii=0; ii<nFeatures; ii++){
			sum = sum + featureWeights[ii]*concaveFunction(featurescoresset[ii]);
		}
		return sum;
	}

	double FeatureBasedFunctionsDense::evalFast(const Set& set) const{
	// Evaluation of function valuation.		
		double sum = 0;
		for (int ii=0; ii<nFeatures; ii++){
			sum = sum + featureWeights[ii]*concaveFunction(preCompute[ii]);
		}
		return sum;
	}

	double FeatureBasedFunctionsDense::evalGainsadd(const Set& set, int item) const{
	// Evaluation of gains.
		if(set.contains(item)){
				cout<<"Error in using evalGainsadd: the provided item already belongs to the subset\n";
				return 0;
		}
		double gains = 0; 
		double temp;
		double diff;
		vector<double> featurescoresset (nFeatures, 0);
		Set::const_iterator it;
		for( it = set.begin(); it != set.end(); ++it){
			for (int j = 0; j < nFeatures; j++){
				featurescoresset[j] += feats[*it].featureVec[j];   
			}
		}
		for (int i=0; i<nFeatures; i++){
			temp = featurescoresset[i];
			diff = concaveFunction(temp + feats[item].featureVec[i]) - concaveFunction(temp);
			gains += featureWeights[i] * diff;
		}
		return gains;
	}

	double FeatureBasedFunctionsDense::evalGainsremove(const Set& sset, int item) const{
	// Evaluation of function valuation.
		if(!sset.contains(item)){
			cout<<"Error in using evalGainsremove: the provided item does not belong to the subset\n";
			return 0;
		}
		double sum = 0;
		double sumd = 0;
		vector<double> featurescoresset (nFeatures, 0);
		vector<double> featurescoressetd (nFeatures, 0);
		Set::const_iterator it;
		for( it = sset.begin(); it != sset.end(); ++it){
			for (int j = 0; j < nFeatures; j++){
				    featurescoresset[j] += feats[*it].featureVec[j];  
				if(*it != item){
		            featurescoressetd[j] += feats[*it].featureVec[j];
		    }
			}
		}
		for (int ii=0; ii<nFeatures; ii++){
			sum = sum + featureWeights[ii]*concaveFunction(featurescoresset[ii]);
			sumd = sumd + featureWeights[ii]*concaveFunction(featurescoressetd[ii]);	
		}
		return (sum - sumd);
	}

	double FeatureBasedFunctionsDense::evalGainsaddFast(const Set& sset, int item) const{
	// Fast evaluation of Adding gains using the precomputed statistics. This is used, for example, in the implementation of the forward greedy algorithm.
		double gains = 0; 
		double temp;
		double diff;
		gains = 0;;
		for (int i=0; i<nFeatures; i++){
			temp = preCompute[i];
			diff = concaveFunction(temp + feats[item].featureVec[i]) - concaveFunction(temp);
			gains += featureWeights[i] * diff;
			}
			return gains;	
		}

	double FeatureBasedFunctionsDense::evalGainsremoveFast(const Set& sset, int item) const{
	// Fast evaluation of Removing gains using the precomputed statistics. This is used, for example, in the implementation of the reverse greedy algorithm.
		double gains = 0; 
		double temp;
		double diff;
		gains = 0;;
		for (int i=0; i<nFeatures; i++){
			temp = preCompute[i] - feats[item].featureVec[i];
			diff = concaveFunction(preCompute[i]) - concaveFunction(temp);
			gains += featureWeights[i] * diff;
		}
		return gains;
	}

	void FeatureBasedFunctionsDense::updateStatisticsAdd(const Set& sset, int item) const{
	// Update statistics for algorithms sequentially adding elements (for example, the greedy algorithm).
		for (int i = 0; i < nFeatures; i++){
			preCompute[i] += feats[item].featureVec[i];
		}
		preComputeSet.insert(item);
	}

	void FeatureBasedFunctionsDense::updateStatisticsRemove(const Set& sset, int item) const{
	// Update statistics for algorithms sequentially removing elements (for example, the reverse greedy algorithm).
		for (int i = 0; i < nFeatures; i++){
		        preCompute[i] -= feats[item].featureVec[i];
		}
		preComputeSet.remove(item);
	}

	void FeatureBasedFunctionsDense::clearpreCompute() const{
		for (int i = 0; i < sizepreCompute; i++) { preCompute[i] = 0; }
		preComputeSet.clear();
		}

	void FeatureBasedFunctionsDense::setpreCompute(const Set& sset) const{
		clearpreCompute();
		Set::const_iterator it;
		for( it = sset.begin(); it != sset.end(); ++it ){
			updateStatisticsAdd(sset, *it);
		}
		preComputeSet = sset;
	}
}

