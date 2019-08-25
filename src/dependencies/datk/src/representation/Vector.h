/*
	CMTK: Convex Optimization And Machine Learning ToolKit
 *	A typedef defining vectors (since the vector class in STL is quite versatile, we just use it in this toolkit)
	Author: Rishabh Iyer
	Melodi Lab, University of Washington, Seattle
 *
 */

#ifndef VECTOR_H
#define VECTOR_H

#include <stdio.h>
#include <stdlib.h>	
#include <vector>

namespace datk{

typedef std::vector<double> Vector;
}
#endif
