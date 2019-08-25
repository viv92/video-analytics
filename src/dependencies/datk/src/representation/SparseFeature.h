#ifndef SMTK_SPARSE_FEATURE
#define SMTK_SPARSE_FEATURE
#include <vector>
namespace datk {

	struct SparseFeature{ //Stores the feature vector for each item in the groundset
	     long int index; // index of the item
	     int numUniqueFeatures; // number of non-zero enteries in the feature vector
	     std::vector<int> featureIndex; // Indices which are non-zero (generally sparse)
	     std::vector<double> featureVec; // score of the features present.
	     int numFeatures;
	};
}

#endif
