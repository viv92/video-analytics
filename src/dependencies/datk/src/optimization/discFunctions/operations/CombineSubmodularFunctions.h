/*
    Copyright (C) Rishabh Iyer 2015
 *	Header file for Combining two submodular functions h(X) = \sum_{i = 1}^k \lambda_i f_i(X), for two submodular functions f and g.
	Melodi Lab, University of Washington, Seattle
 *
 */
#ifndef COMBINE_SF
#define COMBINE_SF

#include <vector>
#include "SetFunctions.h"
#include "SubmodularFunctions.h"
#include "../../representation/Set.h"

namespace datk {
	class CombineSubmodularFunctions: public SubmodularFunctions{
		protected:
			std::vector<SubmodularFunctions*> fvec;
			std::vector<double> lambda;
		public:
		// Functions
			CombineSubmodularFunctions(int n, std::vector<SubmodularFunctions*> fvec, std::vector<double>& lambda);
            CombineSubmodularFunctions(const CombineSubmodularFunctions &c);
            ~CombineSubmodularFunctions();
			CombineSubmodularFunctions * clone() const;            
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

