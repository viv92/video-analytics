/*
 *	Abstract base class for submodular functions
	Author: Rishabh Iyer
	Melodi Lab, University of Washington, Seattle
 *
*/

#include<iostream>
using namespace std;

#include "../../representation/Set.h"
#include "../../utils/error.h"
#include "SubmodularFunctions.h"

namespace datk {
    SubmodularFunctions::SubmodularFunctions(): SetFunctions(0){}
	SubmodularFunctions::SubmodularFunctions(int n): SetFunctions(n){}
	SubmodularFunctions::SubmodularFunctions(const SubmodularFunctions& f) : SetFunctions(f) {}

    SubmodularFunctions::~SubmodularFunctions(){}
	
    void SubmodularFunctions::resetData(vector<int> Rset){}
	
	void SubmodularFunctions::getSubgradient(totalOrder<double> ordering, const Set& sset, vector<double>& subgradient) const
	{
		//Check if the input ordering is consistent with the input sset.
		for(int i = 0; i < sset.size(); i++){ 
			if(!sset.contains(ordering[i])){
				cerr<<"Error: Inputed ordering inconsistent with the inputed set\n";
				return;
			}
		}
		subgradient.resize(n);
		Set currset;
        setpreCompute(currset);
		for(int i = 0; i < n; i++) {
			subgradient[ordering[i]] = evalGainsaddFast(currset, ordering[i]);
			updateStatisticsAdd(currset, ordering[i]);	
			currset.insert(ordering[i]);
		}
	}

	void SubmodularFunctions::modularUpperBoundA(const Set& sset, std::vector<double>& mupper, double& moffset) const{//std::vector<double> lastgains,
		setpreCompute(sset);
		mupper.resize(n);
		moffset = evalFast(sset);
		Set groundSet(n, true); // GroundSet constructor for BitSet
		for(int i = 0; i < n; i++ ) {
			if(!sset.contains(i)){// i is not in sset
				mupper[i] = evalGainsaddFast(sset, i);
			}
		}
		setpreCompute(groundSet);
		Set::const_iterator it;
		for( it = sset.begin(); it != sset.end(); ++it ) {
			mupper[*it] = evalGainsremoveFast(groundSet, *it);
			moffset-= mupper[*it];	
		}
	}

	void SubmodularFunctions::modularUpperBoundB(const Set& sset, std::vector<double>& mupper, double& moffset) const
	{
		moffset = 0;
		mupper.resize(n);
		Set emptyset;
        setpreCompute(emptyset);
		for(int i = 0; i < n; i++) {
			if(!sset.contains(i)){// i is not in sset
				mupper[i] = evalGainsaddFast(emptyset, i);
			}
		}
		setpreCompute(sset);
		moffset = evalFast(sset);
		Set::const_iterator it;
		for(it = sset.begin(); it != sset.end(); ++it ) {
			mupper[*it] = evalGainsremoveFast(sset, *it);
			moffset-= mupper[*it];
		}
	}
	
	void SubmodularFunctions::modularUpperBoundC(const Set& sset, std::vector<double>& mupper, double& moffset) const
	{
		moffset = 0;
		mupper.resize(n);
		Set emptyset;
        setpreCompute(emptyset);
		for(int i = 0; i < n; i++) {
			if(!sset.contains(i)){// i is not in sset
				mupper[i] = evalGainsaddFast(emptyset, i);
			}
		}
		Set groundSet(n, true); // GroundSet constructor for Set
		setpreCompute(groundSet);
		Set::const_iterator it;
		for( it = sset.begin(); it != sset.end(); ++it ) {
			mupper[*it] = evalGainsremoveFast(groundSet, *it);
			moffset-= mupper[*it];	
		}
		moffset+=eval(sset);
	}
	
	void SubmodularFunctions::getExtremePoint(totalOrder<double> ordering, vector<double>& extremePt) const{
		/* 
			Get an extreme point corresponding to the base polytope, corresponding to the input ordering.
		*/
		extremePt.resize(n);
		Set currset;
		clearpreCompute();
		for(int i = 0; i < n; i++) {
			extremePt[ordering[i]] = evalGainsaddFast(currset, ordering[i]);
			currset.insert(ordering[i]);
			updateStatisticsAdd(currset, ordering[i]);	
		}
	}
	
	double SubmodularFunctions::LovaszExtension(std::vector<double>& x, std::vector<double>& extremePt) const
	{
		/* 
			Get an extreme point corresponding to the base polytope, corresponding to the input ordering.
		*/
		totalOrder<double> ordering(x, false);
		extremePt.resize(n);
		Set currset;
        Set emptyset; 
        setpreCompute(emptyset);
		double lovaszVal = eval(emptyset);
		for(int i = 0; i < n; i++) {
            if ((x[i]>1) || (x[i]<0)){
                error("Error input for evaluating Lovasz Extension: please make sure the input vector is within range [0,1]\n");
            }
			extremePt[ordering[i]] = evalGainsaddFast(currset, ordering[i]);
			currset.insert(ordering[i]);
			updateStatisticsAdd(currset, ordering[i]);	
			lovaszVal += x[ordering[i]]*extremePt[ordering[i]];
		}
		return lovaszVal;
	}
}
