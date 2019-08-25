/*
	Copyright (C) Rishabh Iyer, 2015

 *	Combining two submodular functions h(X) = \lambda_1 f(X) + \lambda_2 g(X), for two submodular functions f and g.
	Melodi Lab, University of Washington, Seattle
 *
 */
#ifndef SMTK_DifferenceSF
#define SMTK_DifferenceSF

#include <vector>
#include <algorithm>
#include "SetFunctions.h"
#include "SubmodularFunctions.h"
#include "../sets/Set.h"

namespace datk {
class DifferenceSubmodularFunctions: public SetFunctions{
protected:
	const SubmodularFunctions* f;
	const SubmodularFunctions* g;
public:
	// Functions
	DifferenceSubmodularFunctions(const SubmodularFunctions& f, const SubmodularFunctions& g);
	DifferenceSubmodularFunctions(const DifferenceSubmodularFunctions &d);
	~DifferenceSubmodularFunctions();
    DifferenceSubmodularFunctions * clone() const;
    
    double eval(const Set& sset) const;
	double evalFast(const Set& sset) const;
	double evalGainsadd(const Set& sset, int item) const;
	double evalGainsremove(const Set& sset, int item) const;
	double evalGainsaddFast(const Set& sset, int item) const;
	double evalGainsremoveFast(const Set& sset, int item) const;
	void updateStatisticsAdd(const Set& sset, int item) const;
	void updateStatisticsRemove(const Set& sset, int item) const;
	void setpreCompute(const Set& sset) const; // Set preCompute for removing elements.
	void clearpreCompute() const;
	
	void resetData(std::vector<int> Rset);
};
}
#endif
