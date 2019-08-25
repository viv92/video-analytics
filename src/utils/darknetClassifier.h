/*
    Copyright 2016 AITOE
*/

#ifndef DARKNET_CLASSIFIER_H
#define DARKNET_CLASSIFIER_H

#include <iostream>
#include <vector>
#include <fstream>

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "network.h"
#include "detection_layer.h"
#include "region_layer.h"
#include "cost_layer.h"
#include "utils.h"
#include "parser.h"
#include "box.h"
// #include "demo.h"
#include "image.h"
#ifdef GPUDARKNET
#include "cuda.h"
#endif

namespace aiSaac {
class DarknetClassifier {
 public:
    DarknetClassifier();
    DarknetClassifier(std::string configFilePath,
        std::string weightFilePath,
        std::string labelFilePath,
        float threshold);
    ~DarknetClassifier();

    float Classify(const cv::Mat &rawFrame, std::vector<std::pair<std::string, cv::Rect>> &result);
    bool isEmpty();
 private:
    bool empty = true;
    network net;
    detection_layer detectionLayer;
    box *boxes;
    float **probs;
    std::vector<std::string> labels;
    float darknetThreshold;
};
}

#endif  // DARKNET_CLASSIFIER_H
