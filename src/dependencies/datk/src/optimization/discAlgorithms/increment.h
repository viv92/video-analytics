/*
	Copyright (C) Rishabh Iyer 2015
 *	A class containing the increments, used in lazy greedy algorithms
	Anthor: Rishabh Iyer
	Melodi Lab, University of Washington, Seattle
 *
 */
#ifndef __INCREMENT__
#define __INCREMENT__
namespace datk{
class Increment{
	public: 
		Increment();
		Increment(double x, int i);
		bool operator< (const Increment& right) const;
		int get_index() const;
		double get_value() const;
	private:
		int index;
		double value;
};
}
#endif
