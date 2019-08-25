/*
 *	Implementation of the feature based over feature based functions (essentially sums of concave over modular functions): f(X) = \sum_{f \in F} w_f \sum_{g \in G} w_g h(m_fg(X)), where g is a concave function, {m_fg}_{f \in F ang g \in G } are a set of feature scores, and f \in F and g \in G are 2 sets of features.
	Author: Rishabh Iyer.
	Melodi Lab, University of Washington, Seattle
 *
 */
#include <iostream>
#include <cmath>
using namespace std;

#include "FeatureBasedFunctionsTwoLayerDense.h"
namespace smtk {
	
	FeatureBasedFunctionsTwoLayerDense::FeatureBasedFunctionsTwoLayerDense(int n, const vector<int> type, vector< vector<struct DenseFeature> >& feats, vector< vector<double> >& featureWeights, const double thresh):
	SubmodularFunctions(n), type(type), thresh(thresh), feats(feats), featureWeights(featureWeights){
		
        //sizepreCompute = 0;
        preCompute.resize(featureWeights.size(),vector<double>(0));
        
        
        for (int featCnt = 0; featCnt < featureWeights.size(); featCnt++) {
            
            int currSz = featureWeights[featCnt].size() - 1;
            nFeatures.push_back(currSz);
            sizepreCompute.push_back(currSz);
            
            //cout<<"Before preCompute"<<endl;
            //cout<<"preCompute.size()"<<preCompute.size()<<endl;
            preCompute[featCnt].resize(currSz);
            //cout<<"After preCompute"<<endl;
            
        }
        
		//preCompute.resize(sizepreCompute);
	}
    
    
    FeatureBasedFunctionsTwoLayerDense::FeatureBasedFunctionsTwoLayerDense(const FeatureBasedFunctionsTwoLayerDense & f) :
	SubmodularFunctions(f), feats(f.feats), featureWeights(f.featureWeights), thresh(f.thresh), nFeatures(f.nFeatures), type(f.type){
        preCompute = f.preCompute;
        preComputeSet = f.preComputeSet;
        sizepreCompute = f.sizepreCompute;
        //cout << "Called copy constructor " << endl; 
    }

	FeatureBasedFunctionsTwoLayerDense::~FeatureBasedFunctionsTwoLayerDense(){
        //cout << "Call destructor of feature based function!" << endl;
        preCompute.clear();
        preComputeSet.clear();
        sizepreCompute.clear();// = 0;
    }



    FeatureBasedFunctionsTwoLayerDense* FeatureBasedFunctionsTwoLayerDense::FeatureBasedFunctionsTwoLayerDense::clone() const{
        //cout << "called feature based function clone" << endl;
        return new FeatureBasedFunctionsTwoLayerDense(*this);
    }



	double FeatureBasedFunctionsTwoLayerDense::concaveFunction(double K, int concaveType) const{
		switch(concaveType)
		{
			case 1 :
				return sqrt(K);
			case 2 :
				return (1-1/(K+1));
			case 3 :
				return log(1 + K);
			case 4 : 
				return std::min(K, thresh[0]); // update this to use appropriate  thresh
            case 5 :
                return pow(K,1);
			default :
				return sqrt(K);
		}
	}

	double FeatureBasedFunctionsTwoLayerDense::eval(const Set& set) const{
	// Evaluation of function valuation.
		double inner_sum = 0.0;
        double sum = 0.0;
        const int numOuterFeatures = featureWeights.size();
        
		vector< vector<double> >featurescoresset (numOuterFeatures, vector<double>(0));
		Set::const_iterator it(set);
        
        // featureWeights is a N X M dim matrix where N is #outer features and M = # inner features
        // Note featureWeights.size() == nFeatures.size();
        
        //cout<<"nFeatures.size() = "<<nFeatures.size()<<endl;
        //cout<<"nFeatures[1] = "<<nFeatures[0]<<endl;
        
        
        for (int j = 0; j < nFeatures.size(); j++){
            for (int k = 0; k < nFeatures[j]; k++) {
                int count = 0;
                for( it = set.begin(); it != set.end(); ++it ) {
                    if (count == 0) {
                        // First time memory needs to be allocated. Hence, push_back
                        featurescoresset[j].push_back(feats[*it][j].featureVec[k]);
                        count++;
                    }
                    else {
                        featurescoresset[j][k] += feats[*it][j].featureVec[k];
                    }
                }
                //featurescoresset[j] += feats[*it].featureVec[j];
                //cout<<"featurescoresset["<<j<<"]["<<k<<"] = "<<featurescoresset[j][k]<<endl;
            }
        }
        
        //cout<<featurescoresset<<endl;
        
		for (int ii = 0; ii < nFeatures.size(); ii++){
            inner_sum = 0.0;
            for (int jj = 0; jj < nFeatures[jj]; jj++) {
                inner_sum = inner_sum + featureWeights[ii][jj + 1]*concaveFunction(featurescoresset[ii][jj],type[1]);
            }
            sum += featureWeights[ii][0]*concaveFunction(inner_sum,type[0]);
		}
		return sum;
	}

