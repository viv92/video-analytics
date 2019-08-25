/*
 *	A helper function, which takes in a submodular function f and a set of sets A_1, A_2, ... A_k and computes the function h(X) = \sum_{i =1}^k f(X \cap A_i).
	Melodi Lab, University of Washington, Seattle
 *
 */
#include "OperationsSubmodularFunctions.h"
#include "IntersectSubmodularFunctions.h"
using namespace std;

namespace smtk {
	CombineSubmodularFunctions ClusteredSubmodularFunctions(const SubmodularFunctions& f, const vector<HashSet>& clusters){
		vector<SubmodularFunctions*> fvec = vector<SubmodularFunctions*>();
		vector<double> lambda(clusters.size(), 1);
		for (int i = 0; i < clusters.size(); i++){
			IntersectSubmodularFunctions* fIsect = new IntersectSubmodularFunctions(f, clusters[i]);
			fvec.push_back(fIsect);
		}
		CombineSubmodularFunctions fComb(f.size(), fvec, lambda);
		for (int i = 0; i < fvec.size(); i++)
			delete fvec[i];
		return fComb;
	}
	
	ComplementSubmodularFunctions operator! (const SubmodularFunctions& f){ // unary complement operator
		ComplementSubmodularFunctions c(f);
		return c;
	}
	
	AddSubmodularFunctions operator+ (const SubmodularFunctions& f, const SubmodularFunctions& g){ // binary + operator to add submdular functions
		AddSubmodularFunctions a(f, g);
		return a;
	}
	
	AddSubmodularFunctions operator+ (const SubmodularFunctions& f, const double lambda){ // binary + operator to add a submdular function with a constant.
		vector<double> weight(f.size(), 0);
		ModularFunctions m(weight, lambda);
		AddSubmodularFunctions a(f, m);
		return a;
	}
	
	AddSubmodularFunctions operator+ (const SubmodularFunctions& f, const std::vector<double> weight){ // binary + operator to add a submdular function and a vector
		ModularFunctions m(weight, 0);
		AddSubmodularFunctions a(f, m);
		return a;
	}
	
	ScaleSubmodularFunctions operator* (const SubmodularFunctions& f, const double lambda){ // binary * operator to scale submdular functions
		ScaleSubmodularFunctions s(f, lambda);
		return s;
	}
	
	ScaleSubmodularFunctions operator* (const double lambda, const SubmodularFunctions& f){ // binary * operator to scale submdular functions
		ScaleSubmodularFunctions s(f, lambda);
		return s;
	}
	
	DifferenceSubmodularFunctions operator- (const SubmodularFunctions& f, const SubmodularFunctions& g){ // binary - operator for a difference of submodular functions.
		DifferenceSubmodularFunctions d(f, g);
		return d;
	}
	
	DifferenceSubmodularFunctions operator- (const SubmodularFunctions& f){ // unary - operator to define -f
		vector<double> weight(f.size(), 0);
		ModularFunctions m(weight, 0);
		DifferenceSubmodularFunctions d(m, f);
		return d;
	}
	
	DifferenceSubmodularModularFunctions operator- (const SubmodularFunctions& f, const ModularFunctions& m){
		DifferenceSubmodularModularFunctions d(f, m);
		return d;
	}
		
	ConcaveSubmodularFunctions sqrt(const SubmodularFunctions& f){
		ConcaveSubmodularFunctions c(f, 1);
		return c;
	}
	
	ConcaveSubmodularFunctions log(const SubmodularFunctions& f){
		ConcaveSubmodularFunctions c(f, 3);
		return c;
	}
	
	ConcaveSubmodularFunctions min(const SubmodularFunctions& f, double thresh){
		ConcaveSubmodularFunctions c(f, 4, thresh);
		return c;
	}
	
	ConcaveSubmodularFunctions pow(const SubmodularFunctions& f, double exponent){
		ConcaveSubmodularFunctions c(f, 5, 0, exponent);
		return c;
	}
	
}

