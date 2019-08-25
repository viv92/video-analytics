/*
	CMTK: Convex Optimization And Machine Learning ToolKit
 *	Abstract base class for simple functions (whose prox operator can be efficiently found)
	Author: Rishabh Iyer
	Melodi Lab, University of Washington, Seattle
 *
 */

#ifndef SIMPLE_FUNCTIONS_H
#define SIMPLE_FUNCTIONS_H

#include "../datarep/Vector.h"
#include "../datarep/Matrix.h"
#include "../datarep/VectorOperations.h"
#include "ContinuousFunctions.h"
namespace cmtk {

	class SimpleFunctions: public ContinuousFunctions{
	protected:
		int m; // Dimension of vectors or features (i.e. size of x in f(x))
	public:
		SimpleFunctions(int m);
        SimpleFunctions(const SimpleFunctions& s); // copy constructor
		
		virtual ~SimpleFunctions();
		
		virtual double eval(const Vector& x) const; // functionEval
		virtual Vector evalProx(const Vector& x, double t) const; // a prox operator
	};
	
}
#endif