	double FeatureBasedFunctionsTwoLayerDense::evalFast(const Set& set) const{
	// Evaluation of function valuation.		
		double sum = 0;
        
        for (int i = 0; i < nFeatures.size(); i++){
            double inner_sum_curr = 0.0;
            for (int j = 0; j < nFeatures[i]; j++) {
                inner_sum_curr += concaveFunction(preCompute[i][j],type[1]);
                //cout<<"preCompute["<<i<<"]["<<j<<"] = "<<preCompute[i][j]<<endl;
                
            }
            sum += featureWeights[i][0]*concaveFunction(inner_sum_curr,type[0]);
        }
        
		return sum;
	}

	double FeatureBasedFunctionsTwoLayerDense::evalGainsadd(const Set& set, int item) const{
	// Evaluation of gains. At present Highly Inefficient (both computationally and from memory usage). Look into improving implementation
		if(set.contains(item)){
				cout<<"Error in using evalGainsadd: the provided item already belongs to the subset\n";
				return 0;
		}
		double gains = 0;
        double summCurr = 0.0;
        double summUpdt = 0.0;
        double inner_sum = 0.0;
        double inner_sum_add = 0.0;
		double temp;
		double diff;
		//vector<double> featurescoresset (nFeatures, 0);
        vector< vector<double> > featurescoresset (featureWeights.size(), vector <double>(0));
        vector< vector<double> > featurescoressetUpdt;
		Set::const_iterator it(set);

        
        for (int j = 0; j < nFeatures.size(); j++){
            for (int k = 0; k < nFeatures[j]; k++) {
                int count = 0;
                for( it = set.begin(); it != set.end(); ++it ) {
                    if (count == 0) {
                        // First time memory needs to be allocated. Hence, push_back
                        featurescoresset[j].push_back(feats[*it][j].featureVec[k]);
                        count++;
                    }
                    else {
                        featurescoresset[j][k] += feats[*it][j].featureVec[k];
                    }
                }
                //featurescoresset[j] += feats[*it].featureVec[j];
                //cout<<"featurescoresset["<<j<<"]["<<k<<"] = "<<featurescoresset[j][k]<<endl;
            }
        }
        
        featurescoressetUpdt = featurescoresset;
        
        for (int j = 0; j < nFeatures.size(); j++){
            for (int k = 0; k < nFeatures[j]; k++) {
                featurescoressetUpdt[j][k] += feats[item][j].featureVec[k];
                //cout<<"featurescoressetUpdt["<<j<<"]["<<k<<"] = "<<featurescoressetUpdt[j][k]<<endl;
                //cout<<"featurescoresset["<<j<<"]["<<k<<"] = "<<featurescoresset[j][k]<<endl;
            }
        }
        
        for (int ii = 0; ii < nFeatures.size(); ii++){
            inner_sum = 0.0;
            inner_sum_add = 0.0;
            for (int jj = 0; jj < nFeatures[jj]; jj++) {
                inner_sum += featureWeights[ii][jj + 1]*concaveFunction(featurescoresset[ii][jj],type[1]);
                inner_sum_add += featureWeights[ii][jj + 1]*concaveFunction(featurescoressetUpdt[ii][jj],type[1]);
                
                //cout<<"inner_sum = "<<inner_sum<<" inner_sum_add = "<<inner_sum_add<<endl;
                
            }
            summCurr += featureWeights[ii][0]*concaveFunction(inner_sum,type[0]);
            summUpdt += featureWeights[ii][0]*concaveFunction(inner_sum_add,type[0]);
        }
        
        //cout<<"summCurr = "<<summCurr<<" summUpdt = "<<summUpdt<<endl;
        gains = summUpdt - summCurr;
        
		return gains;
	}

