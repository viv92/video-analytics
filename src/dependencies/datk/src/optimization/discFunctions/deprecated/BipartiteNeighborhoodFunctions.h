/*
 *	Header file for defining a family of graph cut based functions: f(X) = w(N(X)), where N(X) is the bipartite-neighbor set of X.
    For example, if V = {1, 2, 3} and the set of covering neighbors are N = {a,b,c,d,e}, then we can have N(1) = {a,b,e}, N(2) = {b, c} and N(3) = {a,c,d}.
	w is a weight of each item of N.
	In this example, f is a complexity function of speech corpra. V refers to the set of utterances, while N refers to the vocabulary induced by the speech.
	Author: Rishabh Iyer and Yuzong Liu
	Email: rkiyer@uw.edu, yzliu@uw.edu
	Melodi Lab, University of Washington, Seattle
 *
 */
#ifndef SMTK_BIPARTITENEIGHBOR
#define SMTK_BIPARTITENEIGHBOR
#include <vector>
#include "SetFunctions.h"
#include "SubmodularFunctions.h"
#include "../sets/Set.h"
#include "../sets/HashSet.h"
#include "../sets/VectorSet.h"

namespace smtk {
class BipartiteNeighborhoodFunctions: public SubmodularFunctions
{
public:
	std::vector<double> weight; // the modular weight in front of each neighbor element
	std::vector<HashSet>& neighbors; // {N(i)}_{i \in V}
	mutable unordered_map<int,int> preCompute; // Precomputed Statistics
	int numNeighbors; // Size of the neighborhood set |N|
	mutable double currentVal;

	// Functions
	BipartiteNeighborhoodFunctions(int n, std::vector<double>& weight, std::vector<HashSet>& neighbors, int numNeighbors);
	BipartiteNeighborhoodFunctions(int n, std::vector<HashSet>& neighbors, int numNeighbors);	
	BipartiteNeighborhoodFunctions(const BipartiteNeighborhoodFunctions &b);
    BipartiteNeighborhoodFunctions * clone() const;
    
	double eval(const Set& sset) const;
	double evalFast(const Set& sset) const;
	double evalGainsadd(const Set& sset, int item) const;
	double evalGainsremove(const Set& sset, int item) const;
	double evalGainsaddFast(const Set& sset, int item) const;
	double evalGainsremoveFast(const Set& sset, int item) const;
	void updateStatisticsAdd(const Set& sset, int item) const;
	void updateStatisticsRemove(const Set& sset, int item) const;
	void clearpreCompute() const;
	void setpreCompute(const Set& current_set) const;
	HashSet getVocabulary(const Set& sset) const;
};
}
#endif
