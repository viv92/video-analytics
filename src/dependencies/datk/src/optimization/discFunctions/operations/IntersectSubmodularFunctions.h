/*
 *	Intersection submodular functions h(X) = f(X \cap A), a submodular functions f, and a set A \subseteq V.
	Melodi Lab, University of Washington, Seattle
 *
 */
#ifndef INTERSECT_SF_H
#define INTERSECT_SF_H
#include "../sets/Set.h"
#include "../sets/HashSet.h"
#include "SetFunctions.h"
#include "SubmodularFunctions.h"

namespace smtk {
class IntersectSubmodularFunctions: public SubmodularFunctions{
	protected:
	const SubmodularFunctions* f;
	mutable HashSet preCompute; // preCompute A \cap X, for a given set X.
	const HashSet A;
	public:
	// Functions
	IntersectSubmodularFunctions(const SubmodularFunctions& in_f, const HashSet& A);
	IntersectSubmodularFunctions(const IntersectSubmodularFunctions& If);
    ~IntersectSubmodularFunctions();
    IntersectSubmodularFunctions* clone() const;
	
	double eval(const Set& sset) const;
	double evalFast(const Set& sset) const;
	double evalGainsadd(const Set& sset, int item) const;
	double evalGainsremove(const Set& sset, int item) const;
	double evalGainsaddFast(const Set& sset, int item) const;
	double evalGainsremoveFast(const Set& sset, int item) const;
	void updateStatisticsAdd(const Set& sset, int item) const;
	void updateStatisticsRemove(const Set& sset, int item) const;
	void setpreCompute(const Set& sset) const; 
	void clearpreCompute() const;
};
}
#endif

