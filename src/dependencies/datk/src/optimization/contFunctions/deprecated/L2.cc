/*
	CMTK: Convex Optimization And Machine Learning ToolKit
 *	Least Squares Loss with L2 regularization
	Author: Rishabh Iyer
	Melodi Lab, University of Washington, Seattle
 *
*/

#include<iostream>
#include <math.h>
using namespace std;

#include "L2.h"
#include "../datarep/VectorOperations.h"
#include <assert.h>
#define EPSILON 1e-6
#define MAX 1e2
namespace cmtk {	
	L2::L2(int m): ContinuousFunctions(true, m, 1){}
	
	L2::L2(const L2& l) : ContinuousFunctions(true, l.m, 1){}

    L2::~L2(){}
	
	double L2::eval(const Vector& x) const{
		assert(x.size() == m);
		return x*x;
	}
	
	Vector L2::evalGradient(const Vector& x) const{
		assert(x.size() == m);
		return 2*x;
	}

	void L2::eval(const Vector& x, double& f, Vector& g) const{
		g = 2*x;
		f = x*x;
		return;
	}	
}
