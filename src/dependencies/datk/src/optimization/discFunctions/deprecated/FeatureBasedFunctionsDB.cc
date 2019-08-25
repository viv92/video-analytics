/*
 *	Implementation of the feature based functions (essentially sums of concave over modular functions): f(X) = \sum_{f \in F} w_f g(m_f(X)), where g is a concave function, {m_f}_{f \in F} are a set of feature scores, and f \in F are features.
	Author: Rishabh Iyer.
	Melodi Lab, University of Washington, Seattle
TODO: 
(1) Add control of number of threads;
(2) add control of whether using parallel computation. 
 *
 */
#include <cmath>
#include <iostream>
#ifdef _OPENMP_
#include<omp.h> 
#endif
#include "FeatureBasedFunctionsDB.h"
using namespace std;

namespace smtk {


	FeatureBasedFunctionsDB::FeatureBasedFunctionsDB(int n, int type, SparseRepresentation& feats, vector<double>& featureWeights, double thresh):
	 SubmodularFunctions(n), type(type), thresh(thresh), feats(feats), featureWeights(featureWeights), nFeatures(featureWeights.size()), sizepreCompute(nFeatures) {
        for (std::vector<double>::const_iterator it = featureWeights.begin(); it != featureWeights.end(); it++){
            if (*it<0){
                cout << "********************************************************************" << endl;
                cout << "Warning: the input feature weights has some entry being negative, please double check" << endl;
                cout << "********************************************************************" << endl;
            }
        }
		preCompute.resize(sizepreCompute); 
        cout << "Called standard constructor " << endl; 
	}
    
    
    FeatureBasedFunctionsDB::FeatureBasedFunctionsDB(const FeatureBasedFunctionsDB & f) : 
	SubmodularFunctions(f), feats(f.feats), featureWeights(f.featureWeights), type(f.type), thresh(f.thresh), nFeatures(f.nFeatures), sizepreCompute(f.sizepreCompute) {
        preCompute = f.preCompute;
        preComputeSet = f.preComputeSet;
        cout << "Called copy constructor " << endl; 
    }

	FeatureBasedFunctionsDB::~FeatureBasedFunctionsDB(){
        //cout << "Call destructor of feature based function!" << endl;
    }



    FeatureBasedFunctionsDB* FeatureBasedFunctionsDB::FeatureBasedFunctionsDB::clone() const{
        //cout << "called feature based function clone" << endl;
        return new FeatureBasedFunctionsDB(*this);
    }

    void FeatureBasedFunctionsDB::resetData(const std::vector<int> Rset){
        // 
        feats.resetRepresentationSet(Rset);
    }


