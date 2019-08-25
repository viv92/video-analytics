/*
 *	Header file for defining sparse version of facility location functions (f(X) = \sum_{i\in V} \max_{j\in S} w_{i,j}. In this case, the similarity graph is not complete. Instead, we only input a KNN graph, where each vertex is only connected to its K-nearest neighbors.
Author: Kai Wei (email: kaiwei@uw.edu)
	Melodi Lab, University of Washington, Seattle
 *
 */
#ifndef SMTK_FACILITYLOCATIONSPARSEDB_SF
#define SMTK_FACILITYLOCATIONSPARSEDB_SF

#include <vector>
#include "../sets/Set.h"
#include "../sets/HashSet.h"
#include "../datarep/SparseRepresentation.h"
#include "SetFunctions.h"
#include "SubmodularFunctions.h"

namespace smtk {

	class FacilityLocationSparseDB: public SubmodularFunctions{
		protected:
        SparseRepresentation& graph; // structure of the graph items from KNN graph, in this case, we simply use the SparseRepresentation. 
		mutable std::vector<double> preCompute; // Precomputed statistics. For a set X, p_X(j) = \max_{i\in X} w_{i,j}. 
		mutable HashSet preComputeSet; // This points to the preComputed Set for which the statistics p_X is calculated.
		const int sizepreCompute;// size of the precompute statistics (in this case, |V|).
	
		public:
		// Functions
        FacilityLocationSparseDB(int n, SparseRepresentation& graph);
        FacilityLocationSparseDB(const FacilityLocationSparseDB& f);

        void resetData(std::vector<int> Rset);
        FacilityLocationSparseDB* clone() const;
        ~FacilityLocationSparseDB();
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

