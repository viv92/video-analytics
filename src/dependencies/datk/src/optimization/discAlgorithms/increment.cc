/*
	Copyright (C) Rishabh Iyer 2015
 *	A class containing the increments, used in lazy greedy algorithms
	Anthor: Rishabh Iyer
	Melodi Lab, University of Washington, Seattle
 *
 */
#include "stdio.h"
#include "increment.h"
namespace datk{
	Increment::Increment(){};

	Increment::Increment(double x, int i){ value=x; index=i; }

	int Increment::get_index() const { return index; }

	double Increment::get_value() const{ return value; }

	bool Increment::operator< (const Increment& right) const
	{
		return value < right.value;
	}
}
