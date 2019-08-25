/*
	Copyright (C) 2015 Rishabh Iyer
 *	Header file for defining the facility location function: f(X) = \sum_{i \in V} \max_{j \in X} s_{ij}.
	Assumes that the kernel S is symmetric.
	Author: Rishabh Iyer.
	Email: rkiyer@uw.edu
	Melodi Lab, University of Washington, Seattle
 *
 */

#ifndef SET_COVER
#define SET_COVER
#include <vector>
#include "../../representation/Set.h"
#include "SetFunctions.h"
#include "SubmodularFunctions.h"

namespace datk {
class SetCover: public SubmodularFunctions{
	protected:
		const std::vector<Set> & coverSet; // The sets U_i, i \in V
		mutable Set preCompute; // Precomputed statistics \cup_{i \in X} U_i
	public:
	// Functions
		SetCover(int n, const std::vector<Set> & coverSet);
		SetCover(const SetCover & f);
        ~SetCover();
        SetCover* clone() const;
		double eval(const Set& sset) const;
		double evalFast(const Set& sset) const;
		double evalGainsadd(const Set& sset, int item) const;
		double evalGainsremove(const Set& sset, int item) const;
		double evalGainsaddFast(const Set& sset, int item) const;
		double evalGainsremoveFast(const Set& sset, int item) const;
		void updateStatisticsAdd(const Set& sset, int item) const;
		void updateStatisticsRemove(const Set& sset, int item) const;
		void setpreCompute(const Set& ssett) const;
		void clearpreCompute() const;
};
}
#endif
