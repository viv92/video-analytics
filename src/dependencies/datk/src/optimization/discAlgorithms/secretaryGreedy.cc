/*
	Copyright (C) Kai Wei 2015
 *	The algorithm implemented here is generalized version of algorithm!
    Randomly order the whole data set, and then access the items in a sequential manner, this models the minibatch streaming setting. 
	In each iteration, when a minibatch of size m comes in, we choose a set of d items using the greedy algorithm.  
	Solves the problem \max_{|X| \leq B} f(X), where f is a submodular function.
	Anthor: Kai Wei (kaiwei@uw.edu)
TODO: 
(1) implement scenario where the overlap between minibatches is allowed. Allow for an additional hyperparameter of the algorithm. 
 *
 */

#include <unordered_set>
#include <queue>
#include <iostream>
#include "stdio.h"
#include "stdlib.h"
#include <algorithm>
#include <random>
#include <time.h>
#ifdef _OPENMP_
#include<omp.h> 
#endif
using namespace std;

#include "../../utils/error.h"
#include "../../utils/totalOrder.h"
#include "lazyGreedyMax.h"
#include "secretaryGreedy.h"
#include "increment.h"
namespace datk {

    int myrandomSec (int i) { return std::rand()%i;}

    // Out of core implementation of the secretary Greedy algorithm
    void secretaryGreedyMiniBatchOutofCore(SubmodularFunctions& f, double k, Set& GreedySet, int d, int verbosity){

        if (verbosity > 0)
            cout << "Number of iterations needed is " << ceil(k/d) << endl;
        int nV = f.size();
        int nSelected = 0; 
        int m = floor(double(nV)/k*d);
        if (verbosity > 0)
            cout << "Items in each batch " << m << endl;

        // randomly shuffle the ground set
        srand(0);
        Set RemainingSet = Set();
        std::vector<int> randPermutedSet = std::vector<int>();
        for (int idx = 0; idx<nV;idx++){
            //cout<< "Randomly shuffle the ground set" << endl;
            randPermutedSet.push_back(idx);
            RemainingSet.insert(idx);
        }
        if (verbosity > 0)
            cout<< "Randomly shuffle the ground set" << endl;
        //std::random_shuffle ( randPermutedSet.begin(), randPermutedSet.end(), myrandomSec); // randomly shuffle the remaining set.
        int index = 0;
        while (nSelected < k){
            if (verbosity > 0)
                cout << "Number of items selected " << nSelected << endl;
            int budget = d;
            if (nSelected + d > k){
                budget = k - nSelected;
            }
            Set randomSet = Set();
            std::vector<int> vector_Rset = std::vector<int>();
            int jdx;
            for (jdx=index; jdx<index+m; jdx++){
                if (jdx == nV)
                    break;
                else{
                    randomSet.insert(randPermutedSet[jdx]);
                    vector_Rset.push_back(randPermutedSet[jdx]);
                }
            }
            index = jdx;

            const clock_t begin_time = clock();

            f.resetData(vector_Rset);
            if (verbosity > 0){
                std::cout << "Time taken for one loop Data: " << float( clock () - begin_time ) /  CLOCKS_PER_SEC << endl;
            }

            // Run the greedy algorithm over subset R to choose d items
            if (budget <= 1){
                double gain;
                double maxgain = 0;
                int max_item;
       
        //omp_set_num_threads(4);
		//#pragma omp parallel for
                for (int i = 0; i < vector_Rset.size(); i++){ 
			//#pragma omp critical
			{		
		            gain = f.evalGainsaddFast(GreedySet, vector_Rset[i]);
		            if (gain > maxgain){
		                max_item = vector_Rset[i];
		                maxgain = gain;
		            }
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
                lazyGreedyMaxConstrainedSetSecretary(f, budget, GreedySet, randomSet, verbosity, RemainingSet);
            }
            if (verbosity > 0){
                cout << "Selected set size: " << GreedySet.size() << endl;
            }
            nSelected += d;


        }

    }

    void secretaryGreedyMiniBatch(SubmodularFunctions& f, double k, Set& GreedySet, int d, int verbosity){

        if (verbosity > 0)
            cout << "Number of iterations needed is " << ceil(k/d) << endl;
        int nV = f.size();
        int nSelected = 0; 
        int m = floor(double(nV)/k*d);
        if (verbosity > 0)
            cout << "Items in each batch " << m << endl;

        // randomly shuffle the ground set
        srand(0);
        Set RemainingSet = Set();
        std::vector<int> randPermutedSet = std::vector<int>();
        for (int idx = 0; idx<nV;idx++){
            //cout<< "Randomly shuffle the ground set" << endl;
            randPermutedSet.push_back(idx);
            RemainingSet.insert(idx);
        }
        if (verbosity > 0)
            cout<< "Randomly shuffle the ground set" << endl;
        //std::random_shuffle ( randPermutedSet.begin(), randPermutedSet.end(), myrandomSec); // randomly shuffle the remaining set.
        int index = 0;
        while (nSelected < k){
            if (verbosity > 0)
                cout << "Number of items selected " << nSelected << endl;
            int budget = d;
            if (nSelected + d > k){
                budget = k - nSelected;
            }
            Set randomSet = Set();
            //std::vector<int> vector_Rset = std::vector<int>();
            int jdx;
            for (jdx=index; jdx<index+m; jdx++){
                if (jdx == nV)
                    break;
                else
                    randomSet.insert(randPermutedSet[jdx]);
            }
            index = jdx;
            
            const clock_t begin_time = clock();

            // Run the greedy algorithm over subset R to choose d items
            if (budget <= 1){
                double gain;
                double maxgain = 0;
                int max_item;
                for (Set::iterator it = randomSet.begin(); it!=randomSet.end(); it++){ 
                    gain = f.evalGainsaddFast(GreedySet, *it);
                    if (gain > maxgain){
                        max_item = *it;
                        maxgain = gain;
                    }
                }
                //cout << "Checkpoint" << nSelected << endl;
                f.updateStatisticsAdd(GreedySet, max_item);
                //cout << "AfterCheckpoint" << nSelected << endl;
                GreedySet.insert(max_item);
                RemainingSet.remove(max_item);
                if (verbosity > 0){
                    printf("Iteration %d: Selecting %d th sample, max gain = %.10e\n", nSelected, max_item, maxgain);
                }
            }
            else{
                lazyGreedyMaxConstrainedSetSecretary(f, budget, GreedySet, randomSet, verbosity, RemainingSet);
            }
            if (verbosity > 0){
                std::cout << "Time taken for one loop Data: " << float( clock () - begin_time ) /  CLOCKS_PER_SEC << endl;
            }
            if (verbosity > 0){
                cout << "Selected set size: " << GreedySet.size() << endl;
            }
            nSelected += d;


        }

    }


    void lazyGreedyMaxConstrainedSetSecretary(SubmodularFunctions& f, double budget, Set& selectedSet, Set& ConstrainedSet, int verbosity, Set& RemainingSet){
        int nSelected = selectedSet.size() ; // number of items selected
        double maxV;
        double preV = f.evalFast(selectedSet);
        //double currentCost = selectedSet.size();
        double currentCost = 0;
        // accelerated greedy algorithm implementation
        priority_queue <Increment> rho;
        // initilize the priority queue
		//#pragma omp parallel for
        for (Set::iterator it = ConstrainedSet.begin(); it!=ConstrainedSet.end(); it++) { 
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
