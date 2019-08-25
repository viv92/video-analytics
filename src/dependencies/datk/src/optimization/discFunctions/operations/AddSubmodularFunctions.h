/*
    Copyright (C) Rishabh Iyer 2015

 *	Header file for adding two submodular functions f and g.
	Melodi Lab, University of Washington, Seattle
 *
 */
#ifndef ADD_SF
#define ADD_SF

#include <vector>
#include "SetFunctions.h"
#include "SubmodularFunctions.h"
#include "../../representation/Set.h"

namespace datk {
	class AddSubmodularFunctions: public SubmodularFunctions{
		protected:
			const SubmodularFunctions* f;
			const SubmodularFunctions* g;
		public:
		// Functions
			AddSubmodularFunctions(const SubmodularFunctions& f, const SubmodularFunctions& g);
            AddSubmodularFunctions(const AddSubmodularFunctions &c);
			AddSubmodularFunctions * clone() const;            
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

