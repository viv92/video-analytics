/*
    Copyright 2016 AITOE
*/

#ifndef EMOTION_RECOGNITION_H
#define EMOTION_RECOGNITION_H

#include <iostream>
#include <string>
#include <vector>

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "../aiSaacSettings.h"
#include "../utils/caffeClassifier.h"

namespace aiSaac {
class EmotionRecognition {
 public:
    EmotionRecognition(AiSaacSettings &aiSaacSettings);
    explicit EmotionRecognition(AiSaacSettings &aiSaacSettings, CaffeClassifier &caffeClassifier);
    void initialize(CaffeClassifier &caffeClassifier);
    std::string detect(const cv::Mat &rawFrame);
    std::string runAlgo(const cv::Mat &rawFrame);
    ~EmotionRecognition();
 private:
    CaffeClassifier *caffeClassifier;
    AiSaacSettings &aiSaacSettings;
    bool loadedAlgos = false;
};
}

#endif  // EMOTION_RECOGNITION_H
