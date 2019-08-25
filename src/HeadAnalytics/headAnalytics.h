/*
    Copyright 2016 AITOE
*/

#ifndef HEAD_ANALYTICS_H
#define HEAD_ANALYTICS_H

#include <opencv2/objdetect/objdetect.hpp>

#include <iostream>
#include <string>
#include <vector>

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "../aiSaacSettings.h"

#include "headBlob.h"

namespace aiSaac {
class HeadAnalytics {
 public:
    explicit HeadAnalytics(AiSaacSettings *aiSaacSettings);
    ~HeadAnalytics();

    void initialize();

    int detect(const cv::Mat &rawFrame, int frameNumber, cv::Rect areaOfInterest = cv::Rect());
    void localize(const cv::Mat &rawFrame,
        int frameNumber,
        std::vector<cv::Rect> &localizedRectangles,
        cv::Rect areaOfInterest = cv::Rect());
    void track(const cv::Mat &rawFrame, int frameNumber, cv::Rect areaOfInterest = cv::Rect());

    void annotate(cv::Mat &rawFrame);
    void annotate(cv::Mat &rawFrame, int frameNumber);

    std::vector<aiSaac::HeadBlob> blobContainer;

 private:
     int ID;
     int analyzedFrameNumber;

     void loadAlgorithmType();
     void runAlgo(const cv::Mat &rawFrame,
         int frameNumber,
         std::vector<cv::Rect> &localizedRectangles);
     void preProcessBlobContainer(int frameNumber);

     cv::CascadeClassifier headCascadeClassifier;
     AiSaacSettings *aiSaacSettings;
};
}  // namespace aiSaac

#endif  // HEAD_ANALYTICS_H
