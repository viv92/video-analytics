// 	Copyright (C) Rishabh Iyer 2015

// Implementation of MMin-I/ II for unconstrained submodular minimization.
// This acts as a preprocessing step for general purpose SFM algorithms.
// Author: Rishabh Iyer 

#include <unordered_set>
#include <iostream>
#include <algorithm>
#include <queue>
#include "stdio.h"
using namespace std;

#include "../utils/error.h"
#include "../utils/totalOrder.h"
#include "../functions/SetFunctions.h"
#include "../functions/SubmodularFunctions.h"
#include "../sets/Set.h"
#include "../sets/HashSet.h"
#include "../sets/SetOperations.h"

namespace smtk {
	
template <class Container>
void mminUC(SubmodularFunctions& f, Container& minStartSet, Container& maxStartSet){
	minStartSet = Container();
	maxStartSet = Container(f.size(), true);
	Container addSet = Container();
	Container removeSet = Container();
	bool stuck;
	while(1){
		stuck = 1;
		addSet.clear();
		f.setpreCompute(minStartSet);
		for (int i = 0; i < f.size(); i++){
			if( (!minStartSet.contains(i)) && (f.evalGainsaddFast(minStartSet, i) < 0)){ // *it is not in minStartSet and adding it to minStartSet reduces value
				addSet.insert(i);
				stuck = 0;
			}
		}
		minStartSet += addSet;
		if(stuck == 1) break;
	}

	while(1){
		stuck = 1;
		removeSet.clear();
		f.setpreCompute(maxStartSet);
		for (int i = 0; i < f.size(); i++){
			if( (maxStartSet.contains(i)) && (f.evalGainsremoveFast(maxStartSet, i) >= 0)){ // *it is in minSTartSet and removing it from maxStartSet reduces value 
				removeSet.insert(i);
				stuck = 0;
			}
		}
		maxStartSet -= removeSet;
		if(stuck == 1) break;
	}
}

template void mminUC(SubmodularFunctions& f, HashSet& minStartSet, HashSet& maxStartSet);

}
