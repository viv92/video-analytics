/*
 *	The algorithm implemented here is generalized version of lazier than lazy greedy algorithm!
	In each iteration, it samples a random subset R of size m, and run greedy to choose d items and add into the selected set, and keep until the budget is filled. 
	Solves the problem \max_{|X| \leq B} f(X), where f is a submodular function.
	Anthor: Kai Wei (kaiwei@uw.edu)
	Based on an implementation by Hui Lin. 
 *
 */

#include <queue>
#include <iostream>
#include "stdio.h"
#include "stdlib.h"
#include <algorithm>
#include <random>
using namespace std;

#include "../../utils/error.h"
#include "lazyGreedyMax.h"
#include "lazierThanLazy.h"
#include "increment.h"

namespace datk {

int myrandom (int i) { return std::rand()%i;}


void lazierThanLazyAlgOutofCore(SubmodularFunctions& f, double k, Set& GreedySet, int m, int d, int verbosity){

    if (( k/f.size() > 0.3) || ( double(m) / f.size() > 0.3)){
        cout << "Warning: Current implementation works efficiently only when summary size k and memory for each iteration m are relatively small to ground set n!" << endl;
        cout << "Warning: please consider choosing k and m such that k/n and m/n to be small, otherwise, the code might be inefficient!" << endl;
    }
    if (verbosity > 0){
        cout << "Number of iterations needed is " << ceil(k/d) << endl;
    }
    int nV = f.size();
    int nSelected = 0; 
    srand(0);
    Set RemainingSet = Set();
    for (int idx=0; idx<nV; idx++){
        RemainingSet.insert(idx);
    }
    f.setpreCompute(GreedySet);
    while (nSelected < k){
        if (verbosity > 0)
            cout << "Number of items selected " << nSelected << endl;
        int budget = d;
        if (nSelected + d > k){
            budget = k - nSelected;
        }
        // randomly sample a subset of size m from the remaining set
        //std::random_shuffle ( RemainingSet.begin(), RemainingSet.end(), myrandom); // randomly shuffle the remaining set.
        Set randomSet = Set();
        std::vector<int> vector_Rset = std::vector<int>();
        Set::iterator it;
        while (randomSet.size() < m){
            int randel = rand() % nV;
            if ((RemainingSet.contains(randel)) && (!randomSet.contains(randel))){
                randomSet.insert(randel);
                vector_Rset.push_back(randel);
                //cout << randel << " ";
            }
        }
        
        f.resetData(vector_Rset);
        //cout << endl;
        // Run the greedy algorithm over subset R to choose d items
        if (budget <= 1){
            double gain;
            double maxgain = 0;
            int max_item;
			Set::iterator it;
            for (it = randomSet.begin(); it!=randomSet.end(); ++it){ 
                gain = f.evalGainsaddFast(GreedySet, *it);
                if (gain > maxgain){
                    max_item = *it;
                    maxgain = gain;
                }
            }
            f.updateStatisticsAdd(GreedySet, max_item);
            GreedySet.insert(max_item);
            RemainingSet.remove(max_item);
            if (verbosity > 0){
                printf("Iteration %d: Selecting %d th sample, max gain = %.10e\n", nSelected, max_item, maxgain);
            }
        }
        else{
            lazyGreedyMaxConstrainedSet(f, budget, GreedySet, randomSet, verbosity, RemainingSet);
        }
        if (verbosity > 0){
            cout << "Selected set size: " << GreedySet.size() << endl;
        }
        nSelected += d;


    }
    if (verbosity > 0){
        cout << "Selected set size: " << GreedySet.size() << endl;
    }

}

void lazierThanLazyAlg(SubmodularFunctions& f, double k, Set& GreedySet, int m, int d, int verbosity){

    if (( k/f.size() > 0.3) || ( double(m) / f.size() > 0.3)){
        cout << "Warning: Current implementation works efficiently only when summary size k and memory for each iteration m are relatively small to ground set n!" << endl;
        cout << "Warning: please consider choosing k and m such that k/n and m/n to be small, otherwise, the code might be inefficient!" << endl;
    }
    if (verbosity > 0){
        cout << "Number of iterations needed is " << ceil(k/d) << endl;
    }
    int nV = f.size();
    int nSelected = 0; 
    srand(0);
    Set RemainingSet = Set();
    for (int idx=0; idx<nV; idx++){
        RemainingSet.insert(idx);
    }
    f.setpreCompute(GreedySet);
    while (nSelected < k){
        if (verbosity > 0)
            cout << "Number of items selected " << nSelected << endl;
        int budget = d;
        if (nSelected + d > k){
            budget = k - nSelected;
        }
        // randomly sample a subset of size m from the remaining set
        //std::random_shuffle ( RemainingSet.begin(), RemainingSet.end(), myrandom); // randomly shuffle the remaining set.
        Set randomSet = Set();
        while (randomSet.size() < m){
            int randel = rand() % nV;
            if ((RemainingSet.contains(randel)) && (!randomSet.contains(randel))){
                randomSet.insert(randel);
                //cout << randel << " ";
            }
        }
        //cout << endl;
        // Run the greedy algorithm over subset R to choose d items
        if (budget <= 1){
            double gain;
            double maxgain = 0;
            int max_item;
			Set::iterator it;
            for (it = randomSet.begin(); it!=randomSet.end(); ++it){ 
                gain = f.evalGainsaddFast(GreedySet, *it);
                if (gain > maxgain){
                    max_item = *it;
                    maxgain = gain;
                }
            }
            f.updateStatisticsAdd(GreedySet, max_item);
            GreedySet.insert(max_item);
            RemainingSet.remove(max_item);
            if (verbosity > 0){
                printf("Iteration %d: Selecting %d th sample, max gain = %.10e\n", nSelected, max_item, maxgain);
            }
        }
        else{
            lazyGreedyMaxConstrainedSet(f, budget, GreedySet, randomSet, verbosity, RemainingSet);
        }
        if (verbosity > 0){
            cout << "Selected set size: " << GreedySet.size() << endl;
        }
        nSelected += d;


    }
    if (verbosity > 0){
        cout << "Selected set size: " << GreedySet.size() << endl;
    }

}

void lazyGreedyMaxConstrainedSet(SubmodularFunctions& f, double budget, Set& selectedSet, Set& ConstrainedSet, int verbosity, Set& RemainingSet){
	int nSelected = selectedSet.size(); // number of items selected
    double maxV;
    double preV = f.evalFast(selectedSet);
	//double currentCost = selectedSet.size;
	double currentCost = 0;
	// accelerated greedy algorithm implementation
	priority_queue <Increment> rho;
	// initilize the priority queue
	Set::iterator it;
	for (it = ConstrainedSet.begin(); it!=ConstrainedSet.end(); ++it) { 
		rho.push(Increment(f.evalGainsaddFast(selectedSet, *it),*it)); 
	}
	int sort_cnt = 0;
	while (! rho.empty()) {
		int topid = rho.top().get_index();
		rho.pop();
		maxV = preV + f.evalGainsaddFast(selectedSet, topid);
		double newV = (maxV - preV); // return normalized gain 
		if (verbosity >= 5) printf("max gain = %.6e, rho->top() = %.6e\n",newV,rho.top().get_value());
		if (newV < rho.top().get_value()) {
			rho.push(Increment(newV, topid)); // if condition not satisfied, push back and re-sort
			sort_cnt++;
			if (verbosity >= 10) printf("Condition not met, re-sorting queue (%d iterations)\n",sort_cnt);
		}
        else {
            // guaranteed to be optimal because of submodularity
            selectedSet.insert(topid);
            RemainingSet.remove(topid);
            nSelected++;
            currentCost += 1;
            if(verbosity > 0) {
                printf("Iteration %d: Selecting %d th sample, curCost/budget = %.6e/%.6e, preV = %.10e, curV = %.10e, number of resorting operations=%d\n", nSelected, topid, currentCost, budget, preV, maxV, sort_cnt);
            }
            preV = maxV;
            f.updateStatisticsAdd(selectedSet, topid);

            //printf("Debug eval fast=%.10e\n", f.evalFast(selectedSet));
            sort_cnt = 0;
            if ((currentCost >= budget) || (newV < 0))
                break;
        }    
	}	
}

}
