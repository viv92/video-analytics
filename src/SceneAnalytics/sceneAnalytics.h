/*
    Copyright 2016 AITOE
*/

#ifndef SCENE_ANALYTICS_H
#define SCENE_ANALYTICS_H

#include <iostream>
#include <vector>

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "../aiSaacSettings.h"
#include "../utils/caffeClassifier.h"
#include "sceneBlob.h"

namespace aiSaac {
class SceneAnalytics {
 public:
    SceneAnalytics(aiSaac::AiSaacSettings &aiSaacSettings);
    SceneAnalytics(aiSaac::AiSaacSettings &aiSaacSettings, aiSaac::CaffeClassifier &caffeClassifier);
    ~SceneAnalytics();

    void initialize();
    void initialize(aiSaac::CaffeClassifier &caffeClassifier);

    std::vector<std::string> detect(const cv::Mat &rawFrame,
        int frameNumber = 0,
        cv::Rect areaOfInterest = cv::Rect());
    void track(const cv::Mat &rawFrame,
        int frameNumber,
        cv::Rect areaOfInterest = cv::Rect());

    void annotate(cv::Mat &rawFrame);
    void annotate(cv::Mat &rawFrame, int frameNumber);
    void annotate(cv::Mat &rawFrame, std::vector<std::string>& labels);
    std::vector<aiSaac::SceneBlob> blobContainer;

 private:
    int ID;
    int analyzedFrameNumber;

    void loadAlgorithmType();
    void runAlgo(const cv::Mat &rawFrame,
        int frameNumber,
        std::vector<aiSaac::SceneBlob> &detectedScenes);
    void preProcessBlobContainer(int frameNumber);

    AiSaacSettings &aiSaacSettings;
    CaffeClassifier *caffeClassifier;
};
}

#endif  // Scene_ANALYTICS_H
