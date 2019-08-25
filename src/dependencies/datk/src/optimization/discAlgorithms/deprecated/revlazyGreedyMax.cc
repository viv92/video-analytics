/*
 *	The reverse greedy algorithm starting with the groundset and greedily removing elements until there is no benefit in removing elements any more. 	
	This is an accelerated version with priority queue (Minoux 1976).
	Solves the problem \max_{X \subseteq V} f(X), where f is a non-monotone submodular function.
	Anthor: Rishabh Iyer
	Melodi Lab, University of Washington, Seattle

	Input: Submodular Function: f
	Input/Output: GreedySet: greedySet (output of the reverse Greedy algorithm)
	The algorithm starts with greedySet and removes elements from it until convergence

	Suggested choice of the Set datastructure: HashSet or BitVectorSet (efficient to remove elements)
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
#include "revlazyGreedyMax.h"
#include "increment.h"
namespace smtk {

#define SMALLEST_NUMBER -1e70
#define LARGEST_NUMBER 1e70

void revlazyGreedyMax(SubmodularFunctions& f, Set& greedySet, int verbosity, bool precomputeSet)
{
	int nSelected = 0 ; // number of items selected
    	double maxV;
	double newV;
    	double currentCost = 0;
	if (!precomputeSet){
		f.setpreCompute(greedySet); // clear the precomputed statistics, in case it is already not cleared, and set it to the greedySet.
	}
	double preV = f.evalFast(greedySet);
	
	// accelerated greedy algorithm implementation
	priority_queue <Increment> rho;
	// initilize the priority queue
	Set::iterator it(greedySet);
	for ( it = greedySet.begin(); it != greedySet.end(); ++it) { 
		rho.push(Increment(-f.evalGainsremoveFast(greedySet, *it ), *it )); 
	}
	int sort_cnt = 0;
	while (! rho.empty()) {
		int topid = rho.top().get_index();
		rho.pop();
		maxV = preV - f.evalGainsremoveFast(greedySet, topid);
		newV = - f.evalGainsremoveFast(greedySet, topid); // return normalized gain 
		if (verbosity >= 5) printf("max gain = %.6e, rho->top() = %.6e\n",newV,rho.top().get_value());
		if (newV < rho.top().get_value()) {
			rho.push(Increment(newV, topid)); // if condition not satisfied, push back and re-sort
			sort_cnt++;
			if (verbosity >= 10) printf("Condition not met, re-sorting queue (%d iterations)\n",sort_cnt);
		}
		else {
			// guaranteed to be optimal because of submodularity
			if(maxV - preV < 0) break;
			greedySet.remove(topid);
			nSelected++;
			if(verbosity > 0) printf("Selecting %dth sample: preV = %.10e, curV = %.10e\n", topid, preV, maxV);
			preV = maxV;
			f.updateStatisticsRemove(greedySet, topid);
			sort_cnt = 0;
		}    
	}	
}
}