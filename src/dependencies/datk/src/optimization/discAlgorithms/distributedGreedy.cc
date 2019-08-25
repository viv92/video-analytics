/*
	Copyright (C) Kai Wei 2015
 *	The algorithm implemented here is the DiGreed algorithm first proposed by Mirzasoleiman et al. NIPS 2013.
    Randomly order the whole data set, and then partition the data into m blocks, and then run the lazy greedy algorithm on each block to choose d items. In the last step, aggregate the solutions from each block and run a last step greedy. 
	Solves the problem \max_{|X| \leq B} f(X), where f is a submodular function.
	Anthor: Kai Wei (kaiwei@uw.edu) 
 *
 */

#include <unordered_set>
#include <queue>
#include <iostream>
#include "stdio.h"
#include "stdlib.h"
#include <algorithm>
#include <random>
using namespace std;

#include "../../utils/error.h"
#include "lazyGreedyMax.h"
#include "distributedGreedy.h"
#include "increment.h"
namespace datk {

    int myrandomDiGreed (int i) { return std::rand()%i;}
// k: budget; m: number of blocks; d: number of items to choose in each block. 
	
    void distributedGreedySingleMachine(SubmodularFunctions& f, double k, int m, Set& GreedySet, int d, int verbosity){

        cout << "Number of blocks is " << m << endl;
        int nV = f.size();
        int nSelected = 0; 

        if (d < ceil(k/m)){
            cout << "The budget for each block d = " << d << " is set lower than " << ceil(k/m) << "; We set d to " << ceil(k/m) << " so that the algorithm can run!" << endl;
            d = ceil(k/m);
        }

        if (verbosity > 0){
            cout << " Randomly shuffle the ground set " << endl;
        }
        srand(0);
        std::vector<int> randPermutedSet = std::vector<int>();
        for (int idx = 0; idx<nV;idx++){
            randPermutedSet.push_back(idx);
        }
        std::random_shuffle ( randPermutedSet.begin(), randPermutedSet.end(), myrandomDiGreed); // randomly shuffle the ground set.
        Set R_set = Set(); // the set used as input to the second stage.
        Set Candidate_Output; 
        double Max_val = 0;
        int running_index = 0;
        int block_size = ceil(float(nV)/m);
        for (int m_idx = 0; m_idx < m; m_idx++){
            Set OutputSet = Set();
            Set ConstrainedSet = Set(); // used as input to the lazeGreed
            for (int idx = running_index; idx < running_index+block_size; idx++){
                if (idx >= nV){
                    break;
                }
                ConstrainedSet.insert(randPermutedSet[idx]);
            }
            running_index = running_index + block_size;
            if (verbosity > 0){
                cout << "Number of items in block " << m_idx << " is " << ConstrainedSet.size() << "\n";
            }
            lazyGreedyMaxConstrainedDiGreed(f, d, OutputSet, ConstrainedSet, 0);
            if (verbosity > 0){
                cout << "Block " << m_idx << " returns a set of size " << OutputSet.size() << " and of value: " << f.evalFast(OutputSet) << endl;
            }
            if (f.evalFast(OutputSet) > Max_val){
                Max_val = f.evalFast(OutputSet);
                Candidate_Output = OutputSet; // if the current output solution is bigger than other blocks, we keep track of this value and the set as well. 
            }
			Set::iterator it;
            for (it = OutputSet.begin(); it != OutputSet.end(); ++it){
                R_set.insert(*it); // add the selected items into the set used for the next stage. 
            }
            f.clearpreCompute(); // clear the statistics for this instance. 
        }
        // Run the last stage of Distributed Greedy algorithm
        Set OutputSet = Set();
        lazyGreedyMaxConstrainedDiGreed(f, k, OutputSet, R_set, 0);
        if (verbosity > 0){
            cout << "Second stage outputs a solution with size " << OutputSet.size() << " and of function value " << f.evalFast(OutputSet) << endl;
        }
        if (f.evalFast(OutputSet) > Max_val){
            Max_val = f.evalFast(OutputSet);
            Candidate_Output = OutputSet; 
        }
        GreedySet = Candidate_Output; 
    }

    void lazyGreedyMaxConstrainedDiGreed(SubmodularFunctions& f, double budget, Set& selectedSet, Set& ConstrainedSet, int verbosity){
        int nSelected = selectedSet.size() ; // number of items selected
        double maxV;
        double preV = f.evalFast(selectedSet);
        //double currentCost = selectedSet.size();
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
                //RemainingSet.erase(topid);
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
