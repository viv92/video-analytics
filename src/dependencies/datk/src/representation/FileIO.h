/*
 *	Utility code for file handling of various functions
	Author: Rishabh Iyer.
	Melodi Lab, University of Washington, Seattle
 *
 */

#ifndef CMTK_FILE_IO
#define CMTK_FILE_IO

#include "Vector.h"
#include "SparseFeature.h"
#include "DenseFeature.h"
#include <string>
namespace datk {

long GetFileSize(const char * filename);

Vector readVector(char* File, int n);

Vector readVectorBinary(char* File, int n);
// A specific implementation to read a binary matrix stored as floats.
std::vector<std::vector<float> > readKernelfromFileFloat(char* graphFile, int n);
// A specific implementation to read a binary matrix stored as doubles.
std::vector<std::vector<float> > readKernelfromFileDouble(char* graphFile, int n);
int line2words(char *s, struct SparseFeature & Feature, int& maxID);
std::vector<struct SparseFeature> readFeatureVectorSparse(char* featureFile, int& n, int &numFeatures);
std::vector<struct DenseFeature> readFeatureVectorsDense(std::string featureVectorPath, int&n, int& numDeatures);
void readFeatureLabelsLibSVM( const char* fname, std::vector<struct SparseFeature>& features, Vector& y, int& n, int &numFeatures);
}

#endif
