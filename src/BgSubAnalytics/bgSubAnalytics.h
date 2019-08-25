/*
    Copyright 2016 AITOE
*/
#ifndef SRC_AISENTINELLITE_AISAAC_BGSUBANALYTICS_BGSUBANALYTICS_H_
#define SRC_AISENTINELLITE_AISAAC_BGSUBANALYTICS_BGSUBANALYTICS_H_

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv/cv.hpp>
#include <opencv2/video/background_segm.hpp>
#include <opencv2/bgsegm.hpp>
// #include <opencv2/contrib/contrib.hpp>

#include <iostream>
#include <string>
#include <vector>

#include "../aiSaacSettings.h"
#include "../utils/BackgroundSubtraction/MLBGS/MultiLayer.h"
// #include "../utils/BackgroundSubtraction/VIBE/vibe.h"
#include "blobObject.h"

using namespace cv;
using namespace cv::bgsegm;

namespace aiSaac {
class BgSubAnalytics {
 public:
    BgSubAnalytics(AiSaacSettings &aiSaacSettings);
    ~BgSubAnalytics();
    // std::vector<BlobObject> run();
    void initialize();
    bool detect(const cv::Mat &rawFrame, int frameNumber);
    void localize(const cv::Mat &rawFrame, int frameNumber,
        std::vector<cv::Rect> &localizedRectangles);
    void track(const cv::Mat &rawFrame, int frameNumber);
    void humanize(const cv::Mat &rawFrame, int frameNumber);

    void annotate(cv::Mat &rawFrame);
    void annotate(cv::Mat &rawFrame, int frameNumber);

    // const std::vector<BlobObject>& getBlobContainer();
    bool getMotion();

    std::vector<BlobObject> blobContainer;

 private:
    int analyzedFrameNumber;
    bool isMotion;
    int noMotionFrameCount;
    cv::Mat backgroundFrame;
    cv::Mat rawFrame;
    int lastBGFrameRecorded;

    void loadAlgorithmType();
    void runAlgo(const cv::Mat &rawFrame);
    Ptr<BackgroundSubtractorMOG> pMOG;
    Ptr<BackgroundSubtractorMOG2> pMOG2;
    Ptr<BackgroundSubtractorGMG> pGMG;
    // VIBE vibe;
    MultiLayer bgs;
    // PixelBasedAdaptiveSegmenter pabs;

    cv::Mat foregroundFrame;
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;

    void preProcessBlobContainer(int frameNumber);

    int ID;
    bool firstFrame;

    AiSaacSettings &aiSaacSettings;
};
}  // namespace aiSaac

#endif  // SRC_AISENTINELLITE_AISAAC_BGSUBANALYTICS_BGSUBANALYTICS_H_
