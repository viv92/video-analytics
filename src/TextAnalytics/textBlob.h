/*
    Copyright 2016 AITOE
*/

#ifndef TEXT_BLOB_H
#define TEXT_BLOB_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <vector>

namespace aiSaac {
class TextBlob {
 public:
    int ID;
    int firstFrameNumber;
    int lastFrameNumber;
    int frameCount;
    std::vector<cv::Point> pastPositions;
    cv::Rect firstRectangle;
    cv::Rect lastRectangle;
    std::string text;
    std::vector<cv::Mat> textImages;
};
}  // namespace aiSaac

#endif  // TEXT_BLOB_H
