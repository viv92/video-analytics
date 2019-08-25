/*
	Copyright (C) Rishabh Iyer 2015
 *	The algorithm implemented here is the forward greedy algorithm, and has a 1 - 1/e guarantee for monotone submodular maximization!
	It greedily adds elements untill it violates the budget (and the gain is non-negative). This is an accelerated version with priority queue (Minoux 1976).
	Solves the problem \max_{c(X) \leq B} f(X), where f is a submodular function and c is a modular cost function.
	Special case of this is the problem, \max_{|X| \leq k} f(X).
	Anthor: Rishabh Iyer
	Based on an implementation by Hui Lin. 
	Melodi Lab, University of Washington, Seattle

	Input: Submodular Function: f
		budget: b
		GroundSet (in case it is not [n])	
	Output: GreedySet: greedySet

 *
 */
#include <queue>
#include <algorithm>
#include <stdio.h>
#include <cmath>
using namespace std;

#include "../../utils/error.h"
#include "../../utils/totalOrder.h"
#include "lazyGreedyMaxKnapsack.h"
#include "increment.h"

namespace datk{
	
	void lazyGreedyMaxKnapsack(SubmodularFunctions& f, vector<double>& costList, double budget, Set& greedySet,  
	int verbosity, double alpha, Set groundSet, bool grSetn)	
{
	if (grSetn){
		groundSet = Set(f.size(), true); // GroundSet constructor
	}
	int nSelected = 0 ; // number of items selected
    	double maxV;
    	double preV = 0;
	double currentCost = 0;
	f.setpreCompute(greedySet); // clear the precomputed statistics, in case it is already not cleared, and set it to startSet.
	// accelerated greedy algorithm implementation
	priority_queue <Increment> rho;
	// initilize the priority queue
	Set::iterator it;
	for (it = groundSet.begin(); it!= groundSet.end(); ++it) { 
		rho.push(Increment(f.evalGainsaddFast(greedySet, *it) / pow(costList[*it],alpha),*it ) ); 
	}
	int sort_cnt = 0;
	while (! rho.empty()) {
		int topid = rho.top().get_index();
		rho.pop();
		maxV = preV + f.evalGainsaddFast(greedySet, topid);
		double newV = (maxV - preV)/pow(costList[topid],alpha); // return normalized gain 
		if (verbosity >= 5) printf("max gain = %.6e, rho->top() = %.6e\n",newV,rho.top().get_value());
		if (newV < rho.top().get_value()) {
			rho.push(Increment(newV, topid)); // if condition not satisfied, push back and re-sort
			sort_cnt++;
			if (verbosity >= 10) printf("Condition not met, re-sorting queue (%d iterations)\n",sort_cnt);
		}
		else {
			// guaranteed to be optimal because of submodularity
		    	if ((currentCost + costList[topid] < budget) && (newV >= 0))
			{
				greedySet.insert(topid);
				nSelected++;
				currentCost += costList[topid];
				if(verbosity > 0) printf("Iteration %d: Selecting %d th sample, curCost/budget = %.6e/%.6e, preV = %.10e, curV = %.10e, number of resorting operations=%d\n", nSelected,topid, currentCost, budget, preV, maxV, sort_cnt);
				preV = maxV;
				f.updateStatisticsAdd(greedySet, topid);
				sort_cnt = 0;
			}
		}    
	}	
}
}