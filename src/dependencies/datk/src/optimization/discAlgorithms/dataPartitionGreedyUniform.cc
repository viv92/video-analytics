/*
	Copyright (C) Kai Wei (2015)
 *	The algorithm implemented here is greedily partition of the ground set into m blocks!
	In each iteration, choose a block with the minimum function value, and assign an item with the maximum marginal gain to this block. 
	Solves the problem \max_{\pi(V) = \{A_1,\dots,A_m\}} \min f(A_i), where f is a submodular function.
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
#include "../../utils/totalOrder.h"
#include "../discFunctions/SetFunctions.h"
#include "../discFunctions/SubmodularFunctions.h"
#include "dataPartitionGreedyUniform.h"
#include "increment.h"

namespace datk {
    void dataPartitionGreedyUniform(SubmodularFunctions& f, vector<Set>& vecSets, int m_blocks, int verb){

        if (verb > 0)
            cout << "Number of blocks is " << m_blocks << endl;
        int nV = f.size();
        int nSelected = 0; 
        Set dummy_set = Set();
        vector<SubmodularFunctions*> vec_functions(m_blocks);
        vector<priority_queue <Increment> > rho_vec(m_blocks, priority_queue<Increment>());
        for (int idx=0; idx<m_blocks; idx++){
            vec_functions[idx] = f.clone();
        }
        if (verb > 0)
            printf("Start initializing the priority queue\n");
        for (int i=0;i<nV;i++){
            double singletonVal = vec_functions[0]->evalGainsaddFast(dummy_set, i);
            for (int j=0;j<m_blocks;j++){
                rho_vec[j].push(Increment(singletonVal, i));
            }
        }
        if (verb > 0)
            printf("Start running the greedy algorithm\n");
        // start greedy algorithm: find the one block with minimum value and find the item with the maximum     gain and add it to the block. 
        int min_block_index = 0;
        int sort_cnt = 0;
        double min_block_value = 0;
        vector<double> block_value_vec(m_blocks, 0);
        vector<bool> hashSelected(nV, false); // an index table to show whether an item has been allocated
        vector<double> preV_vec(m_blocks, 0);

        //int** SelectedSet = new int*[m_blocks]; // array to store the allocated items into each block
        vecSets = vector<Set>(m_blocks, Set());
        //int* Block_Cnt = new int[m_blocks](); // array to store the number of items allocated to each block

        while (!rho_vec[min_block_index].empty()){

            int topid = rho_vec[min_block_index].top().get_index();
            int item_index = topid; // index of the item
            int block_index = min_block_index; // index of the block
            rho_vec[min_block_index].pop();
            if (hashSelected[item_index] == true)
                continue;

            double maxV = vec_functions[block_index]->evalGainsaddFast(dummy_set, item_index) + preV_vec[block_index];
            double func_val = maxV;
            double newV = (maxV - preV_vec[block_index]); // the gain of this item on block 
            if (verb >= 5){
                printf("max gain = %.6e, rho->top() = %.6e\n",newV,rho_vec[min_block_index].top().get_value());
            }
            if (newV < rho_vec[min_block_index].top().get_value()) {
                rho_vec[min_block_index].push(Increment(newV, topid)); // if condition not satisfied, push back and re-sort
                sort_cnt++;
                if (verb >= 10) printf("Condition not met, re-sorting queue (%d iterations)\n",sort_cnt);
            }
            else {
                // guaranteed to be optimal because of submodularity
                hashSelected[item_index] = true;
                vecSets[block_index].insert(item_index);
                block_value_vec[block_index] += newV; 
                nSelected++;
                if (verb >= 3){
                    printf("Allocate %dth sample into %dth block (%d selected, current increment = %.6e), preV = %.10e, curV = %.10e, num_resort=%d\n", item_index, block_index, nSelected, newV, preV_vec[block_index], maxV, sort_cnt);
                }
                preV_vec[block_index] = maxV;
                block_value_vec[block_index] = maxV; 
                vec_functions[block_index]->updateStatisticsAdd(dummy_set, item_index);

                sort_cnt = 0;
                //double min_val = block_value_vec[0];
                double min_val = 1e30;
                for (int i=0;i<m_blocks;i++){
                    if (block_value_vec[i] < min_val){
                        min_val = block_value_vec[i];
                        min_block_index = i;
                    }
                }

            }    
        }
        for (int idx=0; idx<m_blocks; idx++){
            delete vec_functions[idx];
        }
    }
		
}
