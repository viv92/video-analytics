/*
 *	Header file for defining a modular function: f(X) = m(X) + c = \sum_{i \in X} m_i + c 
	Author: Rishabh Iyer
	Melodi Lab, University of Washington, Seattle
 *
 */
#ifndef MODULAR_FUNCTIONS_H
#define MODULAR_FUNCTIONS_H
#include <vector>
#include <algorithm>
#include "../../utils/error.h"
#include "../../utils/totalOrder.h"

#include "SetFunctions.h"
#include "SubmodularFunctions.h"
#include "../../representation/Set.h"

namespace datk {
	class ModularFunctions: public SubmodularFunctions{
		public:
		const std::vector<double> weight; // m_i's
		const double offset; // c
		mutable double preCompute; // stores m(X) 
		// Functions
		ModularFunctions(const std::vector<double> weightin, const double offsetin);
		ModularFunctions(const ModularFunctions &m);
		~ModularFunctions();

        ModularFunctions* clone() const;    
        
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

