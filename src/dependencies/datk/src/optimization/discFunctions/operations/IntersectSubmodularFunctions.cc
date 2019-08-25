/*
 *	Truncate a submodular functions h(X) = f(X \cap A), a submodular functions f, and a set A \subseteq V.
	Melodi Lab, University of Washington, Seattle
 *
 */
#include <vector>
#include <iostream>
#include "../utils/error.h"
#include "../utils/totalOrder.h"
using namespace std;

#include "IntersectSubmodularFunctions.h"
#include "../sets/SetOperations.h"
namespace smtk {

IntersectSubmodularFunctions::IntersectSubmodularFunctions(const SubmodularFunctions& in_f, const HashSet& A): SubmodularFunctions(in_f), A(A) {
	f = in_f.clone();
}


IntersectSubmodularFunctions::IntersectSubmodularFunctions(const IntersectSubmodularFunctions& If): SubmodularFunctions(If), A(If.A), preCompute(If.preCompute) {
	f = If.f->clone();
}

IntersectSubmodularFunctions::~IntersectSubmodularFunctions(){
    delete f; 
}

IntersectSubmodularFunctions* IntersectSubmodularFunctions::clone() const{
    return new IntersectSubmodularFunctions(*this);
}

double IntersectSubmodularFunctions::eval(const Set& sset) const{
	HashSet isset;
	setInterSection(A, sset, isset);
	return f->eval(isset);
}

double IntersectSubmodularFunctions::evalFast(const Set& sset) const{
	return f->evalFast(preCompute);
}

double IntersectSubmodularFunctions::evalGainsadd(const Set& sset, int item) const{
	if (!A.contains(item))
		return 0;
	else{
		HashSet isset;
		setInterSection(A, sset, isset);
		return f->evalGainsadd(isset, item);
	}
}

double IntersectSubmodularFunctions::evalGainsremove(const Set& sset, int item) const{
	if (!A.contains(item))
		return 0;
	else{
		HashSet isset;
		setInterSection(A, sset, isset);
		return f->evalGainsremove(isset, item);
	}
}

double IntersectSubmodularFunctions::evalGainsaddFast(const Set& sset, int item) const{
	if (!A.contains(item))
		return 0;
	else{	
		return f->evalGainsaddFast(preCompute, item);  
	}
}

double IntersectSubmodularFunctions::evalGainsremoveFast(const Set& sset, int item) const{
	if (!A.contains(item))
		return 0;
	else{
		return f->evalGainsremoveFast(preCompute, item);
	}
}

void IntersectSubmodularFunctions::updateStatisticsAdd(const Set& sset, int item) const{
	if (!A.contains(item))
		return;
	else{
		f->updateStatisticsAdd(preCompute, item);
		preCompute.insert(item);
	}
}

void IntersectSubmodularFunctions::updateStatisticsRemove(const Set& sset, int item) const{
	if (!A.contains(item))
		return;
	else{
		f->updateStatisticsRemove(preCompute, item);
		preCompute.remove(item);
	}
}

void IntersectSubmodularFunctions::clearpreCompute() const{
	f->clearpreCompute();
	preCompute.clear();
}

void IntersectSubmodularFunctions::setpreCompute(const Set& sset) const{
	preCompute.clear();
	setInterSection(A, sset, preCompute);
	f->setpreCompute(preCompute);
}
}

