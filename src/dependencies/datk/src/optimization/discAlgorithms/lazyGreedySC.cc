/*
	Copyright (C) Rishabh Iyer 2015

 *	Implementation of the accelerated greedy algorithm for submodular set cover (Minoux 1976, Wolsey 1982).
	Solves the problem \min_{f(X) \geq C} m(X), where f is a monotone submodular function and c is a modular cost function.
	This greedy algorithm has a log(n) approximation guarantee for this problem
	Author: Rishabh Iyer
 *
	Input: Submodular Function: f
		Cost List: m
		cover: c
		GroundSet (in case it is not [n])
	Output: GreedySet: greedySet
 */
#include <queue>
#include "stdio.h"
#include <algorithm>
#include <cmath>
using namespace std;

#include "../../utils/error.h"
#include "../../utils/totalOrder.h"
#include "lazyGreedySC.h"
#include "increment.h"

namespace datk {
#define SMALLEST_NUMBER -1e70
#define LARGEST_NUMBER 1e70

void lazyGreedySC(SubmodularFunctions& f, vector<double> costList, double cover, Set& greedySet, int verbosity, 
double alpha, Set groundSet, bool grSetn)
{
	if (grSetn){
		groundSet = Set(f.size(), true); // GroundSet constructor
	}
	int nSelected = 0 ; // number of items selected
    	double maxV;
    	double preV = 0;
    	double objV = 0;
	double currentCost = 0;
	f.clearpreCompute(); // clear the precomputed statistics, in case it is already not cleared.
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
		    	if (objV < cover) 
			{
				greedySet.insert(topid);
				nSelected++;
				objV += newV;
				currentCost += costList[topid];
				if(verbosity >= 0) printf("Selecting %dth sample, curCost/cover = %.6e/%.6e, preV = %.10e, curV = %.10e, num_resort=%d\n", topid, currentCost, cover, preV, maxV, sort_cnt);
				preV = maxV;
				f.updateStatisticsAdd(greedySet, topid);
				sort_cnt = 0;
			}
		}    
	}	
}
}
