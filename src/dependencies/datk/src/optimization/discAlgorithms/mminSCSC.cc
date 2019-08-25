/*
	Copyright (C) Rishabh Iyer 2015

 *	Implementation of the Majorization-Minimization Supergradient Algorithm for Submodular Cost Submodular Knapsack (SCSK) Problem.
	The problem is: \max_{X: g(X) \geq C} f(X), where f and g are monotone submodular function.
	Uses the idea of iteratively minimizing upper bounds computed via supergradients (Iyer-Jegelka-Bilmes, ICML-2013)
	Author: Rishabh Iyer
	Melodi Lab, University of Washington, Seattle
 *
 */

#include <unordered_set>
#include <queue>
#include <algorithm>
#include "stdio.h"
#include <iostream>
using namespace std;

#include "../../utils/error.h"
#include "../../utils/totalOrder.h"
#include "lazyGreedyMax.h"
#include "lazyGreedySC.h"

namespace datk {
#define EPSILON 1e-10

	void mminSCSC(SubmodularFunctions& f, SubmodularFunctions& g, Set& currset, double cover, int verbosity, Set startSet){
	std::vector<double> muppera;
	std::vector<double> mupperb;
	double moffseta = 0;
	double moffsetb = 0;
	muppera.assign(f.size(),0);
	mupperb.assign(f.size(),0);
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
		lazyGreedySC(g, muppera, cover, newseta, -1, 1);
		lazyGreedySC(g, mupperb, cover, newsetb, -1, 1);
		if (f.eval(newseta) < f.eval(newsetb))
			newset = newseta;
		else
			newset = newsetb;
		currVal = f.eval(currset);
		newVal = f.eval(newset);
		if(verbosity > 0) cout<<"Current value is "<< newVal<<" in iteration "<<iter<<"\n"<<flush;
		if (iter == 0){
			currset = newset;
		}
		else if (newVal - currVal < -EPSILON){
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


