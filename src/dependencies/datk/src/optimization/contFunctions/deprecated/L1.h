/*
  CMTK: Convex Optimization And Machine Learning ToolKit
  *	Least Squares Loss with L1 regularization
  Author: Rishabh Iyer
  Melodi Lab, University of Washington, Seattle
  *
  */

#ifndef L1_H
#define L1_H

#include "../datarep/Vector.h"
#include "../datarep/Matrix.h"
#include "../datarep/VectorOperations.h"
#include "ContinuousFunctions.h"
namespace cmtk {

  class L1: public ContinuousFunctions{
  public:
    L1(int m);
    L1(const L1& l2); // copy constructor
		
    ~L1();
		
    double eval(const Vector& x) const; // functionEval
    Vector evalGradient(const Vector& x) const; // gradientEval
    void eval(const Vector& x, double& f, Vector& gradient) const; // combined function and gradient eval
  };
	
}
#endif
