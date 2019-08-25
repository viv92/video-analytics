/*
	Copyright (C) Rishabh Iyer 2015
*	The Bi-directional Greedy Algorithm for non-monotone submodular Maximization by Buchbinder et al (2007). This gives a deterministic 1/3 approximation, and a randomized 1/2 approximation for unconstrained maximization.
	Solves the problem \max_{X \subseteq V} f(X), where f is a non-monotone submodular function. 
	Anthor: Rishabh Iyer
	Melodi Lab, University of Washington, Seattle
	The variable mode decides on the algorithm (randomized or deterministic greedy).
 *
 */

#include <queue>
#include <algorithm>
#include <time.h>
#include "stdio.h"
using namespace std;

#include "../../utils/error.h"
#include "bidirGreedyMax.h"
#include "../discFunctions/SubmodularFunctions.h"	

namespace datk {
#define LARGE_NUM 1e50
#define EPSILON 1e-10

template <class Container>
void bidirGreedyMax(const SubmodularFunctions& f, Container& incSet, totalOrder<int> initialOrdering, int verbosity, int mode){
	srand(time(NULL));
	SubmodularFunctions* fcopy = f.clone();
	incSet.clear();
	Container decSet = Container(f.size(), true);
	// Create a copy of f as fcopy
	double alpha;
	double beta;
	f.setpreCompute(incSet);
	fcopy->setpreCompute(decSet);
	for(int i = 0; i < f.size(); i++){
		if (mode == 0){ // The deterministic BD Greedy
			alpha = f.evalGainsaddFast(incSet, initialOrdering[i]);
			beta = -fcopy->evalGainsremoveFast(decSet, initialOrdering[i]);
			if (alpha >= beta){
				f.updateStatisticsAdd(incSet, initialOrdering[i]);
				incSet.insert(initialOrdering[i]);
			}
			else {
				fcopy->updateStatisticsRemove(decSet, initialOrdering[i]);
				decSet.remove(initialOrdering[i]);
			}
		}
		else{ // The randomized BD Greedy
			alpha = max(f.evalGainsaddFast(incSet, initialOrdering[i]), 0.0);
			beta = max(-fcopy->evalGainsremoveFast(decSet, initialOrdering[i]), 0.0);
			double p = alpha/(alpha + beta);
			bool TrueFalse = (rand() % 100) < 100*p;
			if(TrueFalse == 1){
				f.updateStatisticsAdd(incSet, initialOrdering[i]);
				incSet.insert(initialOrdering[i]);
			}
			else {
				fcopy->updateStatisticsRemove(decSet, initialOrdering[i]);
				decSet.remove(initialOrdering[i]);
			}
		}
	}
	delete fcopy;
	return;
}

template void bidirGreedyMax<Set>(const SubmodularFunctions& f, Set& incSet, totalOrder<int> initialOrdering, int verbosity, int mode);
}