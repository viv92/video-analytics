/*
	Copyright (C) 2015 Rishabh Iyer
 *	Header file for defining the facility location function: f(X) = \sum_{i \in V} \max_{j \in X} s_{ij}.
	Assumes that the kernel S is symmetric.
	Author: Rishabh Iyer.
	Email: rkiyer@uw.edu
	Melodi Lab, University of Washington, Seattle
 *
 */

#ifndef PROBABILISTIC_SET_COVER
#define PROBABILISTIC_SET_COVER
#include <vector>
#include "../../representation/Set.h"
#include "SetFunctions.h"
#include "SubmodularFunctions.h"

namespace datk {
class ProbabilisticSetCover: public SubmodularFunctions{
	protected:
		const std::vector<std::vector<double> > & p; // The sets U_i, i \in V
		mutable std::vector<double> preCompute; // Precomputed statistics \cup_{i \in X} U_i
        int nConcepts;
	public:
	// Functions
		ProbabilisticSetCover(int n, int nConcepts, const std::vector<std::vector<double> >& p);
		ProbabilisticSetCover(const ProbabilisticSetCover & f);
        ~ProbabilisticSetCover();
        ProbabilisticSetCover* clone() const;
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
