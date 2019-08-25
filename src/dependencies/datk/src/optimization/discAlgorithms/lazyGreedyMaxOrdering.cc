/*
	Copyright (C) Rishabh Iyer 2015
 *	The algorithm implemented here is the forward greedy algorithm, and has a 1 - 1/e guarantee for monotone submodular maximization!
	It greedily adds elements untill it violates the budget (and the gain is non-negative). This is an accelerated version with priority queue (Minoux 1976).
	Solves the problem \max_{|X| \leq B} f(X), where f is a submodular function.
	Anthor: Rishabh Iyer
	Based on an implementation by Hui Lin.
	Melodi Lab, University of Washington, Seattle
 *
	Input: Submodular Function: f
		budget: b
		GroundSet (in case it is not [n])
	Output: GreedySet: greedySet

	Suggested choice of the Set datastructure: VectorSet or Set

 */

#include <queue>
#include "stdio.h"
#include <algorithm>
#include <iostream>
using namespace std;

#include "../../utils/error.h"
#include "../../utils/totalOrder.h"
#include "lazyGreedyMax.h"
#include "increment.h"

namespace datk {

void lazyGreedyMaxOrdering(const SubmodularFunctions& f, Set& greedySet, std::vector<int>& greedyOrdering, int verbosity, bool precomputeSet, bool equalityConstraint, Set groundSet, bool grSetn)
{
	int nSelected = 0 ; // number of items selected
    	double maxV;
    	double preV = 0;
		double currentCost = 0;
	if (!precomputeSet){
		f.setpreCompute(greedySet); // clear the precomputed statistics, in case it is already not cleared, and set it to the greedySet.
	}

	// accelerated greedy algorithm implementation
	priority_queue <Increment> rho;
	// initilize the priority queue
    if (verbosity > 0){
        cout << "Start Initializing the priority queue" << endl;
    }
	if (grSetn){
		for (int i = 0; i < f.size(); i++) {
			rho.push(Increment(f.evalGainsaddFast(greedySet, i), i) );
		}
	}
	else{
		Set::iterator it;
		for (it = groundSet.begin(); it!= groundSet.end(); ++it) {
			rho.push(Increment(f.evalGainsaddFast(greedySet, *it), *it) );
		}
	}
	int sort_cnt = 0;
	while (! rho.empty()) {
		int topid = rho.top().get_index();
		rho.pop();
		maxV = preV + f.evalGainsaddFast(greedySet, topid);
		double newV = (maxV - preV); // return normalized gain
		if (verbosity >= 5) printf("max gain = %.6e, rho->top() = %.6e\n",newV,rho.top().get_value());
		if (newV < rho.top().get_value()) {
			rho.push(Increment(newV, topid)); // if condition not satisfied, push back and re-sort
			sort_cnt++;
			if (verbosity >= 10) printf("Condition not met, re-sorting queue (%d iterations)\n",sort_cnt);
		}
    else {
          // guaranteed to be optimal because of submodularity
          greedySet.insert(topid);
          greedyOrdering.push_back(topid);
            nSelected++;
            currentCost += 1;
            if(verbosity > 0) {
                printf("Iteration %d: Selecting %d th sample, curCost = %.6e, preV = %.10e, curV = %.10e, number of resorting operations=%d\n", nSelected, topid, currentCost, preV, maxV, sort_cnt);
            }
            preV = maxV;
            f.updateStatisticsAdd(greedySet, topid);
            sort_cnt = 0;
        }
	}
}
}
