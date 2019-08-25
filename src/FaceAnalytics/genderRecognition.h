/*
    Copyright 2016 AITOE
*/

#ifndef GENDER_RECOGNITION_H
#define GENDER_RECOGNITION_H

#include <iostream>
#include <string>
#include <vector>

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "../aiSaacSettings.h"
#include "../utils/caffeClassifier.h"

namespace aiSaac {
class GenderRecognition {
 public:
    GenderRecognition(AiSaacSettings &aiSaacSettings);
    explicit GenderRecognition(AiSaacSettings &aiSaacSettings, CaffeClassifier &caffeClassifier);
    void initialize(CaffeClassifier &caffeClassifier);
    ~GenderRecognition();
    std::string runAlgo(const cv::Mat &rawFrame);
    std::string detect(const cv::Mat &rawFrame);
 private:
    CaffeClassifier caffeClassifier;
    AiSaacSettings &aiSaacSettings;
};
}

#endif  // GENDER_RECOGNITION_H
