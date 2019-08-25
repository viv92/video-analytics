/*

Copyright (C) Rishabh Iyer

 *	Gradient Descent for Unconstrained Convex Minimization with constant step size
	Solves the problem \min_x \phi(x), where \phi is a convex (or continuous) function.
	Anthor: Rishabh Iyer
	Melodi Lab, University of Washington, Seattle
 *
	Input: 	Continuous Function: c
		   	Initial starting point x0
			step-size parameter (alpha)
			max number of iterations (maxiter)
			Tolerance (TOL)
			Verbosity

	Output: Output on convergence (x)
 */

#include <stdio.h>
#include <algorithm>
#include <iostream>
using namespace std;

#include "gd.h"

namespace datk {

Vector gd(const ContinuousFunctions& c, const Vector& x0, const double alpha, 
const int maxEval, const double TOL, const int verbosity){
	cout<<"Started Gradient Descent\n";
	Vector x(x0);
	double f;
	Vector g;
	c.eval(x, f, g);
	double gnorm = norm(g);
	int funcEval = 1;
	while ((gnorm >= TOL) && (funcEval < maxEval) )
	{
		multiplyAccumulate(x, alpha, g);
		c.eval(x, f, g);
		funcEval++;
		gnorm = norm(g);
		if (verbosity > 0)
			printf("numIter: %d, alpha: %f, ObjVal: %f, OptCond: %f\n", funcEval, alpha, f, gnorm);
	}
	return x;
}		
}
