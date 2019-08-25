/*
 *	Abstract base class for submodular functions
	Author: Rishabh Iyer
	Melodi Lab, University of Washington, Seattle
 *
*/

#include <iostream>
#include <cstdlib>
#include <string>
#include "../src/datk.h"
using namespace datk;
using namespace std;
char* trainFeatureFile = NULL;
char* trainLabelFile = NULL;
char* testFeatureFile = NULL;
char* testLabelFile = NULL;
int method = 5;
int nClasses = 2;
double lambda = 1;
char* outFile = NULL;
int maxIter = 1000;
int algtype = 0;
double tau = 1e-4;
double eps = 1e-2;
int verb = 0;
char* help = NULL;
bool startwith1 = false;
int sparse_dense = 0;
#define L1LR 1
#define L2LR 2
#define L1SSVM 3
#define L2SSVM 4
#define L2HSVM 5

Arg Arg::Args[]={
    Arg("trainFeatureFile", Arg::Req, trainFeatureFile, "the input training feature file",Arg::SINGLE),
    Arg("trainLabelFile", Arg::Req, trainLabelFile, "the input training label file",Arg::SINGLE),
    Arg("testFeatureFile", Arg::Req, testFeatureFile, "the input test feature file",Arg::SINGLE),
    Arg("testLabelFile", Arg::Req, testLabelFile, "the input test label file",Arg::SINGLE),
	  Arg("nClasses", Arg::Opt, nClasses, "The number of classes", Arg::SINGLE),
	  Arg("method", Arg::Opt, method, "Training method: 1(L1LR), 2(L2LR), 3(L1SSVM), 4(L2SSVM), 5(L2HSVM)", Arg::SINGLE),
	  Arg("reg", Arg::Opt, lambda, "Regularization parameter (default 1)", Arg::SINGLE),
	  Arg("maxIter", Arg::Opt, maxIter, "Maximum number of iterations (default 250)", Arg::SINGLE),
	  Arg("epsilon", Arg::Opt, eps, "epsilon for convergence (default: 1e-2)", Arg::SINGLE),
    Arg("algtype", Arg::Opt, algtype, "type of algorithm for training the corresponding method",Arg::SINGLE),
    Arg("model", Arg::Opt, outFile, "saving the training model",Arg::SINGLE),
    Arg("verb", Arg::Opt, verb, "verbosity",Arg::SINGLE),
    Arg("help", Arg::Help, help, "Print this message"),
    Arg("startwith1", Arg::Opt, startwith1, "Whether the Label file starts with one or zero"),
    Arg("sparse_dense", Arg::Opt, sparse_dense, "Whether the format is sparse/dense"),
    Arg()
};

string algs[] = {"L1 Logistic Regression", "L2 Logistic Regression", "L1 Smooth SVM",
"L2 Smooth SVM", "L2 Hinge SVM"};

template <class Feature>
double predictAccuracy(Classifiers<Feature>* c, vector<Feature>& testFeatures, Vector& ytest){
	assert(testFeatures.size() == ytest.size());
	double accuracy = 0;
	for (int i = 0; i < testFeatures.size(); i++){
		if (c->predict(testFeatures[i]) == ytest[i])
			accuracy++;
	}
	return accuracy;
}

