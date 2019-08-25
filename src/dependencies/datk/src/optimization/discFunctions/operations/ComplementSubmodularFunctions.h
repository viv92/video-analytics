/*
 *	Complement submodular functions h(X) = f(V \ X), a submodular functions f.
	Melodi Lab, University of Washington, Seattle
 *
 */

#include "../../representation/Set.h"
#include "SetFunctions.h"
#include "SubmodularFunctions.h"

#ifndef COMPLEMENT_SF
#define COMPLEMENT_SF
namespace smtk {
	class ComplementSubmodularFunctions: public SubmodularFunctions{
		protected:
		const SubmodularFunctions* f;
		mutable Set preCompute; // preCompute V \ X, for a given set X.
		public:
		// Functions
		ComplementSubmodularFunctions(const SubmodularFunctions& input_f); // this is standard constructor 
        ComplementSubmodularFunctions(const ComplementSubmodularFunctions& input_f); // copy constructor
        ComplementSubmodularFunctions* clone() const; 
        ~ComplementSubmodularFunctions(); 
		Set findComplement(const Set& sset) const; // Return the complement of the set.
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
