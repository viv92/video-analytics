/*
    A simple utils library. Implementations of min, max ...
*/
	
#ifndef DATK_UTILS_H
#define DATK_UTILS_H

namespace datk{
	inline double min(double a, double b)
	{
	  if (a < b){
	    return a;
	  }
	  else{
	    return b;
	  }
	}

	inline double max(double a, double b)
	{
	  if (a > b){
	    return a;
	  }
	  else{
	    return b;
	  }
	}
	
	inline int sign(double x){
		return (0 < x) - (x < 0);
	}
}

#endif
