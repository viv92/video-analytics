/*
 *	Abstract base class for simple functions (whose prox operator can be efficiently found)
	Author: Rishabh Iyer
	Melodi Lab, University of Washington, Seattle
 *
*/

#include<iostream>
using namespace std;

#include "SimpleFunctions.h"
#define EPSILON 1e-6
namespace cmtk {
	SimpleFunctions::SimpleFunctions(int m): m(m), ContinuousFunctions(false, m, 1){}
	SimpleFunctions::SimpleFunctions(const SimpleFunctions& s) : ContinuousFunctions(s){}

    SimpleFunctions::~SimpleFunctions(){}
	
	double SimpleFunctions::eval(const Vector& x) const{
		return 0;
	}
	
	Vector SimpleFunctions::evalProx(const Vector& x, double t) const{ // in case the function is non-differentiable, this is the subgradient
		Vector proxgrad(m, 0);
		return proxgrad;
	}
}
