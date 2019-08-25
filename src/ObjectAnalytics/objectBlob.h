/*
    Copyright 2016 AITOE
*/

#ifndef OBJECT_BLOB_H
#define OBJECT_BLOB_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <vector>
#include <string>

namespace aiSaac {
class ObjectBlob {
 public:
    int ID;
    std::string label;
    int firstFrameNumber;
    int lastFrameNumber;
    int motionFrameNumber;
    int frameCount;
    std::vector<cv::Point> pastPositions;
    cv::Rect firstRectangle;
    cv::Rect lastRectangle;
    std::vector<cv::Mat> frames;
    std::vector<std::string> metaData;
    bool isValid;
};
}  // namespace aiSaac

#endif  // OBJECT_BLOB_H