	double FeatureBasedFunctionsDB::concaveFunction(const double& K) const{
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

	double FeatureBasedFunctionsDB::eval(const Set& sset) const{
	// Evaluation of function valuation.
		double sum = 0;
		vector<double> featurescoresset (nFeatures, 0);
		Set::const_iterator it(sset);
		for( it = sset.begin(); it != sset.end(); ++it ){
			for (int j = 0; j < feats[*it].num_uniq_wrds; j++){
				featurescoresset[feats[*it].featureIndex[j]] += feats[*it].featureVec[j];   
			}
		}
		for (int ii=0; ii<nFeatures; ii++){
			sum = sum + featureWeights[ii]*concaveFunction(featurescoresset[ii]);
		}
		return sum;
	}

	double FeatureBasedFunctionsDB::evalFast(const Set& sset) const{
	// Evaluation of function valuation.		
		double sum = 0;
		for (int ii=0; ii<nFeatures; ii++){
		sum = sum + featureWeights[ii]*concaveFunction(preCompute[ii]);
		}
		return sum;
	}

	double FeatureBasedFunctionsDB::evalGainsadd(const Set& sset, int item) const{
	// Evaluation of gains.
		if(sset.contains(item)){
				cout<<"Error in using evalGainsadd: the provided item already belongs to the subset\n";
				return 0;
		}
		double gains = 0; 
		double temp;
		double diff;
		vector<double> featurescoresset (nFeatures, 0);
		Set::const_iterator it(sset);
		for( it = sset.begin(); it != sset.end(); ++it ){
			for (int j = 0; j < feats[*it].num_uniq_wrds; j++){
				featurescoresset[feats[*it].featureIndex[j]] += feats[*it].featureVec[j];   
			}
		}
		for (int i=0; i<feats[item].num_uniq_wrds; i++){
			temp = featurescoresset[feats[item].featureIndex[i]];
			diff = concaveFunction(temp + feats[item].featureVec[i]) - concaveFunction(temp);
			gains += featureWeights[feats[item].featureIndex[i]] * diff;
		}
		return gains;
	}

	double FeatureBasedFunctionsDB::evalGainsremove(const Set& sset, int item) const{
	// Evaluation of function valuation.
		if(!sset.contains(item)){
			cout<<"Error in using evalGainsremove: the provided item does not belong to the subset\n";
			return 0;
		}
		double sum = 0;
		double sumd = 0;
		vector<double> featurescoresset (nFeatures, 0);
		vector<double> featurescoressetd (nFeatures, 0);
		Set::const_iterator it(sset);
		for( it = sset.begin(); it != sset.end(); ++it ){
			for (int j = 0; j < feats[*it].num_uniq_wrds; j++){
				    featurescoresset[feats[*it].featureIndex[j]] += feats[*it].featureVec[j];  
				if(*it != item){
		            featurescoressetd[feats[*it].featureIndex[j]] += feats[*it].featureVec[j];
		    }
			}
		}
		for (int ii=0; ii<nFeatures; ii++){
			sum = sum + featureWeights[ii]*concaveFunction(featurescoresset[ii]);
			sumd = sumd + featureWeights[ii]*concaveFunction(featurescoressetd[ii]);	
		}
		return (sum - sumd);
	}

	double FeatureBasedFunctionsDB::evalGainsaddFast(const Set& sset, int item) const{
	// Fast evaluation of Adding gains using the precomputed statistics. This is used, for example, in the implementation of the forward greedy algorithm.
		double gains = 0; 
		double temp;
		double diff;
        SparseFeature currfeat = feats[item];
		int num_wrds = currfeat.num_uniq_wrds;
		gains = 0.0;
		//#pragma omp parallel for reduction(+:gains) // cannot include the parallel computing, since it causes error. 
		for (int i=0; i<num_wrds; i++){
			temp = preCompute[currfeat.featureIndex[i]];
			diff = concaveFunction(temp + currfeat.featureVec[i]) - concaveFunction(temp);
			gains += featureWeights[currfeat.featureIndex[i]] * diff;
			}
			return gains;	
		}

	double FeatureBasedFunctionsDB::evalGainsremoveFast(const Set& sset, int item) const{
	// Fast evaluation of Removing gains using the precomputed statistics. This is used, for example, in the implementation of the reverse greedy algorithm.
		double gains = 0; 
		double temp;
		double diff;
		int num_wrds = feats[item].num_uniq_wrds;
		gains = 0.0;
		//#pragma omp parallel for reduction(+:gains) // cannot include the parallel computing, since it causes error.
		for (int i=0; i<num_wrds; i++){
			temp = preCompute[feats[item].featureIndex[i]] - feats[item].featureVec[i];
			diff = concaveFunction(preCompute[feats[item].featureIndex[i]]) - concaveFunction(temp);
			gains += featureWeights[feats[item].featureIndex[i]] * diff;
		}
		return gains;
	}

	void FeatureBasedFunctionsDB::updateStatisticsAdd(const Set& sset, int item) const{
	// Update statistics for algorithms sequentially adding elements (for example, the greedy algorithm).
		//#pragma omp parallel for // cannot include the parallel computing, since it causes error.
		for (int i = 0; i < feats[item].num_uniq_wrds; i++){
			preCompute[feats[item].featureIndex[i]] += feats[item].featureVec[i];
		}
		preComputeSet.insert(item);
	}

	void FeatureBasedFunctionsDB::updateStatisticsRemove(const Set& sset, int item) const{
	// Update statistics for algorithms sequentially removing elements (for example, the reverse greedy algorithm).
		//#pragma omp parallel for // cannot include the parallel computing, since it causes error.
		for (int i = 0; i < feats[item].num_uniq_wrds; i++){
		        preCompute[feats[item].featureIndex[i]] -= feats[item].featureVec[i];
		}
		preComputeSet.remove(item);
	}

	void FeatureBasedFunctionsDB::clearpreCompute() const{
		for (int i = 0; i < sizepreCompute; i++) { preCompute[i] = 0; }
		preComputeSet.clear();
		}

	void FeatureBasedFunctionsDB::setpreCompute(const Set& sset) const{
		clearpreCompute();
		Set::const_iterator it(sset);
		for( it = sset.begin(); it != sset.end(); ++it ){
			updateStatisticsAdd(sset, *it);
		}
		preComputeSet = sset;
	}
}

