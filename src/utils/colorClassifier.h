/*
    Copyright 2016 AITOE
*/

#ifndef COLOR_CLASSIFIER_H
#define COLOR_CLASSIFIER_H

#include <iostream>
#include <string>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "opencv2/imgproc/imgproc.hpp"
#include <cv.h>

namespace aiSaac {
class ColorClassifier {
 public:
    ColorClassifier();
    std::string patchColor(const cv::Mat &rawimage, cv::Rect &roiRect);
 private:
    int getPixelColorType(int H, int S, int V);
};
}

#endif  // COLOR_CLASSIFIER_H
