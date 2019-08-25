/*
    Copyright 2016 AITOE
*/

#ifndef FACE_BLOB_H
#define FACE_BLOB_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <vector>
#include <string>

namespace aiSaac {
class FaceBlob {
 public:
    int ID;
    int firstFrameNumber;
    int lastFrameNumber;
    int frameCount;
    std::string label;
    std::vector<cv::Point> pastPositions;
    cv::Rect firstRectangle;
    cv::Rect lastRectangle;
    std::vector<cv::Mat> faceImages;
};
}  // namespace aiSaac

#endif  // HEAD_BLOB_H
