/*
 *	Header file for defining feature based functions (essentially sums of concave over modular functions): f(X) = \sum_{f \in F} w_f g(m_f(X)), where g is a concave function, {m_f}_{f \in F} are a set of feature scores, and f \in F are features.
	Author: Rishabh Iyer.
	Melodi Lab, University of Washington, Seattle
 *
 */

#ifndef SMTK_FEATUREBASED_TWOLAYER_DENSE_SF
#define SMTK_FEATUREBASED_TWOLAYER_DENSE_SF

#include <vector>
#include "../sets/Set.h"
#include "../sets/HashSet.h"
#include "SetFunctions.h"
#include "SubmodularFunctions.h"
#include "../datarep/DenseFeature.h"

namespace smtk {
	class FeatureBasedFunctionsTwoLayerDense: public SubmodularFunctions{
		protected:
		vector <int> nFeatures; // Number of Features |F| for corresponding |G|
		const vector<int> type; //type: type of concave function, 1: sqrt over modular, 2: inverse function, 3: Log function, 4: Whatever else you want, Default: sqrt over modular
		const vector<double> thresh;
        /* structure of the feature vectors for items. 2D as FeatureBased over Feature Based Func */
		std::vector< std::vector<struct DenseFeature> >& feats;  //Size = n (groundset size)
		std::vector< std::vector<double> >& featureWeights; // Feature Weights (w_f) and corresponding w_g
		
		mutable std::vector< std::vector<double> > preCompute; // Precomputed statistics. For a set X, p_X(f) = m_fg(X).
		mutable HashSet preComputeSet; // This points to the preComputed Set for which the statistics p_X is calculated.
		mutable vector <int> sizepreCompute;// size of the precompute statistics (in this case, |F|*|G|).
        double concaveFunction(double K, int concaveType) const;
		public:
		// Functions
        //FeatureBasedFunctionsTwoLayerDense(int n, vector <int> type, std::vector< std::vector<struct DenseFeature> >& feats, std::vector< std::vector<double> >& featureWeights, vector<double> thresh (2,0));
        
        FeatureBasedFunctionsTwoLayerDense(int n, vector <int> type, std::vector< std::vector<struct DenseFeature> >& feats, std::vector< std::vector<double> >& featureWeights, double thresh = 0);
        
        FeatureBasedFunctionsTwoLayerDense(const FeatureBasedFunctionsTwoLayerDense & f);
		~FeatureBasedFunctionsTwoLayerDense();
		
        FeatureBasedFunctionsTwoLayerDense* clone() const;
        
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

