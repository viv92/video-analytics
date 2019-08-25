/*
 *	The randomized greedy algorithm for non-monotone submodular function maximization. 
	Solves the problem \max_{c(X) \leq B} f(X), where f is a non-monotone submodular function and k is the cardinality budget.
	Special case of this is the problem, \max_{|X| = k} f(X).
	Author: Rishabh Iyer
	Melodi Lab, University of Washington, Seattle

	Note: This algorithm is considerably slower than the accelerated greedy. It would be great if one could add acceleration tricks here.
 *
 */
#include <unordered_set>
#include <iostream>
#include <algorithm>
#include <queue>
#include "stdio.h"
using namespace std;

#include "../utils/error.h"
#include "../utils/totalOrder.h"
#include "../utils/sort.h"
#include "../functions/setFunctions.h"
#include "../functions/submodularFunctions.h"
#include "lazyGreedyMax.h"
#include "lazyGreedyMax.h"
namespace smtk {

#define SMALLEST_NUMBER -1e70
#define LARGEST_NUMBER 1e70

std::unordered_set<int> randomizedGreedyMax(submodularFunctions& f, int k, std::unordered_set<int> startSet, int verbosity){
	if(verbosity > 0) cout<<"Starting the randomized greedy algorithm\n"<<flush;
	std::unordered_set<int> sset = startSet;	
    	double currvalbest;
	int currid;
	double currentCost;
	int iter = 0;
	f.setpreCompute(startSet); // Set the precomputed statistics.
	unordered_set<int>::iterator it;
	int n = f.getSize();
	vector<double> currgains(n, 0);
	std::vector<size_t> currindexgains;
	std::vector<double> currsortedgains;
	int randchoice;
	// naive greedy algorithm implementation
	while (1) {
		iter++;
		currgains.assign(n, -LARGEST_NUMBER);
		for( it = f.getGroundSet().begin(); it != f.getGroundSet().end(); it++ ) {
			if(sset.find(*it) == sset.end()){
				currgains[*it] = f.evalGainsaddFast(sset, *it);
			}
		}
		srand(time(NULL));
		sort(currgains, currsortedgains, currindexgains);
		randchoice = rand() % k;
		currid = currindexgains[n - randchoice];
		if((iter <= k)){
			f.updateStatisticsAdd(sset, currid);
			currentCost+=1;
			sset.insert(sset.end(), currid);
			if(verbosity > 0) cout<<"Adding element "<< currid<<" in iteration "<<iter<<" and the objective value is "<<f.eval(sset)<<"\n"<<flush;
		}
		else{
			break;
		}
			    
	}	
	return sset;
}
}