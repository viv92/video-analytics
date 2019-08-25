/*
    Copyright 2016 AITOE
*/

#ifndef SRC_AISENTINELLITE_AISAAC_BGSUBANALYTICS_BLOBOBJECT_H_
#define SRC_AISENTINELLITE_AISAAC_BGSUBANALYTICS_BLOBOBJECT_H_


#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <vector>

#include <vector>

namespace aiSaac {
class BlobObject {
 public:
    int ID;
    int firstFrameNumber;
    int lastFrameNumber;
    int lastHumanFrameNumber;
    int frameCount;
    int collision;
    cv::Point currentPosition;
    std::vector<cv::Point> pastPositions;
    cv::MatND currentHist;
    cv::Rect firstRectangle;
    cv::Rect lastRectangle;
    std::vector<int> contactRectsIndex;
    std::vector<cv::Mat> frames;
    std::vector<cv::MatND> histograms;
    time_t startTime;
    time_t endTime;
    bool isCounted;
    int humanDetCount;
    int histDetCount;
    double currTime;
    unsigned long permanentImageNo;
    bool isValid;
};
}  // namespace aiSaac

#endif  // SRC_AISENTINELLITE_AISAAC_BGSUBANALYTICS_BLOBOBJECT_H_
