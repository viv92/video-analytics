/*
    Copyright 2016 AITOE
*/

#ifndef NSFW_ANALYTICS_H
#define NSFW_ANALYTICS_H

#include <iostream>
#include <string>
#include <vector>

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "../aiSaacSettings.h"
#include "../utils/caffeClassifier.h"

namespace aiSaac {
class NSFWAnalytics {
 public:
    explicit NSFWAnalytics(AiSaacSettings *aiSaacSettings);
    void initialize();
    bool detect(const cv::Mat &rawFrame);
 private:
    void loadAlgorithmType();
    std::string runAlgo(const cv::Mat &rawFrame);
    CaffeClassifier *caffeClassifier;
    AiSaacSettings *aiSaacSettings;
};
}

#endif  // NSFW_ANALYTICS_H
