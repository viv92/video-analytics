/*
    Copyright 2016 AITOE
*/

#ifndef AGE_RECOGNITION_H
#define AGE_RECOGNITION_H

#include <iostream>
#include <string>
#include <vector>

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "../aiSaacSettings.h"
#include "../utils/caffeClassifier.h"

namespace aiSaac {
class AgeRecognition {
 public:
    AgeRecognition(AiSaacSettings &aiSaacSettings);
    explicit AgeRecognition(AiSaacSettings &aiSaacSettings, CaffeClassifier &caffeClassifier);
    void initialize(CaffeClassifier &caffeClassifier);
    std::string runAlgo(const cv::Mat &rawFrame);
    std::string detect(const cv::Mat &rawFrame);
    double detectExpected(const cv::Mat &rawFrame);
 private:
    CaffeClassifier caffeClassifier;
    AiSaacSettings &aiSaacSettings;
};
}

#endif  // AGE_RECOGNITION_H
