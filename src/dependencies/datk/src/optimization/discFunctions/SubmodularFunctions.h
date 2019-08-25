/*
 *	Abstract base class for submodular functions
	Author: Rishabh Iyer
	Melodi Lab, University of Washington, Seattle
 *
 */

#ifndef SUBMODULAR_FUNCTIONS_H
#define SUBMODULAR_FUNCTIONS_H
#include <vector>
#include "../../representation/Set.h"
#include "../../utils/totalOrder.h"
#include "SetFunctions.h"

namespace datk {

	class SubmodularFunctions: public SetFunctions{
		public:
            SubmodularFunctions(); 
			SubmodularFunctions(int n);
            SubmodularFunctions(const SubmodularFunctions& f);
			
            virtual ~SubmodularFunctions();

            //SubmodularFunctions & operator=(const SubmodularFunctions & f);

            virtual SubmodularFunctions * clone() const = 0;
            
            void resetData(std::vector<int> Rset); // functionality for out-of-core access of the data
            
            // Obtain a subgradient (modular lower bound) for a given set and ordering.
			void getSubgradient(totalOrder<double> ordering, const Set& set, std::vector<double>& subgradient) const;
			// Extreme point of the base polytope
			void getExtremePoint(totalOrder<double> ordering, std::vector<double>& extremePt) const;
			// Lovasz Extension
			double LovaszExtension(std::vector<double>& x, std::vector<double>& extremePt) const;
			// Obtain a supergradient (modular upper bound) A for a given set. 
			void modularUpperBoundA(const Set& set, std::vector<double>& mupper, double& moffset) const; 
			// Obtain a supergradient (modular upper bound) B for a given set. 
			void modularUpperBoundB(const Set& set, std::vector<double>& mupper, double& moffset) const; 
			// Obtain a supergradient (modular upper bound) C for a given set. 
			void modularUpperBoundC(const Set& set, std::vector<double>& mupper, double& moffset) const; 			
	};
	
}
#endif
