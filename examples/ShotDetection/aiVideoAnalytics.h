/*
 *	Abstract base class header for implementing (not necessarily submodular) set functions. This implements all oracle functions of general set functions.

	All set functions are defined on a ground set V = [n] = {1, 2, ..., n}. If the groundSet is not [n],
	one will need to use a transformation function, which transforms a groundSet U, to V = [|U|], through a mapping.

	Author: Rishabh Iyer
	Melodi Lab, University of Washington, Seattle
 *
 */
#ifndef VIDEO_ANALYTICS_h
#define VIDEO_ANALYTICS_h

#include <vector>
#include "Utils/aiTime.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include <iostream>

namespace aisaac {

	class aiVideoAnalytics{
		//protected:
		protected:
			std::string videopath;
			std::vector<Time> startTimes;
			std::vector<Time> endTimes;
			int nSeconds; // Length of the video in seconds
			cv::VideoCapture capture;

		public:
      aiVideoAnalytics(std::string videopath);
			aiVideoAnalytics(std::string videopath, int FPS);
			virtual ~aiVideoAnalytics();
			virtual void analyze();
			virtual void outJSON();
			void getShots();
			int size();
	};
}
#endif
