/*
    Copyright 2016 AITOE
*/

#ifndef SRC_AISENTINELLITE_AISAAC_CMTTRACKING_CMTTRACKING_H_
#define SRC_AISENTINELLITE_AISAAC_CMTTRACKING_CMTTRACKING_H_

#include <string>
#include <vector>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/opencv.hpp>
#include "../aiSaacSettings.h"
#include "../utils/CPTTracking/CMT.h"



namespace aiSaac {
class CmtTracking {
 public:
    CmtTracking(AiSaacSettings *aiSaacSettings);
    ~CmtTracking();

    void initialize();

    int detect(cv::Mat &rawFrame, cv::Rect selection);
    void track(cv::Mat &rawFrame, int frameNumber);
    void annotate(cv::Mat &rawFrame);
    void annotate(cv::Mat &rawFrame, int frameNumber);
    cv::Rect localize();
 private:
    cv::Rect currentRect;
    cmt::CMT* cmtObj;
    int analyzedFrameNumber;

    AiSaacSettings *aiSaacSettings;
};
}  // namespace aiSaac

#endif  // SRC_AISENTINELLITE_AISAAC_CMTTRACKING_CMTTRACKING_H_
