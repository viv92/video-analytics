/*
    Copyright 2016 AITOE
*/

#ifndef FACE_RECOGNITION_H
#define FACE_RECOGNITION_H

#include <iostream>
#include <string>
#include <vector>
#include <dirent.h>
#include <fstream>
#include <algorithm>
#include <unordered_map>

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "../aiSaacSettings.h"
#include "../utils/caffeClassifier.h"

namespace aiSaac {

class FaceRecognition {
 public:
    explicit FaceRecognition(AiSaacSettings &aiSaacSettings);
    ~FaceRecognition();
    void initialize();
    void addToFaceDB(const cv::Mat &rawFrame, std::string label);
    void addStoredFacesToDB();
    void recognize();
    std::string recognize(const cv::Mat &rawFrame);
 private:
    void loadFaceDB();
    void saveFaceDB();
    CaffeClassifier *caffeClassifier;
    AiSaacSettings &aiSaacSettings;
    nlohmann::json faceDB;
};
}  // aiSaac FaceRecognition

#endif  // FACE_RECOGNITION_H
