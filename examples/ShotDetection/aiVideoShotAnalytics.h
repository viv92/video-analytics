/*
 *	Abstract base class header for implementing (not necessarily submodular) set functions. This implements all oracle functions of general set functions.

	All set functions are defined on a ground set V = [n] = {1, 2, ..., n}. If the groundSet is not [n],
	one will need to use a transformation function, which transforms a groundSet U, to V = [|U|], through a mapping.

	Author: Rishabh Iyer
	Melodi Lab, University of Washington, Seattle
 *
 */
#ifndef VIDEO_SHOT_ANALYTICS_h
#define VIDEO_SHOT_ANALYTICS_h

#include <vector>
#include "aiVideoAnalytics.h"
#include "Utils/aiTime.h"
#include "Utils/aiColorHist.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>
#include <iostream>

namespace aisaac {

	class aiVideoShotAnalytics: public aiVideoAnalytics
	{
		//protected:
		protected:
			std::vector<int> shotboundaries; // in terms of the number of frames
			double thresh;
			int samplingRate;
			int compare_method;
			int h_bins;
			int s_bins;

		public:
            aiVideoShotAnalytics(std::string videopath);
			aiVideoShotAnalytics(std::string videopath, int FPS);
			~aiVideoShotAnalytics();
			void analyze();
			void outJSON();
			void getShots();
			int size();
            std::vector<int> startFrameNumbers;
            std::vector<int> endFrameNumbers;
	};
}
#endif
