/*
 *	Abstract base class for implementing (not necessarily submodular) set functions. This implements all oracle functions of general set functions.
	Author: Rishabh Iyer
	Melodi Lab, University of Washington, Seattle
 *
 */
#include "aiVideoAnalytics.h"
#include <iostream>
#include "Utils/aiTime.h"
#include "Utils/aiColorHist.h"
namespace aisaac {

     aiVideoAnalytics::aiVideoAnalytics(std::string videopath): videopath(videopath){
			 capture = VideoCapture(videopath);
		 }

     aiVideoAnalytics::~aiVideoAnalytics(){
			 capture.release();
		 }

     void aiVideoAnalytics::analyze(){}

   void aiVideoAnalytics::outJSON()
   {
      assert(startTimes.size() == endTimes.size());
      std::cout << "{";
      for (int i = 0; i < startTimes.size()-1; i++)
      {
          std::cout << "[StartTime: " << startTimes[i].hours << ":"
          << startTimes[i].minutes << ":" << startTimes[i].seconds
          << ", EndTime: " << endTimes[i].hours << ":"
          << endTimes[i].minutes << ":" << endTimes[i].seconds
          <<  "]";
      }
      int n = startTimes.size()-1;
      std::cout << "[StartTime: " << startTimes[n].hours << ":"
      << startTimes[n].minutes << ":" << startTimes[n].seconds
      << ", EndTime: " << endTimes[n].hours << ":"
      << endTimes[n].minutes << ":" << endTimes[n].seconds
      <<  "]";
      std::cout << "}";
  }
}
