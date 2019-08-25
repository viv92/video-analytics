/*
 *	Abstract base class for submodular functions
	Author: Rishabh Iyer
	Melodi Lab, University of Washington, Seattle
 *
*/

#include <iostream>
#include "../src/datk.h"
using namespace datk;
using namespace std;

int main(int argc, char** argv){
	char* featureFile = "../cmtkData/sidoF.txt";
	char* labelFile = "../cmtkData/sidoL.txt";
	int n; // number of data items
	int m; // numFeatures
	vector<struct SparseFeature> features = readFeatureVectorSparse(featureFile, n, m);
	Vector y = readVector(labelFile, n);
	cout<<y.size()<<"\n";
	// LeastSquaresLoss l(m, features, y);
	// L1 r(m);
	// SumContinuousFunctions ll(l, r, 1);
	L1LeastSquaresLoss<SparseFeature> ll(m, features, y, 1);
	
	Vector x(m, 1);
	double f;
	Vector g;
	cout<<"*******************************************************************\n";
	cout<<"Testing Gradient Descent with Least Squares Loss, press enter to continue...\n";
	#ifndef DEBUG
	cin.get();
	#endif
	gd(ll, Vector(m, 0), 1e-8, 250);
	// cout<<"*******************************************************************\n"
	// cout<<"Testing Gradient Descent with Least Squares Loss\n";	
	// gradientDescent(ss, Vector(m, 0), 1e-8, 250);

	cout<<"*******************************************************************\n";
	cout<<"Testing Gradient Descent with Line Search for L1-Least Squares Loss, press enter to continue...\n";
	#ifndef DEBUG
	cin.get();
	#endif
	gdLineSearch(ll, Vector(m, 0), 1, 1e-4, 250);
	// gradientDescentLS(ss, Vector(m, 0), 1, 1e-4, 250);

	cout<<"*******************************************************************\n";
	cout<<"Testing Gradient Descent with Barzilia-Borwein Step Length for L1-Least Squares Loss, press enter to continue...\n";
	#ifndef DEBUG
	cin.get();
	#endif
	gdBarzilaiBorwein(ll, Vector(m, 0), 1, 1e-4, 250);
	// gradientDescentBB(ss, Vector(m, 0), 1, 1e-4, 250);

	cout<<"*******************************************************************\n";
	cout<<"Testing Nesterov's Method for L1-Least Squares Loss, press enter to continue...\n";
	#ifndef DEBUG
	cin.get();
	#endif
	gdNesterov(ll, Vector(m, 0), 1, 1e-4, 250);

	cout<<"*******************************************************************\n";
	cout<<"Testing Conjugate Gradient for L1-Least Squares Loss, press enter to continue...\n";
	#ifndef DEBUG
	cin.get();
	#endif
	cg(ll, Vector(m, 0), 1, 1e-4, 250);

	cout<<"*******************************************************************\n";
	cout<<"L-BFGS for L1-Least Squares Loss, press enter to continue...\n";
	#ifndef DEBUG
	cin.get();
	#endif
	lbfgsMin(ll, Vector(m, 0), 1, 1e-4, 250);
	// lbfgsMin(ss, Vector(m, 0), 1, 1e-4, 250);

	cout<<"*******************************************************************\n";
	cout<<"L-BFGS-OWL for L1-Least Squares Loss, press enter to continue...\n";
	#ifndef DEBUG
	cin.get();
	#endif
	lbfgsMinOwl(ll, Vector(m, 0), 1, 1e-4, 250);
	// lbfgsMin(ss, Vector(m, 0), 1, 1e-4, 250);

	cout<<"*******************************************************************\n";
	cout<<"Stochastic Gradient Descent for L1-Least Squares Loss, press enter to continue...\n";
	#ifndef DEBUG
	cin.get();
	#endif
	sgd(ll, Vector(m, 0), n, 1e-8, 100, 1e-4, 250);

	cout<<"*******************************************************************\n";
	cout<<"Stochastic Gradient Descent with Decaying Learning Rate for L1-Least Squares Loss, press enter to continue...\n";
	#ifndef DEBUG
	cin.get();
	#endif
	sgdDecayingLearningRate(ll, Vector(m, 0), n, 0.5*1e-6, 200, 1e-4, 250, 0.6);

	cout<<"*******************************************************************\n";
	cout<<"Stochastic Gradient Descent with AdaGrad for L1-Least Squares Loss, press enter to continue...\n";
	#ifndef DEBUG
	cin.get();
	#endif
	sgdAdagrad(ll, Vector(m, 0), n, 1e-2, 200, 1e-4, 250);
}
