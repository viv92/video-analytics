/*

	Copyright (C) Rishabh Iyer 2015

 *	Implementation of the Majorization-Minimization Supergradient Algorithm for Submodular Cost Submodular Knapsack (SCSK) Problem.
	The problem is: \max_{X: f(X) \leq B} g(X), where f and g are monotone submodular function.
	Uses the idea of iteratively minimizing upper bounds computed via supergradients (Iyer-Jegelka-Bilmes, ICML-2013)
	Author: Rishabh Iyer
	Melodi Lab, University of Washington, Seattle
 *
 */
#include <unordered_set>
#include <iostream>
#include <algorithm>
#include <queue>
#include "stdio.h"
using namespace std;

#include "../../utils/totalOrder.h"
#include "mminSCSK.h"
#include "lazyGreedyMaxKnapsack.h"
namespace datk {
#define EPSILON 1e-10

	void mminSCSK(SubmodularFunctions& f, SubmodularFunctions& g, Set& currset, double budget, int verbosity, Set startSet){
	std::vector<double> muppera;
	std::vector<double> mupperb;
	double moffseta = 0;
	double moffsetb = 0;
	muppera.assign(f.size(),0);
	mupperb.assign(f.size(),0);
	Set emptyset = Set();
	currset = startSet;
	Set newset;
	Set newseta;
	Set newsetb;
	int iter = 0;
	double newVal = 0;
	double currVal = 0;
	while(1){
		f.modularUpperBoundA(currset, muppera, moffseta);
		f.modularUpperBoundB(currset, mupperb, moffsetb);
		lazyGreedyMaxKnapsack(g, muppera, budget - moffseta, newseta, 0, 1);
		lazyGreedyMaxKnapsack(g, mupperb, budget - moffsetb, newsetb, 0, 1);
		if (g.eval(newseta) > g.eval(newsetb))
			newset = newseta;
		else
			newset = newsetb;
		currVal = g.eval(currset);
		newVal = g.eval(newset);
		if(verbosity > 0) cout<<"Current value is "<< newVal<<" in iteration "<<iter<<"\n"<<flush;
		if (iter == 0){
			currset = newset;
		}
		else if (newVal - currVal > EPSILON){
			currset = newset;
		}
		else{
			break;
		}
		iter++;
	}
	return;
}

}


