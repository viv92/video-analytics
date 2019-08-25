/*
 *	Header file for defining sparse version of facility location functions (f(X) = \sum_{i\in V} \max_{j\in S} w_{i,j}. In this case, the similarity graph is not complete. Instead, we only input a KNN graph, where each vertex is only connected to its K-nearest neighbors.
Author: Kai Wei (email: kaiwei@uw.edu)
	Melodi Lab, University of Washington, Seattle
 *
 */
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <iostream>
#include <cmath>
#include "../utils/error.h"
#include "../utils/totalOrder.h"
#include <assert.h>
#ifdef _OPENMP_
#include<omp.h> 
#endif
using namespace std;

#include "FacilityLocationSparseDB.h"
namespace smtk {

	FacilityLocationSparseDB::FacilityLocationSparseDB(int n, SparseRepresentation& graph): SubmodularFunctions(n), graph(graph), sizepreCompute(n){
		preCompute.resize(sizepreCompute); 
	}
    FacilityLocationSparseDB::FacilityLocationSparseDB(const FacilityLocationSparseDB & f) : SubmodularFunctions(f), graph(f.graph), sizepreCompute(f.sizepreCompute) {
        preCompute = f.preCompute;
        preComputeSet = f.preComputeSet;
    }


	
    FacilityLocationSparseDB* FacilityLocationSparseDB::FacilityLocationSparseDB::clone() const{
        
        //cout <<"called FacilityLocationSparseDB clone" << endl;
        return new FacilityLocationSparseDB(*this);
    }

	FacilityLocationSparseDB::~FacilityLocationSparseDB(){
        //preCompute = vector<double>();
        //cout << " Now calling the destructor function of sparse facility location \n";
    }


    void FacilityLocationSparseDB::resetData(std::vector<int> Rset){
        // 
        graph.resetRepresentationSet(Rset);
    }

	double FacilityLocationSparseDB::eval(const Set& sset) const{
	// Evaluation of function valuation.
		vector<double> maxVals(n, 0);
        int idx = 0;
		Set::const_iterator it(sset);
		for (it = sset.begin(); it!= sset.end(); ++it){
			for (int i = 0; i < graph[*it].num_uniq_wrds; i++){
		    		if (maxVals[graph[*it].featureIndex[i]] < graph[*it].featureVec[i]) {
                        if (graph[*it].featureIndex[i] >= n){ 
                            cout << "I'm broken: *it=" << *it << " i= "<<i<< " index value: " << graph[*it].featureIndex[i]  << endl;
                            cout << "The sparse graph for the facility location function is not right, where the nearest neighbor takes index >= n";
                            assert(graph[*it].featureIndex[i] < n);
                        }
		        		maxVals[graph[*it].featureIndex[i]] = graph[*it].featureVec[i];
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

	double FacilityLocationSparseDB::evalFast(const Set& sset) const{
	// Evaluation of function valuation.		
		double sum = 0;
        //cout << "N="<<n<<endl;
        //cout << "Preocompute size: " << preCompute.size() << endl;
        	for (int ii=0; ii<n; ii++){
            		sum += preCompute[ii];
        	}
		return sum;
	}

	double FacilityLocationSparseDB::evalGainsadd(const Set& sset, int item) const{
	// Evaluation of gains.
    // Sanity check if the item to be added is within sset
		if(sset.contains(item)){
				cout<<"Error in using evalGainsadd: the provided item already belongs to the subset\n";
				return 0; // I think here should be exit with error code
		}
        HashSet sset_added(sset);
        sset_added.insert(item);
        return (eval(sset_added) - eval(sset));
	}

	double FacilityLocationSparseDB::evalGainsremove(const Set& sset, int item) const{
	// Evaluation of function valuation.
    // For now, I am not implementing the removing function. Just leave it empty for now.
		if(!sset.contains(item)){
			cout<<"Error in using evalGainsremove: the provided item does not belong to the subset\n";
			return 0;
		}
        HashSet sset_deleted(sset);
        sset_deleted.remove(item);
        return (eval(sset) - eval(sset_deleted));
	}

	double FacilityLocationSparseDB::evalGainsaddFast(const Set& sset, int item) const{
	// Fast evaluation of Adding gains using the precomputed statistics. This is used, for example, in the implementation of the forward greedy algorithm.
		double gains = 0; 
		double temp;
		double diff;
		int num_wrds = graph[item].num_uniq_wrds;
        //cout << "Number of NN indices " << num_wrds << endl;
        //#pragma omp parallel for reduction(+:gains)
        // it seems the above code for parallelizing the implementation is not correct. 
		for (int i=0; i<num_wrds; i++){
			temp = preCompute[graph[item].featureIndex[i]];
            if (temp < graph[item].featureVec[i]){
                gains += graph[item].featureVec[i] - temp;
            }
		}
        //cout<<"Eval Fast value is "<<evalFast(sset)<<"\n";
        return gains; 
    }

	double FacilityLocationSparseDB::evalGainsremoveFast(const Set& sset, int item) const{
	// Fast evaluation of Removing gains using the precomputed statistics. This is used, for example, in the implementation of the reverse greedy algorithm.
    // we implement the naive version of removing an item. It could be slow. 
        return evalGainsremove(sset,item);
	}

	void FacilityLocationSparseDB::updateStatisticsAdd(const Set& sset, int item) const{
	// Update statistics for algorithms sequentially adding elements (for example, the greedy algorithm).
        
		//#pragma omp parallel for // we have to be careful about parallelization. becuase it may cause unexpected problems that are hard to debug. 
        // I found if enabled parallelization, I will run into problems that can't be explained when I run the secretary algorithm with d=1 without outofcore setting. 
		for (int i = 0; i < graph[item].num_uniq_wrds; i++){
            		if (preCompute[graph[item].featureIndex[i]] < graph[item].featureVec[i])
                		preCompute[graph[item].featureIndex[i]] = graph[item].featureVec[i];
		}
		preComputeSet.insert(item);
	}

	void FacilityLocationSparseDB::updateStatisticsRemove(const Set& sset, int item) const{
	// Update statistics for algorithms sequentially removing elements (for example, the reverse greedy algorithm).
    // I will later implement it, and leave it as empty for now. 
	}

	void FacilityLocationSparseDB::clearpreCompute() const{
		for (int i = 0; i < sizepreCompute; i++) { 
            preCompute[i] = 0; 
        }
		preComputeSet.clear();
	}

	void FacilityLocationSparseDB::setpreCompute(const Set& sset) const{
		clearpreCompute();
		Set::const_iterator it (sset);
		for( it = sset.begin(); it != sset.end(); ++it ){
			updateStatisticsAdd(sset, *it);
		}
		preComputeSet = sset;
	}
}

