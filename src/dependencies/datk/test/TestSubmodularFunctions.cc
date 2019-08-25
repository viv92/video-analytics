/*
 *	A tutorial for SOTK.
 *	Author: Rishabh Iyer
 *	Email: rkiyer@uw.edu
 *	Melodi Lab, University of Washington, Seattle
 *
 *	Here, we run medium scale experiments using the TIMIT corpus, for speech recognition. 
 *	The size of the dataset is n = 4620.
 *	Our algorithms are tested on variants of datasubset selection problem for speech recognition
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <sys/stat.h>
#include <sys/types.h>
#include <set>
#include <algorithm>

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "math.h"
#include "time.h"
#include <stdarg.h>
#include <assert.h>
#include <queue>
using namespace std;

#include "../src/datk.h"
using namespace datk;
	
//#define SMALLEST_NUMBER -1e70
//#define LARGEST_NUMBER 1e70

int main(int argc, char** argv){
	int n; // GroundSet size
	int numWords;	
	cout<<"***************************************************************************\n";
	cout<<"Running Small Scale Experiments with TIMIT\n";
	cout<<"***************************************************************************\n";
	char* similarityFileTIMIT = "../data/TIMITSimilarity";
	char* featureFileTIMIT = "../data/TIMITFeature";
	n = 4620;
	numWords = 48;
	time_t start, end;
	double timetaken;
    

	/*

			******************** Monotone Submodular Maximization *******************

	*/
	cout<<"\n";
	cout<<"First we do monotone submodular maximization using the Greedy Algorithms!\n";
	cout<<"\n";
	double budget;
	double frac = 0.1;
	/*
		Facility Location Function
	*/
	cout<<"***************************************************************************\n";
	cout<<"Let's try the facility Location Function: Choose "<<frac*100<<"% of the data\n";
	budget = frac*n;
	vector<vector<float> > kernel = readKernelfromFileFloat(similarityFileTIMIT, n);
	FacilityLocation fL(n, kernel);
	// Lazy Greedy
	Set greedySetFLa;
	start = time(0);
	lazyGreedyMax(fL, budget, greedySetFLa, 0);
	end = time(0);
	timetaken = difftime(end, start);
	cout<<"Maximum obtained with the Lazy Greedy with Facility Location is "<< fL.evalFast(greedySetFLa)<<" and the time taken is "<<timetaken<<" seconds.\n";

	// Naive Greedy
	Set greedySetFLn;
	start = time(0);
	naiveGreedyMax(fL, budget, greedySetFLn, 0);
	end = time(0);
	timetaken = difftime(end, start);
	cout<<"Maximum obtained with the Naive Greedy with Facility Location is "<< fL.evalFast(greedySetFLn)<<" and the time taken is "<<timetaken<<" seconds.\n";

	// Random Subset
	srand(time(NULL));
	Set randomSetfL;
	for (int tt = 0; tt < budget; tt++){
		int randel = rand() % n;
		randomSetfL.insert(randel);
	}
	cout<<"The value of a random set with Facility Location of the same size is "<< fL.eval(randomSetfL)<<"\n";
	cout<<"\n";
	/*
		Saturated Coverage Function
	*/
	cout<<"***************************************************************************\n";
	cout<<"Let's try the Saturated Coverage Function: Choose "<<frac*100<<"% of the data, and alpha = 0.1.\n";
	budget = frac*n;
	SaturateCoverage sC(n, kernel, 0.1);
	// Lazy Greedy
	Set greedySetsCa;
	start = time(0);
	lazyGreedyMax(sC, budget, greedySetsCa, 0);
	end = time(0);
	timetaken = difftime(end, start);
	cout<<"Maximum obtained with the Lazy Greedy with Saturated Coverage is "<< sC.evalFast(greedySetsCa)<<" with a set of size "<<greedySetsCa.size()<<" and the time taken is "<<timetaken<<" seconds.\n";

	// Naive Greedy
	Set greedySetsCn;
	start = time(0);
	naiveGreedyMax(sC, budget, greedySetsCn, 0);
	end = time(0);
	timetaken = difftime(end, start);
	cout<<"Maximum obtained with the Naive Greedy with Saturated Coverage is "<< sC.evalFast(greedySetsCn)<<" with a set of size "<<greedySetsCn.size()<<" and the time taken is "<<timetaken<<" seconds.\n";

	// Random Subset
	srand(time(NULL));
	Set randomSetsC;
	for (int tt = 0; tt < budget-1; tt++){
		int randel = rand() % n;
		randomSetsC.insert(randel);
	}
	cout<<"The value of a random set with Saturated Coverage of the same size is "<< sC.eval(randomSetsC)<<" with a set of size "<<randomSetsC.size()<<"\n";
	cout<<"\n";

	/*
		Feature Based Function
	*/
	cout<<"***************************************************************************\n";
	cout<<"Let's try the Feature Based Functions: Choose "<<frac*100<<"% of the data.\n";
	budget = frac*n;
	int numFeatures = 0;
	vector<struct SparseFeature> features = readFeatureVectorSparse(featureFileTIMIT, n, numFeatures);
	vector<double> featureWeights(numFeatures, 1);
	cout<<numFeatures<<"\n";
	FeatureBasedFunctions ff (n, 1, features, featureWeights); 
	// Lazy Greedy
	Set greedySetffa;
	start = time(0);
	lazyGreedyMax(ff, budget, greedySetffa, 0);
	end = time(0);
	timetaken = difftime(end, start);
	cout<<"Maximum obtained with the Lazy Greedy with Feature Based Functions is "<< ff.evalFast(greedySetffa)<<" and the time taken is "<<timetaken<<" seconds.\n";

	// Naive Greedy
	Set greedySetffn;
	start = time(0);
	naiveGreedyMax(ff, budget, greedySetffn, 0);
	end = time(0);
	timetaken = difftime(end, start);
	cout<<"Maximum obtained with the Naive Greedy with Feature Based Functions is "<< ff.evalFast(greedySetffn)<<" and the time taken is "<<timetaken<<" seconds.\n";

	// Random Subset
	srand(time(NULL));
	Set randomSetff;
	for (int tt = 0; tt < budget; tt++){
		int randel = rand() % n;
		randomSetff.insert(randel);
	}
	cout<<"The value of a random set with Feature Based Function of the same size is "<< ff.eval(randomSetff)<<"\n";
	cout<<"\n";	
	/*
		Combining all the above functions via Combine Submodular Functions
	
	cout<<"***************************************************************************\n";
	cout<<"Let's try combining all the above Functions: Choose "<<frac*100<<"% of the data.\n";
	budget = frac*n;
	int k = 3; // number of combining functions.
	vector<SubmodularFunctions*> fvec;
	BitSet groundSet(n, true);
	vector<double> lambdaVec(k, 1);
	ScaleSubmodularFunctions ffscaled(ff, 1/ff.eval(groundSet));
	fvec.push_back(&ffscaled);
	ScaleSubmodularFunctions sCscaled(sC, 1/sC.eval(groundSet));
	fvec.push_back(&sCscaled);
	ScaleSubmodularFunctions fLscaled(fL, 1/fL.eval(groundSet));
	fvec.push_back(&fLscaled);
	CombineSubmodularFunctions fComb(n, fvec, lambdaVec);
	// Lazy Greedy
	Set greedySetcomba;
	start = time(0);
	lazyGreedyMax(fComb, budget, greedySetcomba, 0);
	end = time(0);
	timetaken = difftime(end, start);
	cout<<"Maximum obtained with the Lazy Greedy with combination is "<< fComb.evalFast(greedySetcomba)<<" and the time taken is "<<timetaken<<" seconds.\n";

	// Naive Greedy
	Set greedySetcombn;
	start = time(0);
	naiveGreedyMax(fComb, budget, greedySetcombn, 0);
	end = time(0);
	timetaken = difftime(end, start);
	cout<<"Maximum obtained with the Naive Greedy with combination is "<< fComb.evalFast(greedySetcombn)<<" and the time taken is "<<timetaken<<" seconds.\n";

	// Random Subset
	srand(time(NULL));
	Set randomSetcomb;
	for (int tt = 0; tt < budget; tt++){
		int randel = rand() % n;
		randomSetcomb += randel;
	}
	cout<<"The value of a random set with Combined Function of the same size is "<< fComb.eval(randomSetcomb)<<"\n";
	cout<<"\n";
	
	/*
		Combining all the above functions via Operators on Submodular Functions
	
	cout<<"***************************************************************************\n";
	cout<<"Let's try combining all the above Functions via operators: Choose "<<frac*100<<"% of the data.\n";
	budget = frac*n;
	AddSubmodularFunctions fAdd = ff*(2/ff.eval(groundSet)) + sC*(2/sC.eval(groundSet)) + fL*(2/fL.eval(groundSet));
	// Lazy Greedy
	Set greedySetadda;
	start = time(0);
	lazyGreedyMax(fAdd, budget, greedySetadda, 0);
	end = time(0);
	timetaken = difftime(end, start);
	cout<<"Maximum obtained with the Lazy Greedy with combination is "<< fAdd.evalFast(greedySetadda)<<" and the time taken is "<<timetaken<<" seconds.\n";

	// Naive Greedy
	Set greedySetaddn;
	start = time(0);
	naiveGreedyMax(fAdd, budget, greedySetaddn, 0);
	end = time(0);
	timetaken = difftime(end, start);
	cout<<"Maximum obtained with the Naive Greedy with combination is "<< fAdd.evalFast(greedySetaddn)<<" and the time taken is "<<timetaken<<" seconds.\n";

	// Random Subset
	srand(time(NULL));
	Set randomSetadd;
	for (int tt = 0; tt < budget; tt++){
		int randel = rand() % n;
		randomSetadd += randel;
	}
	cout<<"The value of a random set with Combined Function of the same size is "<< fAdd.eval(randomSetadd)<<"\n";
	cout<<"\n";
	/*
			******************** Non-Monotone Submodular Maximization *******************
	
	cout<<"\n";
	cout<<"***************************************************************************\n";
	cout<<"Now we do non-monotone submodular maximization!\n";
	cout<<"\n";
	/*
			Asymmetric Graph Cut + Similarity Penalty 
	*/
	cout<<"***************************************************************************\n";
	double lambda = 4;
	cout<<"Let's try the graph cut function with similarity penalty. Set lambda = "<<lambda<<".\n";

	GraphCutFunctions gC (n, kernel, lambda);
	Set localSetgC;
	start = time(0);
	localSearchMax(gC, localSetgC, 0);
	end = time(0);
	timetaken = difftime(end, start);
	cout<<"Maximum obtained with the Local Search Algorithm with graphCut Penalty is "<< gC.eval(localSetgC)<<" and the time taken is "<<timetaken<<" seconds.\n";

	start = time(0);
	Set bidirGreedySet;
	totalOrder<int> initOrder(n);
	bidirGreedyMax(gC, bidirGreedySet, initOrder, 1, 0);
	end = time(0);
	timetaken = difftime(end, start);
	cout<<"Maximum obtained with the Deterministic Bi-directional Greedy Algorithm with graphCut Penalty is "<< gC.eval(bidirGreedySet)<<" and the time taken is "<<timetaken<<" seconds.\n";
	
	start = time(0);
	Set bidirGreedySetRand;
	bidirGreedyMax(gC, bidirGreedySetRand, initOrder, 1, 1);
	end = time(0);
	timetaken = difftime(end, start);
	cout<<"Maximum obtained with the Randomized Bi-directional Greedy Algorithm with graphCut Penalty is "<< gC.eval(bidirGreedySetRand)<<" and the time taken is "<<timetaken<<" seconds.\n";

	start = time(0);
	Set greedySetgC;
	lazyGreedyMax(gC, n, greedySetgC, 0);
	end = time(0);
	timetaken = difftime(end, start);
	cout<<"Maximum obtained with the Lazy Greedy Algorithm with graphCut Penalty is "<< gC.eval(greedySetgC)<<" and the time taken is "<<timetaken<<" seconds.\n";
	/*start = time(0);
	optSet = randomizedGreedyMax(gC, n/20, emptyset, 0);
	end = time(0);
	timetaken = difftime(end, start);
	cout<<"Maximum obtained with the Randomized Greedy Algorithm with graphCut Penalty with size constraint "<<optSet.size()<<" is "<< gC.eval(optSet)<<" and the time taken is "<<timetaken<<" seconds.\n";

	start = time(0);
	optSet = lazyGreedyMax(gC, n/20, emptyset, 0);
	end = time(0);
	timetaken = difftime(end, start);
	cout<<"Maximum obtained with the Lazy Greedy Algorithm with graphCut Penalty with size constraint "<<optSet.size()<<" is "<< gC.eval(optSet)<<" and the time taken is "<<timetaken<<" seconds.\n";
	*/
	srand(time(NULL));
	Set	randomSetgC;
	for (int i = 0; i < n; i++){
		if( (rand() % 2) == 0)
			randomSetgC.insert(i);
	}
	cout<<"The value of a random set with graphCut Penalty "<< gC.eval(randomSetgC)<<"\n";
	cout<<"\n";

	/*

			***************** Submodular Minimization: Minimum Norm Point Algorithm *******************
			Bipartite Neighborhood Functions - First try to minimize the complexity function minus a modular size penalty: min_X cc(X) - \lambda |X|
	
	cout<<"***************************************************************************\n";
	bipartiteNeighborhoodFunctions cc (groundSet, transcriptFileTIMIT, 48);
	vector<double> weights(n, 1);
	modularFunctions mm(groundSet, weights, 0);
	double scalepar = (cc.eval(groundSet))/(mm.eval(groundSet));
	cout<<"\n";
	cout<<"Now we try the minimum norm point algorithm: min_X cc(X) - \lambda |X|, where \lambda = "<<scalepar<<".\n";
	cout<<"\n";
	combineSubmodularModularFunctions ccm (&cc, &mm, 1, -scalepar);
	unordered_set<int> minSolSet;
	unordered_set<int> maxSolSet; 
	std::vector<double> xstar;
	start = time(0);
	minNormPoint(ccm, minSolSet, maxSolSet, xstar, 10);
	end = time(0);
	timetaken = difftime(end, start);
	cout<<"Minimum norm point value is "<< ccm.eval(minSolSet)<<" and the time taken is "<<timetaken<<" seconds.\n";
	cout<<"cc(X) "<< cc.eval(minSolSet)<<" |X| = "<<minSolSet.size()<<".\n";
	/*

			***************** Submodular Cost Submodular Knapsack (with modular coverage function): max |X|: cc(X) \leq B *******************
	
	cout<<"***************************************************************************\n";
	cout<<"\n";
	cout<<"Now we do SCSK: : max |X|: cc(X) \leq B, where B = "<<100<<".\n";
	cout<<"\n";
	start = time(0);
	std::unordered_set<int> mminscskSet = mminSCSK(cc, mm, 100);
	end = time(0);
	timetaken = difftime(end, start);
	cout<<"SCSK is cc(X) = "<< cc.eval(mminscskSet)<<" and mm(X) = " << mm.eval(mminscskSet)<<"\n";

	/*

			***************** Difference of Submodular Optimization: Submodular Supermodular Procedure *******************
			Complexity Functions - Minimize the complexity function minus the feature based diversity funtion: min_X cc(X) - \lambda ff(X)
	
	cout<<"***************************************************************************\n";
	scalepar = (cc.eval(groundSet))/(ff.eval(groundSet));
	cout<<"\n";
	cout<<"Now we try the submodular-supermodular procedures: min_X cc(X) - \lambda ff(X), where \lambda = "<<scalepar<<".\n";
	cout<<"\n";
	
	// Modular-Modular Procedure
	unordered_set<int> mminModModSet = mminModMod(cc, ff, scalepar, 1, 1);
	cout<<"ModMod is cc(X) = "<< cc.eval(mminModModSet)<<" and ff(X) = " << ff.eval(mminModModSet)<<"\n";
	
	// Supermodular-Submodular Procedure
	unordered_set<int> mminSupSubSet = mminSupSub(cc, ff, scalepar, 1, 0);
	cout<<"SupSub is cc(X) = "<< cc.eval(mminSupSubSet)<<" and ff(X) = " << ff.eval(mminSupSubSet)<<"\n";

	// Submodular-Supermodular Procedure
	unordered_set<int> mminSubSupSet = mminSubSup(cc, ff, scalepar, 1, 1);
	cout<<"SubSup is cc(X) = "<< cc.eval(mminSubSupSet)<<" and ff(X) = " << ff.eval(mminSubSupSet)<<"\n";

	/*

			***************** Submodular Cost Submodular Knapsack (with submodular coverage function): max ff(X): cc(X) \leq B *******************
	
	cout<<"***************************************************************************\n";
	cout<<"\n";
	cout<<"Now we do SCSK: : max ff(X): cc(X) \leq B, where B = "<<100<<".\n";
	cout<<"\n";
	start = time(0);
	mminscskSet = mminSCSK(cc, ff, 100);
	end = time(0);
	timetaken = difftime(end, start);
	cout<<"SCSK is cc(X) = "<< cc.eval(mminscskSet)<<" and ff(X) = " << ff.eval(mminscskSet)<<"\n";
	*/
}
