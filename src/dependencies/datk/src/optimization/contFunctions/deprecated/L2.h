/*
	CMTK: Convex Optimization And Machine Learning ToolKit
 *	Least Squares Loss with L2 regularization
	Author: Rishabh Iyer
	Melodi Lab, University of Washington, Seattle
 *
 */

#ifndef L2_H
#define L2_H

#include "../datarep/Vector.h"
#include "../datarep/Matrix.h"
#include "../datarep/VectorOperations.h"
#include "ContinuousFunctions.h"
namespace cmtk {

	class L2: public ContinuousFunctions{
	public:
		L2(int m);
        L2(const L2& l2); // copy constructor
		
		~L2();
		
		double eval(const Vector& x) const; // functionEval
		Vector evalGradient(const Vector& x) const; // gradientEval
		void eval(const Vector& x, double& f, Vector& gradient) const; // combined function and gradient eval
	};
	
}
#endif