int main(int argc, char** argv){
    bool parse_was_ok = Arg::parse(argc,(char**)argv);
    if(!parse_was_ok){
        Arg::usage(); exit(-1);
    }

	int ntrain; // number of data items in the training set
	int mtrain; // numFeatures of the training data
	int ntest; // number of data items in the test set
	int mtest; // numFeatures of the test data
  vector<struct SparseFeature> trainFeaturesS;
  vector<struct DenseFeature> trainFeaturesD;
  if (sparse_dense == 0)
  {
      trainFeaturesS = readFeatureVectorSparse(trainFeatureFile, ntrain, mtrain);
  }
  else
  {
      trainFeaturesD = readFeatureVectorsDense(trainFeatureFile, ntrain, mtrain);
  }
  Vector ytrain = readVector(trainLabelFile, ntrain);
  if ((startwith1) && (nClasses > 2))
  {
    ytrain = ytrain - 1;
  }
  else if ((startwith1) && (nClasses == 2))
  {
    ytrain = 2*ytrain-1;
  }
  vector<struct SparseFeature> testFeaturesS;
  vector<struct DenseFeature> testFeaturesD;
  Classifiers<SparseFeature>* cS;
  Classifiers<DenseFeature>* cD;
  if (sparse_dense == 0)
  {
	   testFeaturesS = readFeatureVectorSparse(testFeatureFile, ntest, mtest);
  }
  else
  {
     testFeaturesD = readFeatureVectorsDense(testFeatureFile, ntest, mtest);
  }
  Vector ytest = readVector(testLabelFile, ntest);
  if ((startwith1) && (nClasses > 2))
  {
    ytest = ytest - 1;
  }
  else if((startwith1) && (nClasses == 2))
  {
    ytest = 2*ytest-1;
  }
	cout << "Done reading the file, the size of the training set is " << ytrain.size() << " and the size of the test set is " <<ytest.size() << endl;
  cout << "The number of features is " << trainFeaturesD.size() << " and the size of y is " << ytrain.size() << "\n";
	if ((method < 0) || (method > 5)){
		cout << "Invalid method.\n";
		return -1;
	}
	cout << "Now training a " << algs[method-1] << " classifier.\n";
	double accuracy = 0;
	if (method == L1LR)
  {
    if (sparse_dense == 0)
    {
		    cS = new L1LogisticRegression<SparseFeature>(trainFeaturesS, ytrain, mtrain, ntrain, nClasses,
		        lambda, algtype, maxIter, eps);
        cS->train();
        cout << "Done with Training ... now testing\n";
        accuracy = predictAccuracy(cS, testFeaturesS, ytest);
    }
    else
    {
          cD = new L1LogisticRegression<DenseFeature>(trainFeaturesD, ytrain, mtrain, ntrain, nClasses,
            lambda, algtype, maxIter, eps);
          cD->train();
          cout << "Done with Training ... now testing\n";
          accuracy = predictAccuracy(cD, testFeaturesD, ytest);
	  }
  }
  else if (method == L2LR)
  {
    if (sparse_dense == 0)
    {
  		    cS = new L2LogisticRegression<SparseFeature>(trainFeaturesS, ytrain, mtrain, ntrain, nClasses,
  		        lambda, algtype, maxIter, eps);
          cS->train();
          cout << "Done with Training ... now testing\n";
          accuracy = predictAccuracy(cS, testFeaturesS, ytest);
    }
    else
    {
        cD = new L2LogisticRegression<DenseFeature>(trainFeaturesD, ytrain, mtrain, ntrain, nClasses,
          lambda, algtype, maxIter, eps);
        cD->train();
        cout << "Done with Training ... now testing\n";
				accuracy = predictAccuracy(cD, testFeaturesD, ytest);
    }
	}
	else if (method == L1SSVM)
  {
    if (sparse_dense == 0)
    {
		    cS = new L1SmoothSVM<SparseFeature>(trainFeaturesS, ytrain, mtrain, ntrain, nClasses,
		      lambda, algtype, maxIter, eps);
        cS->train();
        cout << "Done with Training ... now testing\n";
        accuracy = predictAccuracy(cS, testFeaturesS, ytest);
    }
    else
    {
        cD = new L1SmoothSVM<DenseFeature>(trainFeaturesD, ytrain, mtrain, ntrain, nClasses,
          lambda, algtype, maxIter, eps);
        cD->train();
        cout << "Done with Training ... now testing\n";
        accuracy = predictAccuracy(cD, testFeaturesD, ytest);
     }
	}
	else if (method == L2SSVM)
  {
    if (sparse_dense == 0)
    {
		    cS = new L2SmoothSVM<SparseFeature>(trainFeaturesS, ytrain, mtrain, ntrain, nClasses,
		        lambda, algtype, maxIter, eps);
        cS->train();
        cout << "Done with Training ... now testing\n";
        accuracy = predictAccuracy(cS, testFeaturesS, ytest);
    }
    else
    {
        cout << "This option is not yet implemented\n";
     }
	}
	else if (method == L2HSVM)
  {
    if (sparse_dense == 0)
    {
		    cS = new L2HingeSVM<SparseFeature>(trainFeaturesS, ytrain, mtrain, ntrain, nClasses,
		        lambda, algtype, maxIter, eps);
        cS->train();
        cout << "Done with Training ... now testing\n";
        accuracy = predictAccuracy(cS, testFeaturesS, ytest);
    }
    else
    {
        cout << "This option is not yet implemented\n";
    }
	}
	else
  {
		cout << "Invalid mode\n";
		return -1;
	}
	double accuracy_percentage = accuracy/ytest.size();
	cout << "The acuracy of the classifier is "<< accuracy_percentage << "("<< accuracy << "/"<< ytest.size()
		<< ")" << "\n";
}
