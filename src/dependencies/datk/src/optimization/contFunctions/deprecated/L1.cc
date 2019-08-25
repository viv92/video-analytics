/*
	CMTK: Convex Optimization And Machine Learning ToolKit
 *	Least Squares Loss with L1 regularization
	Author: Rishabh Iyer
	Melodi Lab, University of Washington, Seattle
 *
*/

#include<iostream>
#include <math.h>
using namespace std;

#include "L1.h"
#include "../utils/utils.h"
#include "../datarep/VectorOperations.h"
#include <assert.h>
#include <cmath>
#define EPSILON 1e-6
#define MAX 1e2
namespace cmtk {	
	L1::L1(int m): ContinuousFunctions(true, m, 1){}
	
	L1::L1(const L1& l) : ContinuousFunctions(true, l.m, 1){}

    L1::~L1(){}
	
	double L1::eval(const Vector& x) const{
		assert(x.size() == m);
		return norm(x, 1);
	}
	
	Vector L1::evalGradient(const Vector& x) const{
		assert(x.size() == m);
		Vector g(m, 0);
		sign(x, g);
	}

	void L1::eval(const Vector& x, double& f, Vector& g) const{
		assert(x.size() == m);
		f = norm(x, 1);
		sign(x, g);
		return;
	}	
}
