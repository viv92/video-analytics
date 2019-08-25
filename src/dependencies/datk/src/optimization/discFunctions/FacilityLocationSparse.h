/*
	Copyright (C) Rishabh Iyer 2015
 *	Header file for defining sparse version of facility location functions (f(X) = \sum_{i\in V} \max_{j\in S} w_{i,j}. In this case, the similarity graph is not complete. Instead, we only input a KNN graph, where each vertex is only connected to its K-nearest neighbors.
Author: Kai Wei (email: kaiwei@uw.edu)
	Melodi Lab, University of Washington, Seattle
 *
 */
#ifndef FACILITYLOCATIONSPARSE_SF
#define FACILITYLOCATIONSPARSE_SF

#include "../../representation/Set.h"
#include "SetFunctions.h"
#include "SubmodularFunctions.h"
#include "../../representation/SparseGraph.h"

namespace datk {
	class FacilityLocationSparse: public SubmodularFunctions{
		protected:
		//int nV; // ground set size |V|
		const std::vector<struct SparseGraphItem>& graph; // structure of the graph items from KNN graph. Size = n (groundset size)
		mutable std::vector<double> preCompute; // Precomputed statistics. For a set X, p_X(j) = \max_{i\in X} w_{i,j}. 
		mutable Set preComputeSet; // This points to the preComputed Set for which the statistics p_X is calculated.
		const int sizepreCompute;// size of the precompute statistics (in this case, |V|).
	
		public:
		// Functions
       	FacilityLocationSparse(int n, std::vector<struct SparseGraphItem>& graph);
        FacilityLocationSparse(const FacilityLocationSparse& f);

        FacilityLocationSparse* clone() const;
        ~FacilityLocationSparse();
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

