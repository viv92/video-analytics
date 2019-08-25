/*
	Copyright (C) Rishabh Iyer 2015
 *	The algorithm implemented here is a streaming greedy algorithm.
	It incrementally adds elements such that f(j | X) >= \epsilon.
	Solves the problem \max_{|X| \leq B} f(X), where f is a submodular function, in a streaming setting.
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
#include <assert.h>
using namespace std;

#include "../../utils/error.h"
#include "../../utils/totalOrder.h"
#include "streamGreedy.h"
#include "increment.h"

namespace datk {

void streamGreedy(const SetFunctions& f, double epsilon, Set& greedySet, vector<int>& stream, int verbosity)
{
	int nSelected = 0 ; // number of items selected
    	double maxV;
    	double preV = 0;
		double currentCost = 0;
		f.setpreCompute(greedySet); // clear the precomputed statistics, in case it is already not cleared, and set it to the greedySet.
	// accelerated greedy algorithm implementation
		// make sure that the length of the stream is the ground set size
		assert(stream.size() == f.size());
		for (int i = 0; i < stream.size(); i++){
			double val = f.evalGainsaddFast(greedySet, stream[i]);
			if (val > epsilon){
				greedySet.insert(stream[i]);
	      f.updateStatisticsAdd(greedySet, stream[i]);
				if (verbosity > 0)
	                printf("Added item %d, gains = %f, epsilon = %f\n", stream[i], val, epsilon);
			}
		}
}
}
