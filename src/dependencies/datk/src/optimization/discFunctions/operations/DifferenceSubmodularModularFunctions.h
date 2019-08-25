/*
	Copyright (C) Rishabh Iyer, 2015

 *	Combining a submodular function f and a modular function m: h(X) = \lambda_1 f(X) + \lambda_2 m(X). This function is submodular as long as \lambda_1 >= 0.
	Melodi Lab, University of Washington, Seattle
 *
 */
#ifndef DIFFERENCESUBMODMOD
#define DIFFERENCESUBMODMOD

#include "SetFunctions.h"
#include "SubmodularFunctions.h"
#include "ModularFunctions.h"
#include "../../representation/Set.h"

namespace datk {
	class DifferenceSubmodularModularFunctions: public SubmodularFunctions{
	protected:
		const SubmodularFunctions* f;
		const ModularFunctions* m;
	public:
		// Functions
		DifferenceSubmodularModularFunctions(const SubmodularFunctions& f, const ModularFunctions& m);
		DifferenceSubmodularModularFunctions(const DifferenceSubmodularModularFunctions& d);
        DifferenceSubmodularModularFunctions * clone() const;
        
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
	};
}
#endif
