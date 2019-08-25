/*
	Copyright (C) Rishabh Iyer 2015

 *	The naive greedy algorithm for set function maximization. This algorithm has guarantees if the function is monotone submodular, under cardinality/knapsack or matroid constraints. In general, though, it is a useful heuristic, which seems to work well in practice.
	Solves the problem \max_{|X| \leq B} f(X), where f is a monotone submodular function.
	Author: Rishabh Iyer
	Melodi Lab, University of Washington, Seattle

	Note: This algorithm is considerably slower than the accelerated greedy, so if you have a reason to believe your function is submodular (or approximately so), 		please use the lazy greedy algorithm.
 *
 */

#include <vector>
#include <iostream>
using namespace std;

#include "../../utils/error.h"
#include  "naiveGreedyMax.h"


namespace datk {
void naiveGreedyMaxOrdering(SetFunctions& f, Set& greedySet, std::vector<int>& greedyOrdering, int verbosity)
{
	Set groundSet = Set(f.size(), true); // GroundSet constructor
	if(verbosity > 0) cout<<"Starting the naive greedy algorithm\n"<<flush;
   	double currvalbest;
	int curridbest;
	double currentCost;
	int iter = 0;
	f.setpreCompute(greedySet); // clear the precomputed statistics, in case it is already not cleared, and set it to the greedySet.
	// naive greedy algorithm implementation
	Set::iterator it;
	while (1) {
		iter++;
		currvalbest = -1;
		for( it = groundSet.begin(); it != groundSet.end(); ++it ) {
			if( !greedySet.contains(*it) && (f.evalGainsaddFast(greedySet, *it) > currvalbest)){
				currvalbest = f.evalGainsaddFast(greedySet, *it);
				curridbest = *it;
			}
		}
		if (verbosity > 0) cout<<"Current best value is "<<currvalbest<<"\n"<<flush;
			f.updateStatisticsAdd(greedySet, curridbest);
			currentCost+=1;
			greedySet.insert(curridbest);
      greedyOrdering.push_back(curridbest);
			if(verbosity > 0) cout<<"Adding element "<< curridbest<<" in iteration "<<iter<<" and the objective value is "<<f(greedySet)<<"\n"<<flush;
	}
}

}
