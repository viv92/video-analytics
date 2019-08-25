#ifndef DENSE_FEATURE
#define DENSE_FEATURE

#include <vector>
namespace datk {

struct DenseFeature{ //Stores the feature vector for each item in the groundset
     long int index; // index of the item
     std::vector<double> featureVec; // score of the dense feature vector.
     int numFeatures;
};
// This is a dense feature representation. Each row has numFeatures number of items (i.e the size of featureVec should be equal to numFeatures)

}

#endif
