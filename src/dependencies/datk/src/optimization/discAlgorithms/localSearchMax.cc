/*
	Copyright (C) Rishabh Iyer 2015

*	The Local Search Algorithm for non-monotone submodular Maximization by Feige et al (2007). This gives a 1/3 approximation for unconstrained maximization.
	This is an accelerated version with priority queue (Minoux 1976).
	Solves the problem \max_{X \subseteq V} f(X), where f is a non-monotone submodular function. 
	Special case of this is the problem, \max_{|X| \leq k} f(X).
	Anthor: Rishabh Iyer
	Melodi Lab, University of Washington, Seattle

	Input: Submodular Function: f
		GroundSet (in case it is not [n])
	Input/Output: localSet
	The algorithm starts with localSet and performs the local search algorithm

	Suggested choices of Set: Set or BitVectorSet
 *
 */
#include <iostream>
#include <queue>
#include "stdio.h"
#include <algorithm>
using namespace std;

#include "../../utils/error.h"
#include "../../utils/totalOrder.h"
#include "localSearchMax.h"
#include "lazyGreedyMax.h"
#include "revlazyGreedyMax.h"

namespace datk {
#define LARGE_NUM 1e50
#define EPSILON 1e-10

template <class Container>
void localSearchMax(SubmodularFunctions& f, Container& localSet, int verbosity, Container groundSet, bool grSetn){
	int nSelected = 0 ; // number of items selected
	Set prevSet = localSet;
	f.setpreCompute(localSet);
	double prevVal = f.evalFast(localSet);
	double newVal = 0;
	while(1){
		// Upward Greedy Pass: Start with localSet and Greedily add elements till the objective value no longer increases.
		lazyGreedyMax(f, LARGE_NUM, localSet, verbosity, true, false, groundSet, grSetn);
		newVal = f.evalFast(localSet);
		if(verbosity > 0)
        	cout<<"Value after upward Pass is "<<newVal<<".\n";
		
        if(newVal - prevVal < EPSILON)
        	break;
        else{
        	prevSet = localSet;
			prevVal = f.evalFast(localSet);
		}
		// Reverse Greedy Pass: Start with localSet and greedily remove elements till the objective value no longer increases.
		revlazyGreedyMax(f, localSet, verbosity, true);
		newVal = f.evalFast(localSet);
		if(verbosity > 0)
        	cout<<"Value after downward Pass is "<<newVal<<".\n";
        if(newVal - prevVal < EPSILON)
        	break;
        else{
        	prevSet = localSet;
			prevVal = f.evalFast(localSet);
		}  
	}
}

template void localSearchMax(SubmodularFunctions& f, Set& localSet, int verbosity, Set groundSet, bool grSetn);

}
