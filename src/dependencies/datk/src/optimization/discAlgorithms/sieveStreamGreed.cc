/*
	Copyright (C) Kai Wei 2015
 *	The algorithm implemented here is generalized version of the sieve streaming greedy algorithm!
    Randomly order the whole data set, and then access the items in a sequential manner, we also extend to the minibatch streaming setting. 
	In each iteration, when a minibatch of size m comes in, we choose a set of d items using the greedy algorithm.  
	Solves the problem \max_{|X| \leq B} f(X), where f is a submodular function.
	Anthor: Kai Wei (kaiwei@uw.edu)
TODO: 
(1) implement the copy constructor for each submodular function;
(2) implement the destructor for each submodular function to free the memory. 
(3) implement the minibatch scenario
 *
 */

#include <queue>
#include <iostream>
#include "stdio.h"
#include "stdlib.h"
#include <algorithm>
#include <random>
#include "assert.h"
using namespace std;

#include "../../utils/error.h"
#include "../../utils/totalOrder.h"
#include "increment.h"
#include "sieveStreamGreed.h"
namespace datk {
int myrandomSieveGreed (int i) { return std::rand()%i;}

// input: submodular function f; number of items to select k; output solution GreedySet (need to be initilized to be emptyset); number of blocks m (when m = n, the algorithm is exactly KDD); tradeoff between memory, running time and accuracy; 
void sieveStreamGreedyMiniBatch(SubmodularFunctions& f, double k, Set& GreedySet, int m, int verbosity, double eps){

    if (eps < 0.01){
        cout << "Parameter epsilon cannot be set to be < 0.01, " << " and we set it to 0.01" << endl;
        eps = 0.01;
    }
    if (verbosity > 0)
        cout << "Number of blocks is " << m << endl;
    int nV = f.size();
    int nSelected = 0; 
    //int m = ceil(double(nV)/k*d);
    int block_size = ceil(double(nV) / m);
    double maxSingletonVal = 0;
    int lower_index = -1000;
    int upper_index = 9000; // we think 9000 is sufficiently large, since 1.01^9000 = 10^38. 
    int smallest_index = 0;
    int offset_index;
    double max_func_val = 0;
    int max_Index = lower_index; 
    Set dummy_set = Set();
    SubmodularFunctions** vec_functions = new SubmodularFunctions*[10000];
    Set* solution_sets = new Set[10000](); // list of sets to store the solution for each threshold. 
    double* func_val_vec = new double[10000](); // list of function values for each threshold;
    for (int idx=0;idx<10000;idx++){
        //vec_functions[idx] = SubmodularFunctions(f);
        vec_functions[idx] = NULL;
    }

    if (verbosity > 0)
        cout << "Number of items in each batch " << block_size << endl;

    // randomly shuffle the ground set
    srand(0);
    std::vector<int> randPermutedSet = std::vector<int>();
    for (int idx = 0; idx<nV;idx++){
        //cout<< "Randomly shuffle the ground set" << endl;
        randPermutedSet.push_back(idx);
    }
    if (verbosity > 0)
        cout<< "Randomly shuffle the ground set" << endl;
    
    std::random_shuffle ( randPermutedSet.begin(), randPermutedSet.end(), myrandomSieveGreed); // randomly shuffle the remaining set.
    int index = 0;
    int running_index = 0;

    for (int m_idx = 0; m_idx < m; m_idx++){

        if (verbosity >= 20){
            cout << "Now processing " << m_idx << "th block\n";
        }
        Set ConstrainedSet = Set(); 
        for (int idx = running_index; idx < running_index+block_size; idx++){
            if (idx >= nV){
                break;
            }
            ConstrainedSet.insert(randPermutedSet[idx]);
            Set SingletonSet = Set();
            SingletonSet.insert(randPermutedSet[idx]);
            double singletonGain = f.evalGainsaddFast(dummy_set,randPermutedSet[idx]); // we use this implementation to make sure fast implementation of the singleton gain. We need to make sure that the preCompute of f does not change in this algorithm.
            if (singletonGain < 0){
                cout << "Singleton value for item " << randPermutedSet[idx] << " is negative, please double check the input data!\n";
            }
            assert(singletonGain >= 0);
            if (singletonGain > maxSingletonVal){
                maxSingletonVal = singletonGain;// update the max singleton value. 
            }
            SingletonSet.clear();
        }


        if (ConstrainedSet.size() > 1){ // if in each iteration, there is more than one item. 
            //int item = randPermutedSet[running_index];
            int lower_index_new; 
            if (ceil(log(maxSingletonVal)) / log (1+eps) > ceil(log(max_func_val) / log (1+eps))){
                lower_index_new = ceil(log(maxSingletonVal) / log(1+eps)); // update the lower index threshold according to max singleton value 
            }
            else{
                lower_index_new = ceil(log (max_func_val) / log (1+eps)); // update the lower bound index to be tighter
            }
            if (verbosity >= 20){
                cout << "Lower index is " << lower_index_new << " and max singleton value is " << maxSingletonVal << endl;
            }
            int upper_index_new = floor(log(maxSingletonVal * 2 * k) / log(1+eps)); // update the upper bound index. 
            if (upper_index_new >= 10000){
                cout << "The range for the indices in the summarization is too large. The algorithm cannot handle, please try smaller value of k";
            }
            assert(upper_index_new < 10000);
            if (m_idx == 0){// in the first round, initialize the lower and upper indices for thresholds.             
                offset_index = lower_index_new;
                lower_index = lower_index_new;
                upper_index = upper_index_new;
            }
            for (int idx=lower_index; idx<lower_index_new; idx++){
                if (vec_functions[idx-offset_index] != NULL){
                    delete vec_functions[idx-offset_index]; // we need to implement the destructor function for each case of submodular functions. At this point, we are not able to delete the set.  
                    if (idx != max_Index){
                        solution_sets[idx-offset_index].clear();
                        if (verbosity >= 20)
                            cout << "Delete solution sets " << idx-offset_index << endl;
                    }
                    if (verbosity >= 20){
                        cout << idx << " has been deleted " <<endl;
                    }
                }
            }
            lower_index = lower_index_new;
            upper_index = upper_index_new;
            if (verbosity > 0){
                cout << "going to update the lists indexed from "<< lower_index<< " to "<<upper_index<< " with the smallest index so far being " << smallest_index <<endl;
            }

            int activate_gain_runs = 0;
            for (int idx=lower_index; idx <= upper_index; idx++){ // for the list of thresholds that are still active, we check their marginal gain and compare against the threshold. 
                if (vec_functions[idx-offset_index] == NULL){
                    //if (idx-offset_index < smallest_index){
                        //smallest_index = idx-offset_index;
                    //}
                    vec_functions[idx-offset_index] = f.clone(); // create a new function for this index
                }
                if (solution_sets[idx-offset_index].size() >= k){
                    if (verbosity >= 40){
                        cout << "budget for block index " << idx - offset_index << " has been filled up\n";
                    }
                    continue; // already filled up the budget, so ready to stop
                }

                lazyGreedyMaxConstrainedSieve(*vec_functions[idx-offset_index], k, solution_sets[idx-offset_index], ConstrainedSet, 0, pow(1+eps, idx)/2, func_val_vec[idx-offset_index]);

                if (func_val_vec[idx-offset_index] > max_func_val){
                    max_func_val = func_val_vec[idx-offset_index];
                    max_Index = idx;
                    if (verbosity >= 20){
                        cout << "max index and max function value have been updated to "<< max_Index << " and " << max_func_val << endl;
                    }
                }
            }
            if (verbosity >= 10){
                cout << "currently mainained best function value: " << func_val_vec[max_Index-offset_index] << " with cost being " << solution_sets[max_Index-offset_index].size() << " Computed " << activate_gain_runs << " number of gains "<< endl;
            }
        }


        if (ConstrainedSet.size() == 1){ // if in each iteration, there is only one item to be chosen. 
            int item = randPermutedSet[running_index];
            int lower_index_new; 
            if (ceil(log(maxSingletonVal)) / log (1+eps) > ceil(log(max_func_val) / log (1+eps))){
                lower_index_new = ceil(log(maxSingletonVal) / log(1+eps)); // update the lower index threshold according to max singleton value 
            }
            else{
                lower_index_new = ceil(log (max_func_val) / log (1+eps)); // update the lower bound index to be tighter
            }
            if (verbosity >= 20){
                cout << "Lower index is " << lower_index_new << " and max singleton value is " << maxSingletonVal << endl;
            }
            int upper_index_new = floor(log(maxSingletonVal * 2 * k) / log(1+eps)); // update the upper bound index. 
            if (upper_index_new >= 10000){
                cout << "The range for the indices in the summarization is too large. The algorithm cannot handle, please try smaller value of k";
            }
            assert(upper_index_new < 10000);
            if (m_idx == 0){// in the first round, initialize the lower and upper indices for thresholds.             
                offset_index = lower_index_new;
                lower_index = lower_index_new;
                upper_index = upper_index_new;
            }
            for (int idx=lower_index; idx<lower_index_new; idx++){
                if (vec_functions[idx-offset_index] != NULL){
                    delete vec_functions[idx-offset_index]; // we need to implement the destructor function for each case of submodular functions. At this point, we are not able to delete the set.  
                    if (idx != max_Index){
                        solution_sets[idx-offset_index].clear();
                        if (verbosity >= 20)
                            cout << "Delete solution sets " << idx-offset_index << endl;
                    }
                    if (verbosity >= 20){
                        cout << idx << " has been deleted " <<endl;
                    }
                }
            }
            lower_index = lower_index_new;
            upper_index = upper_index_new;
            if (verbosity > 0){
                cout << "going to update the lists indexed from "<< lower_index<< " to "<<upper_index<< " with the smallest index so far being " << smallest_index <<endl;
            }

            int activate_gain_runs = 0;
            for (int idx=lower_index; idx <= upper_index; idx++){ // for the list of thresholds that are still active, we check their marginal gain and compare against the threshold. 
                if (vec_functions[idx-offset_index] == NULL){
                    if (idx-offset_index < smallest_index)
                        smallest_index = idx-offset_index;
                    vec_functions[idx-offset_index] = f.clone(); // create a new function for this index
                }
                if (solution_sets[idx-offset_index].size() >= k){
                    if (verbosity >= 40){
                        cout << "budget for block index " << idx - offset_index << " has been filled up\n";
                    }
                    continue; // already filled up the budget, so ready to stop
                }
                double threshold = (pow(1+eps, idx)/2-func_val_vec[idx-offset_index])/(k - solution_sets[idx-offset_index].size());

                double gain = vec_functions[idx-offset_index]->evalGainsaddFast(dummy_set, item); // gain of this item.
                if (gain < 0){
                    cout << "The marginal gain of the submodular function is negative, the submodular function is not monotone. Please double check your input submodular function type or the data to instantiate the submodular function\n";
                    cout << "The SieveStreaming algorithm is designed only for monotone submodular functions.\n";
                }
                assert(gain>=0);
                activate_gain_runs++;
                double normalized_gain = gain;
                if (normalized_gain >= threshold){
                    if (solution_sets[idx-offset_index].size() + 1 > k){
                        if (func_val_vec[idx-offset_index] < max_func_val){
                            vec_functions[idx-offset_index]->~SubmodularFunctions(); // delete the function
                        }
                        continue;
                    }
                    vec_functions[idx-offset_index]->updateStatisticsAdd(dummy_set, item); // add this item into the selected dataset associated with this object, and update the precompute.
                    solution_sets[idx-offset_index].insert(item);
                    func_val_vec[idx-offset_index] += gain; 
                    if (func_val_vec[idx-offset_index] > max_func_val){
                        max_func_val = func_val_vec[idx-offset_index];
                        max_Index = idx;
                        if (verbosity >= 20){
                            cout << "max index and max function value have been updated to "<< max_Index << " and " << max_func_val << endl;
                        }
                    }
                }
            }
            if (verbosity >= 10){
                cout << "currently mainained best function value: " << max_func_val << " with cost being " << solution_sets[max_Index-offset_index].size() << " and of index " << max_Index - offset_index << " Computed " << activate_gain_runs << " number of gains "<< endl;
            }
        }

        running_index += block_size;
        ConstrainedSet.clear();
    }
    if (verbosity > 0){
        cout << "Output Solution set with index " << max_Index-offset_index << " of size " << solution_sets[max_Index-offset_index].size() << " into the output set\n";
    }
    for (Set::iterator it = solution_sets[max_Index-offset_index].begin(); it != solution_sets[max_Index-offset_index].end(); it++){
        GreedySet.insert(*it);
    }
    delete[] vec_functions;
    delete[] solution_sets; // list of sets to store the solution for each threshold. 
    delete[] func_val_vec; // list of function values for each threshold;

}


void lazyGreedyMaxConstrainedSieve(SubmodularFunctions& f, double budget, Set& selectedSet, Set& ConstrainedSet, int verbosity, double nu_over_2, double& func_val){
    int nSelected = selectedSet.size() ; // number of items selected
    double maxV;
    double preV = f.evalFast(selectedSet);
    //double currentCost = selectedSet.size();
    double currentCost = nSelected;
    // accelerated greedy algorithm implementation
    priority_queue <Increment> rho;
    // initilize the priority queue
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
            double threshold = (nu_over_2-func_val)/(budget - selectedSet.size());
            if (newV < threshold){
                break;
            }
            selectedSet.insert(topid);
            func_val += newV; 
            //RemainingSet.erase(topid);
            nSelected++;
            currentCost += 1;
            if(verbosity > 50) {
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
