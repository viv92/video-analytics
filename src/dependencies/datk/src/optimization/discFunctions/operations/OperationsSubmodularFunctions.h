/*
 *	A helper function, which takes in a submodular function f and a set of sets A_1, A_2, ... A_k and computes the function h(X) = \sum_{i =1}^k f(X \cap A_i).
	Melodi Lab, University of Washington, Seattle
 *
 */
#ifndef OPERATIONS_SF_H
#define OPERATIONS_SF_H
#include <vector>
#include "../sets/Set.h"
#include "../sets/HashSet.h"
#include "SetFunctions.h"
#include "SubmodularFunctions.h"
#include "ModularFunctions.h"
#include "CombineSubmodularFunctions.h"
#include "ComplementSubmodularFunctions.h"
#include "AddSubmodularFunctions.h"
#include "ScaleSubmodularFunctions.h"
#include "DifferenceSubmodularFunctions.h"
#include "DifferenceSubmodularModularFunctions.h"
#include "ConcaveSubmodularFunctions.h"

namespace smtk {
	CombineSubmodularFunctions ClusteredSubmodularFunctions(const SubmodularFunctions& f, const std::vector<HashSet>& clusters);
	
	ComplementSubmodularFunctions operator! (const SubmodularFunctions& f);
	
	AddSubmodularFunctions operator+ (const SubmodularFunctions& f, const SubmodularFunctions& g);

	AddSubmodularFunctions operator+ (const SubmodularFunctions& f, const double lambda);
	
	AddSubmodularFunctions operator+ (const SubmodularFunctions& f, const std::vector<double> weight);
	
	ScaleSubmodularFunctions operator* (const SubmodularFunctions& f, const double lambda);
	
	ScaleSubmodularFunctions operator* (const double lambda, const SubmodularFunctions& f);
	
	DifferenceSubmodularFunctions operator- (const SubmodularFunctions& f, const SubmodularFunctions& g);

	DifferenceSubmodularModularFunctions operator- (const SubmodularFunctions& f, const ModularFunctions& m);
	
	DifferenceSubmodularFunctions operator- (const SubmodularFunctions& f);
	
	ConcaveSubmodularFunctions log(const SubmodularFunctions& f);
	
	ConcaveSubmodularFunctions sqrt(const SubmodularFunctions& f);
	
	ConcaveSubmodularFunctions min(const SubmodularFunctions& f, double K);
	
	ConcaveSubmodularFunctions pow(const SubmodularFunctions& f, double exponent);
	
			
}
#endif

