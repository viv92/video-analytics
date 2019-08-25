/*
 *	Combining two submodular functions h(X) = \sum_{i = 1}^k \lambda_i f_i(X), for two submodular functions f and g.
	Melodi Lab, University of Washington, Seattle
 *
 */
#ifndef SMTK_SCALE_SF
#define SMTK_SCALE_SF

#include <vector>
#include <assert.h>     
#include "../utils/error.h"
#include "../utils/totalOrder.h"

#include "../sets/Set.h"
#include "SetFunctions.h"
#include "SubmodularFunctions.h"

namespace smtk {
class ScaleSubmodularFunctions: public SubmodularFunctions{
	protected:
	const SubmodularFunctions* f;
	const double lambda;
	public:
	// Functions
	ScaleSubmodularFunctions(const SubmodularFunctions& f, const double lambda);
	ScaleSubmodularFunctions(const ScaleSubmodularFunctions& s);
	~ScaleSubmodularFunctions();
	ScaleSubmodularFunctions* clone() const;
	
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