	double FeatureBasedFunctionsTwoLayerDense::evalGainsremove(const Set& sset, int item) const{
	// Evaluation of function valuation.  At present Highly Inefficient (both computationally and from memory usage). Look into improving implementation
		if(!sset.contains(item)){
			cout<<"Error in using evalGainsremove: the provided item does not belong to the subset\n";
			return 0;
		}
        
        double inner_sum = 0.0;
        double inner_sum_removed = 0.0;
		double sum = 0;
		double sumd = 0;

        vector< vector<double> > featurescoresset (nFeatures.size(), vector<double>(0));
        vector< vector<double> > featurescoressetRemoved (nFeatures.size(), vector<double>(0));
        
		Set::const_iterator it(sset);
        
        //cout<<"Calculating Modular Funcs "<<endl;
        
        for (int j = 0; j < nFeatures.size(); j++){
            for (int k = 0; k < nFeatures[j]; k++) {
                int count = 0;
                int rem_count = 0;
                for( it = sset.begin(); it != sset.end(); ++it ) {
                    if (count == 0) {
                        // First time memory needs to be allocated. Hence, push_back
                        featurescoresset[j].push_back(feats[*it][j].featureVec[k]);
                        count++;
                    }
                    else {
                        featurescoresset[j][k] += feats[*it][j].featureVec[k];
                    
                    }
                    
                    if (rem_count == 0) {
                        
                        if ( *it != item ) {
                            featurescoressetRemoved[j].push_back(feats[*it][j].featureVec[k]);
                            rem_count++;
                        }
                        
                    }
                    else {
                        if ( *it != item ) {
                            featurescoressetRemoved[j][k] += feats[*it][j].featureVec[k];
                            
                        }
                        
                    }
                }
                //featurescoresset[j] += feats[*it].featureVec[j];
            }
        }
        
        //cout<<"Modular Funcs calculated"<<endl;
        for (int ii = 0; ii < nFeatures.size(); ii++){
            inner_sum = 0.0;
            inner_sum_removed = 0.0;
            for (int jj = 0; jj < nFeatures[jj]; jj++) {
                inner_sum +=  featureWeights[ii][jj + 1]*concaveFunction(featurescoresset[ii][jj],type[1]);
                inner_sum_removed += featureWeights[ii][jj+ 1]*concaveFunction(featurescoressetRemoved[ii][jj],type[1]);
            }
            sum += featureWeights[ii][0]*concaveFunction(inner_sum,type[0]);
            sumd += featureWeights[ii][0]*concaveFunction(inner_sum_removed,type[0]);
        }
        
		return (sum - sumd);
	}

	double FeatureBasedFunctionsTwoLayerDense::evalGainsaddFast(const Set& sset, int item) const{
	// Fast evaluation of Adding gains using the precomputed statistics. This is used, for example, in the implementation of the forward greedy algorithm.
		double gains = 0; 
		double temp;
		double diff;
		gains = 0;;
        
        for (int i = 0; i < nFeatures.size(); i++){
            double inner_sum_curr = 0.0;
            double inner_sum_add = 0.0;
            for (int j = 0; j < nFeatures[i]; j++) {
                inner_sum_curr += concaveFunction(preCompute[i][j],type[1]);
                inner_sum_add += concaveFunction((preCompute[i][j] + feats[item][i].featureVec[j]),type[1]);
            }
            gains += featureWeights[i][0]*(concaveFunction(inner_sum_add,type[0]) - concaveFunction(inner_sum_curr,type[0]));
        }
        
        return gains;
    }

	double FeatureBasedFunctionsTwoLayerDense::evalGainsremoveFast(const Set& sset, int item) const{
	// Fast evaluation of Removing gains using the precomputed statistics. This is used, for example, in the implementation of the reverse greedy algorithm.
		double gains = 0; 
		double temp;
		double diff;
        
        for (int i = 0; i < nFeatures.size(); i++){
            double inner_sum_curr = 0.0;
            double inner_sum_removed = 0.0;
            for (int j = 0; j < nFeatures[i]; j++) {
                inner_sum_curr += concaveFunction(preCompute[i][j],type[1]);
                inner_sum_removed += concaveFunction((preCompute[i][j] - feats[item][i].featureVec[j]),type[1]);
            }
            gains += featureWeights[i][0]*(concaveFunction(inner_sum_curr,type[0]) - concaveFunction(inner_sum_removed,type[0]));
        }
       
		return gains;
	}

	void FeatureBasedFunctionsTwoLayerDense::updateStatisticsAdd(const Set& sset, int item) const{
	// Update statistics for algorithms sequentially adding elements (for example, the greedy algorithm).
        
        for (int i = 0; i < nFeatures.size(); i++){
            for (int j = 0; j < nFeatures[i]; j++) {
                
                preCompute[i][j] += feats[item][i].featureVec[j];
            }
            
        }
        
		preComputeSet.insert(item);
	}

	void FeatureBasedFunctionsTwoLayerDense::updateStatisticsRemove(const Set& sset, int item) const{
	// Update statistics for algorithms sequentially removing elements (for example, the reverse greedy algorithm).

        
        for (int i = 0; i < nFeatures.size(); i++){
            for (int j = 0; j < nFeatures[i]; j++) {
                
                preCompute[i][j] -= feats[item][i].featureVec[j];
            }
            
        }
        
        preComputeSet.remove(item);
        
	}

	void FeatureBasedFunctionsTwoLayerDense::clearpreCompute() const{
		for (int i = 0; i < sizepreCompute.size(); i++) {
            for (int j = 0; j < sizepreCompute[i]; j++) {
                preCompute[i][j] = 0;
            }
            
        }
		preComputeSet.clear();
    }

	void FeatureBasedFunctionsTwoLayerDense::setpreCompute(const Set& sset) const{
		clearpreCompute();
		Set::const_iterator it(sset);
		for( it = sset.begin(); it != sset.end(); ++it ){
			updateStatisticsAdd(sset, *it);
		}
		preComputeSet = sset;
	}
}

