/*
	Copyright (C) Rishabh Iyer, 2015
 *	Header file for defining a disparity sum set function: f(X) = \min_{i, j \in X} (1 - s_{ij})
	Assumes that the kernel S is symmetric.
	Note: This function is *not* submodular!
	Author: Rishabh Iyer 
 *
 */
#ifndef MMR_SF
#define MMR_SF

namespace datk {
class MMR: public SetFunctions{
	public:
	std::vector<std::vector <float> >& kernel; // The matrix s_{ij}_{i \in V, j \in V} of similarity (the assumption is it is normalized between 0 and 1 -- max similarity is 1 and min similarity is 0)
	mutable double preComputeVal;
	// Functions
	MMR(int n, std::vector<std::vector <float> >& kernel);
    MMR(const MMR& f);
    MMR & operator=(const MMR & f);
    MMR * clone() const;
	~MMR();
	double eval(const Set& sset) const;
	double evalFast(const Set& sset) const;
	double evalGainsadd(const Set& sset, int item) const; 
	double evalGainsremove(const Set& sset, int item) const;
	double evalGainsaddFast(const Set& sset, int item) const; 
	double evalGainsremoveFast(const Set& sset, int item) const;
	void updateStatisticsAdd(const Set& sset, int item) const;
	void updateStatisticsRemove(const Set& sset, int item) const;
	void setpreCompute(const Set& sset) const;
	void clearpreCompute() const;
	
	void resetData(std::vector<int> Rset);
};
}
#endif
