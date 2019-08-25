/*
    Copyright 2016 AITOE
*/

#ifndef VIDEO_SUMMARIZATION_H
#define VIDEO_SUMMARIZATION_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv/cv.hpp>
#include <opencv2/video/background_segm.hpp>
// #include <opencv2/contrib/contrib.hpp>

#include <iostream>
#include <string>
#include <vector>

#include "../aiSaacSettings.h"
#include "../utils/Summarization/summarizeUtils.h"

namespace aiSaac {
  class VideoSummarization {
   public:
      VideoSummarization(AiSaacSettings *aiSaacSettings, std::string srcFilePath);
      ~VideoSummarization();

      void summarizeGreedy(int budget);
      void summarizeStream();
      void summarizeGreedyOrdering();
      void writeSummaryFile(std::string destFilePath);
      std::set<int> getKeyPoints();
      std::vector<int> getKeyPointsOrdered();
   private:
      AiSaacSettings *aiSaacSettings;
      std::set<int> keyPoints;
      std::vector<int> orderedKeyPoints;
      std::string srcFilePath;
  };
}

#endif  // VIDEO_SUMMARIZATION_H
