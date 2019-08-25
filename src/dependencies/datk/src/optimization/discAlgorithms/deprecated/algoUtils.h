#ifndef ALGO_UTILS_H
#define ALGO_UTILS_H

namespace smtk {

#include "../sets/Set.h"
#include "../functions/SubmodularFunctions.h"	
#include "../functions/SetFunctions.h"	
	
#define EPSILON 1e-10
#define LARGE_NUM 1e50

inline void getRandomSubgradient(SubmodularFunctions& f, Set& sset, vector<double>& subgradient){
// Outputs a random subgradient at sset.
	srand(time(NULL));
	vector<double> orderedVec(f.size());
	subgradient = vector<double>(f.size(), 0);
	totalOrder<double> order(f.size());
	for(int i = 0; i < f.size(); i++){
		if(sset.contains(i)){ // i belongs to sset
			orderedVec[i] = -LARGE_NUM - rand()%(f.size());
		}
		else{
			orderedVec[i] = -rand()%(f.size());
		}
	}
	order.resetOrderDec(orderedVec);
	f.getSubgradient(order, sset, subgradient);
	return;	
}

inline void getEfficientSubgradient(SubmodularFunctions& f, SetFunctions& h, Set& sset, vector<double>& subgradient){
// Outputs an efficient subgradient based on a heuristic of arranging the gains of the difference function h.
	srand(time(NULL));
	vector<double> orderedVec(f.size());
	subgradient = vector<double>(f.size(), 0);
	totalOrder<double> order(f.size()) ;
	for(int i = 0; i < f.size(); i++){
		if(sset.contains(i)){ // i belongs to sset
			orderedVec[i] = -LARGE_NUM + h.evalGainsremoveFast(sset, i);
		}
		else{
			orderedVec[i] = h.evalGainsaddFast(sset, i);
		}
	}
	order.resetOrderDec(orderedVec);
	f.getSubgradient(order, sset, subgradient);
	return;	
}
}
#endif

