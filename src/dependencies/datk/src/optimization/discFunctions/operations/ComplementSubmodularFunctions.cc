/*
 *	Complement submodular functions h(X) = f(V \ X), a submodular functions f.
	Melodi Lab, University of Washington, Seattle
 *
 */

// This function implementation is buggy: (it seems the bug has been fixed now.) 
// We need to make sure a few things: (1) preCompute is set correctly all the time: in the constructor, it should be set to be complement of the S. 
// In general this function is not normalized since f(\emptyset) = g(V). It seems that the subgradient works fine when the input function is symmetric, but fails when the input function is non-symmetric


#include <vector>
#include <algorithm>
#include <iostream>
using namespace std;

#include "ComplementSubmodularFunctions.h"
namespace datk {
	ComplementSubmodularFunctions::ComplementSubmodularFunctions(const SubmodularFunctions& input_f): SubmodularFunctions(input_f) {
        f = input_f.clone();
        //cout << "Calling Constructor " << endl;
        preCompute.clear();
        preCompute = HashSet(); // FIXED
		for (int i = 0; i < f->size(); i++)
			preCompute.insert(i);
    }
    ComplementSubmodularFunctions::ComplementSubmodularFunctions(const ComplementSubmodularFunctions& input_f): SubmodularFunctions(input_f) {
        preCompute = input_f.preCompute;
        f = input_f.f->clone();
    }
	
    ComplementSubmodularFunctions::~ComplementSubmodularFunctions(){
        delete f; 
    }
	
    ComplementSubmodularFunctions* ComplementSubmodularFunctions::clone() const{
        return new ComplementSubmodularFunctions(*this); 
    }

	Set ComplementSubmodularFunctions::findComplement(const Set& sset) const{
		Set csset = Set();
		for(int i = 0; i < n; i++){
			if(!sset.contains(i))
				csset.insert(i);
		}
		return csset;
	}
	
	double ComplementSubmodularFunctions::eval(const Set& sset) const{
		Set csset = findComplement(sset);
        //cout << "Calling eval function at set of size " << csset.size() << endl;
		return f->eval(csset);
	}

	double ComplementSubmodularFunctions::evalFast(const Set& sset) const{
		return f->evalFast(preCompute);
	}

	double ComplementSubmodularFunctions::evalGainsadd(const Set& sset, int item) const{
		Set csset = findComplement(sset);
		return -1*f->evalGainsremove(csset, item);
	}

	double ComplementSubmodularFunctions::evalGainsremove(const Set& sset, int item) const{
		Set csset = findComplement(sset);
		return -1*f->evalGainsadd(csset, item);
	}

	double ComplementSubmodularFunctions::evalGainsaddFast(const Set& sset, int item) const{
        //HashSet csset = findComplement(sset);
        //if (csset.size() != preCompute.size()){
        //    error("Something wrong\n");
        //}
            //cout << "item is not in the complement set" << endl;
        //}
        //cout << -1*f->evalGainsremoveFast(findComplement(sset), item) << endl;
        //if (item == 0){
            //cout << "Evaluate remove " << item << " from set of size " << findComplement(sset).size() << " of value " <<  -1*f->evalGainsremoveFast(findComplement(sset), item) << " " << -1*f->evalGainsremove(findComplement(sset), item) << endl;
        //}
		return -1*f->evalGainsremoveFast(preCompute, item);  
	}

	double ComplementSubmodularFunctions::evalGainsremoveFast(const Set& sset, int item) const{
		return -1*f->evalGainsaddFast(preCompute, item);
	} 

	void ComplementSubmodularFunctions::updateStatisticsAdd(const Set& sset, int item) const{
		f->updateStatisticsRemove(preCompute, item);
		preCompute.remove(item);
	}

	void ComplementSubmodularFunctions::updateStatisticsRemove(const Set& sset, int item) const{
		f->updateStatisticsAdd(preCompute, item);
		preCompute.insert(item);
	}

	void ComplementSubmodularFunctions::clearpreCompute() const{
		//f->clearpreCompute();
        preCompute.clear();
		for (int i = 0; i < f->size(); i++)
			preCompute.insert(i);
        f->setpreCompute(preCompute); // FIXED
		//preCompute.clear();
	}

	void ComplementSubmodularFunctions::setpreCompute(const Set& sset) const{
		preCompute = findComplement(sset);
		f->setpreCompute(preCompute);
	}
}
