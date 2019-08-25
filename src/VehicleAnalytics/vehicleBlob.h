/*
    Copyright 2016 AITOE
*/

#ifndef VEHICLE_BLOB_H
#define VEHICLE_BLOB_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <vector>

namespace aiSaac {
class VehicleBlob {
 public:
    int ID;
    int firstFrameNumber;
    int lastFrameNumber;
    int frameCount;
    std::vector<cv::Point> pastPositions;
    cv::Rect firstRectangle;
    cv::Rect lastRectangle;
    std::vector<cv::Mat> frames;
};
}  // namespace aiSaac

#endif  // VEHICLE_BLOB_H
