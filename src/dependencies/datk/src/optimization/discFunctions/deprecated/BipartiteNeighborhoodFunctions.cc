/*
 *	Defining a family of graph cut based functions: f(X) = w(N(X)), where N(X) is the bipartite-neighbor set of X.
    For example, if V = {1, 2, 3} and the set of covering neighbors are N = {a,b,c,d,e}, then we can have N(1) = {a,b,e}, N(2) = {b, c} and N(3) = {a,c,d}.
	w is a weight of each item of N.
	In this example, f is a complexity function of speech corpra. V refers to the set of utterances, while N refers to the vocabulary induced by the speech.
	Author: Rishabh Iyer and Yuzong Liu
	Email: rkiyer@uw.edu, yzliu@uw.edu
	Melodi Lab, University of Washington, Seattle
 *
 */


#include <vector>
#include <iostream>
#include <unordered_map>
#include <algorithm> 
#include "../utils/error.h"
using namespace std;

#include "BipartiteNeighborhoodFunctions.h"
namespace smtk {
BipartiteNeighborhoodFunctions::BipartiteNeighborhoodFunctions(int n, std::vector<double>& weight, std::vector<HashSet>& neighbors, int numNeighbors): 
SubmodularFunctions(n), weight(weight), neighbors(neighbors), numNeighbors(numNeighbors)
{
	for (int i = 0; i < numNeighbors; i++)
	{
		preCompute[i] = 0;
	}
	currentVal = 0;
}

BipartiteNeighborhoodFunctions::BipartiteNeighborhoodFunctions(int n, std::vector<HashSet>& neighbors, int numNeighbors): 
SubmodularFunctions(n), neighbors(neighbors), numNeighbors(numNeighbors)
{// Choose default score of 1.
	for (int i = 0; i < numNeighbors; i++)
	{
		preCompute[i] = 0;
		weight.push_back(1);
	}
	currentVal = 0;
}

BipartiteNeighborhoodFunctions::BipartiteNeighborhoodFunctions(const BipartiteNeighborhoodFunctions &b): 
SubmodularFunctions(b.n), weight(b.weight), neighbors(b.neighbors), numNeighbors(b.numNeighbors){}

BipartiteNeighborhoodFunctions* BipartiteNeighborhoodFunctions::clone() const{
	return new BipartiteNeighborhoodFunctions(*this);
}

double BipartiteNeighborhoodFunctions::eval(const Set& sset) const
{	
	double val = 0;
	HashSet subset_tokens;
	Set::const_iterator it(sset);
	for (it = sset.begin(); it != sset.end(); ++it)
	{
		Set::iterator it2(neighbors[*it]);
		for (it2 = neighbors[*it].begin(); it2 != neighbors[*it].end(); ++it2)
		{
			if (!subset_tokens.contains(*it2))
			{
				val+=weight[*it2];
				subset_tokens.insert(*it2);
			}			
		}
	}	
	
	return val;
}



double BipartiteNeighborhoodFunctions::evalFast(const Set& sset) const
{	
	return currentVal;
}


double BipartiteNeighborhoodFunctions::evalGainsadd(const Set& sset, int item) const
{
	double increment = 0;	
	
	// get the set of the corresponding tokens
	HashSet subset_tokens;
	Set::const_iterator it(sset);
	for (it = sset.begin(); it != sset.end(); ++it)
	{
		Set::iterator it2(neighbors[*it]);
		for (it2 = neighbors[*it].begin(); it2 != neighbors[*it].end(); ++it2)
			subset_tokens.insert(*it2);				
	}	
	
	// get increment
	Set::iterator it3 (neighbors[item]);
	for (it3 = neighbors[item].begin(); it3 != neighbors[item].end(); ++it3)
	{
		if (!subset_tokens.contains(*it3))
		{
			increment+=weight[*it3];
		}			
	}	
	return increment;
}

double BipartiteNeighborhoodFunctions::evalGainsremove(const Set& sset, int item) const
{
	double decrement = 0;	

	// get set (current_set\item)
	if (!sset.contains(item))
		error("Current set does not contain the element\n");
		
	// get the set of the corresponding tokens
	HashSet subset_tokens;
	Set::const_iterator it (sset);
	for (it = sset.begin(); it != sset.end(); ++it)
	{
		if(*it!=item){
			Set::iterator it2(neighbors[*it]);
			for (it2 = neighbors[*it].begin(); it2 != neighbors[*it].end(); ++it2)
				subset_tokens.insert(*it2);
		}
	}	
	// get decrement
	Set::iterator it3(neighbors[item]);
	for (it3 = neighbors[item].begin(); it3 != neighbors[item].end(); ++it3)
	{
		if (!subset_tokens.contains(*it3))
		{
			decrement+=weight[*it3];
		}			
	}
	return decrement;
}


double BipartiteNeighborhoodFunctions::evalGainsaddFast(const Set& sset, int item) const
{
	double increment = 0;	
	Set::iterator it(neighbors[item]);
	for (it = neighbors[item].begin(); it != neighbors[item].end(); ++it)
	{
		// if the token is newly observed, add to increment		
		if (preCompute[*it] == 0)
		{
			increment += weight[*it];			
		}			
	}	
		
	return increment;
}

double BipartiteNeighborhoodFunctions::evalGainsremoveFast(const Set& sset, int item) const
{
	double decrement = 0;	
	Set::iterator it (neighbors[item]);
	for (it = neighbors[item].begin(); it != neighbors[item].end(); ++it)
	{
		// when the token is removed, if the quantity becomes 0
		// then add to decrement
		if (preCompute[*it] == 1)
		{
			decrement += weight[*it];			
		}			
	}		
	return decrement;
}



void BipartiteNeighborhoodFunctions::updateStatisticsAdd(const Set& sset, int item) const
{
	//unordered_set<int> new_token(neighbors[item]);
	// compute the increment
	Set::iterator it (neighbors[item]);
	for (it = neighbors[item].begin(); it != neighbors[item].end(); ++it)
	{
		if (preCompute[*it] == 0)
		{
			// preCompute.insert(*it);
			currentVal+=weight[*it];
		}
		preCompute[*it] += 1;
	}			
}

void BipartiteNeighborhoodFunctions::updateStatisticsRemove(const Set& sset, int item) const
{
	Set::iterator it (neighbors[item]);
	for (it = neighbors[item].begin(); it != neighbors[item].end(); ++it)
	{
		if (preCompute[*it] == 1)
		{
			currentVal-=weight[*it];
		}
		if (preCompute[*it] > 0) preCompute[*it] -= 1;
	}
}
void BipartiteNeighborhoodFunctions::clearpreCompute() const
{
	for ( int i = 0; i < numNeighbors; i++)
	{
		preCompute[i] = 0;
	}
	currentVal = 0;
	// preCompute.clear(); 
	
}

void BipartiteNeighborhoodFunctions::setpreCompute(const Set& sset) const
{
	clearpreCompute();
	Set::const_iterator it (sset);
	for (it = sset.begin();	it != sset.end(); ++it)
	{
		//unordered_set<int> cur_token(neighbors[*it]);
		Set::iterator it2 (neighbors[*it]);
		for (it2 = neighbors[*it].begin(); it2 != neighbors[*it].end(); ++it2)
		{
			if (preCompute[*it2] == 0)
			{
				currentVal+=weight[*it2];
			}
			preCompute[*it2] += 1;	
		}		
	}
}	

HashSet BipartiteNeighborhoodFunctions::getVocabulary(const Set& sset) const{
	setpreCompute(sset);
	HashSet vocabIDs;
	vocabIDs.clear();
	for(int i = 0; i < preCompute.size(); i++){
		if(preCompute[i] > 0){
			vocabIDs.insert(i);
		}
	}
	return vocabIDs;
}
}
