/*
 *	Concave over monotone submodular functions h(X) = psi(f(X)), for a submodular functions f.
	Melodi Lab, University of Washington, Seattle
 *
 */
#ifndef CONCAVE_SF
#define CONCAVE_SF

#include <vector>
#include "SetFunctions.h"
#include "SubmodularFunctions.h"
#include "../../representation/Set.h"

namespace datk {
class ConcaveSubmodularFunctions: public SubmodularFunctions{
	protected:
	const SubmodularFunctions* f;
	mutable double preCompute; // preCompute f(X), for a given set X.
	const int type; // type of the concave function
	const double thresh;
	const double exponent;
    double concaveFunction(double K) const;
	public:
	// Functions
	ConcaveSubmodularFunctions(const SubmodularFunctions& f, const int type, const double thresh = 1.0, const double exponent = 0.5);
	ConcaveSubmodularFunctions(const ConcaveSubmodularFunctions& c);
	~ConcaveSubmodularFunctions();
	
	ConcaveSubmodularFunctions* clone() const;
	
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
