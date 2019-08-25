/*
	Copyright (C) Rishabh Iyer 2015
 *	Header file for defining sparse version of facility location functions (f(X) = \sum_{i\in V} \max_{j\in S} w_{i,j}. In this case, the similarity graph is not complete. Instead, we only input a KNN graph, where each vertex is only connected to its K-nearest neighbors.
Author: Kai Wei (email: kaiwei@uw.edu)
	Melodi Lab, University of Washington, Seattle
 *
 */
#include <vector>
#include <algorithm>
#include <iostream>
#include <cmath>
#include "../../utils/error.h"
#include "../../utils/totalOrder.h"
#include <assert.h>
using namespace std;

#include "FacilityLocationSparse.h"
namespace datk {

	FacilityLocationSparse::FacilityLocationSparse(int n, vector<struct SparseGraphItem>& graph): SubmodularFunctions(n), graph(graph), sizepreCompute(n){
		preCompute.resize(sizepreCompute); 
        //cout << "Calling standard constructor" << endl;
	}
    FacilityLocationSparse::FacilityLocationSparse(const FacilityLocationSparse & f) : SubmodularFunctions(f), graph(f.graph), sizepreCompute(f.sizepreCompute) {
        preCompute = f.preCompute;
        preComputeSet = f.preComputeSet;
        //cout << "Calling copy constructor" << endl;
    }
	
    FacilityLocationSparse* FacilityLocationSparse::FacilityLocationSparse::clone() const{
        
        //cout <<"called FacilityLocationSparse clone" << endl;
        return new FacilityLocationSparse(*this);
    }

	FacilityLocationSparse::~FacilityLocationSparse(){
        //cout << " Now calling the destructor function of sparse facility location \n";
    }

	double FacilityLocationSparse::eval(const Set& sset) const{
	// Evaluation of function valuation.
		vector<double> maxVals(n, 0);
        
        int idx = 0;
		Set::const_iterator it;
		for (it = sset.begin(); it!= sset.end(); ++it){
			for (int i = 0; i < graph[*it].num_NN_items; i++){
		    		if (maxVals[graph[*it].NNIndex[i]] < graph[*it].NNSim[i]) {
                        if (graph[*it].NNIndex[i] >= n){ 
                            cout << "The sparse graph for the facility location function is not right, where the nearest neighbor takes index >= n";
                            assert(graph[*it].NNIndex[i] < n);
                        }
		        		maxVals[graph[*it].NNIndex[i]] = graph[*it].NNSim[i];
                    }
			}
            //cout << "Now compute " << idx << " for " << *it << endl;
            idx ++; 
		}
		double sum = 0;
		for (int ii=0; ii<n; ii++){
		    sum += maxVals[ii];           
		}

		return sum;
    }

	double FacilityLocationSparse::evalFast(const Set& sset) const{
	// Evaluation of function valuation.		
		double sum = 0;
        	for (int ii=0; ii<n; ii++){
            		sum += preCompute[ii];
        	}
		return sum;
	}

	double FacilityLocationSparse::evalGainsadd(const Set& sset, int item) const{
	// Evaluation of gains.
    // Sanity check if the item to be added is within sset
		if(sset.contains(item)){
				cout<<"Error in using evalGainsadd: the provided item already belongs to the subset\n";
				return 0; // I think here should be exit with error code
		}
        Set sset_added(sset);
        sset_added.insert(item);
        return (eval(sset_added) - eval(sset));
	}

	double FacilityLocationSparse::evalGainsremove(const Set& sset, int item) const{
	// Evaluation of function valuation.
    // For now, I am not implementing the removing function. Just leave it empty for now.
		if(!sset.contains(item)){
			cout<<"Error in using evalGainsremove: the provided item does not belong to the subset\n";
			return 0;
		}
        Set sset_deleted(sset);
        sset_deleted.remove(item);
        return (eval(sset) - eval(sset_deleted));
	}

	double FacilityLocationSparse::evalGainsaddFast(const Set& sset, int item) const{
	// Fast evaluation of Adding gains using the precomputed statistics. This is used, for example, in the implementation of the forward greedy algorithm.
		double gains = 0; 
		double temp;
		double diff;
		int num_wrds = graph[item].num_NN_items;
        //cout << "Number of NN indices " << num_wrds << endl;
		for (int i=0; i<num_wrds; i++){
			temp = preCompute[graph[item].NNIndex[i]];
            if (temp < graph[item].NNSim[i]){
                gains += graph[item].NNSim[i] - temp;
            }
		}
        //cout<<"Eval Fast value is "<<evalFast(sset)<<"\n";
        return gains; 
    }

	double FacilityLocationSparse::evalGainsremoveFast(const Set& sset, int item) const{
	// Fast evaluation of Removing gains using the precomputed statistics. This is used, for example, in the implementation of the reverse greedy algorithm.
    // we implement the naive version of removing an item. It could be slow. 
        return evalGainsremove(sset,item);
	}

	void FacilityLocationSparse::updateStatisticsAdd(const Set& sset, int item) const{
	// Update statistics for algorithms sequentially adding elements (for example, the greedy algorithm).
		for (int i = 0; i < graph[item].num_NN_items; i++){
            		if (preCompute[graph[item].NNIndex[i]] < graph[item].NNSim[i])
                		preCompute[graph[item].NNIndex[i]] = graph[item].NNSim[i];
		}
		preComputeSet.insert(item);
	}

	void FacilityLocationSparse::updateStatisticsRemove(const Set& sset, int item) const{
	// Update statistics for algorithms sequentially removing elements (for example, the reverse greedy algorithm).
    // I will later implement it, and leave it as empty for now. 
	}

	void FacilityLocationSparse::clearpreCompute() const{
		for (int i = 0; i < sizepreCompute; i++) { 
            preCompute[i] = 0; 
        }
		preComputeSet.clear();
	}

	void FacilityLocationSparse::setpreCompute(const Set& sset) const{
		clearpreCompute();
		Set::const_iterator it;
		for( it = sset.begin(); it != sset.end(); ++it ){
			updateStatisticsAdd(sset, *it);
		}
		preComputeSet = sset;
	}
}

