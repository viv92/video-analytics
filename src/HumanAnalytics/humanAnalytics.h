/*
    Copyright 2016 AITOE
*/

#ifndef HUMAN_ANALYTICS_H
#define HUMAN_ANALYTICS_H

#include <opencv2/objdetect/objdetect.hpp>

#include <iostream>
#include <string>
#include <vector>

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "ccv.h"
#include <sys/time.h>
#include <ctype.h>

#include "../utils/colorClassifier.h"
#include "../aiSaacSettings.h"
#include "humanBlob.h"

namespace aiSaac {
class HumanAnalytics {
 public:
    explicit HumanAnalytics(AiSaacSettings *aiSaacSettings);
    ~HumanAnalytics();

    void initialize();

    int detect(const cv::Mat &rawFrame, int frameNumber, cv::Rect areaOfInterest = cv::Rect());
    void localize(const cv::Mat &rawFrame, int frameNumber, std::vector<cv::Rect> &localizedRectangles, cv::Rect areaOfInterest = cv::Rect());
    void track(const cv::Mat &rawFrame, int frameNumber, cv::Rect areaOfInterest = cv::Rect());

    void annotate(cv::Mat &rawFrame);
    void annotate(cv::Mat &rawFrame, int frameNumber);
    void annotate(cv::Mat &rawFrame, cv::Rect &localizedRectangle, int frameNumber);
    void annotate(cv::Mat &rawFrame, cv::Rect &localizedRectangle);

    void runAlgo(const cv::Mat &rawFrame, int frameNumber,
        std::vector<cv::Rect> &localizedRectangles);
    std::vector<aiSaac::HumanBlob> blobContainer;

 private:
     int ID;
     int analyzedFrameNumber;

     void loadAlgorithmType();
     void preProcessBlobContainer(int frameNumber);

     cv::CascadeClassifier faceCascadeClassifier;
     cv::HOGDescriptor hog;
     ccv_icf_classifier_cascade_t* icfcascade;
     ccv_dpm_mixture_model_t* dpmmodel;
     ccv_dpm_param_t ccv_dpm_params;
     ccv_icf_param_t ccv_icf_params;
     cv::CascadeClassifier fullbody_cascade;
     cv::CascadeClassifier upperbody_cascade;
     cv::CascadeClassifier lowerbody_cascade;

     ColorClassifier *colorClassifier;

     AiSaacSettings *aiSaacSettings;
};
}  // namespace aiSaac

#endif  // HEAD_ANALYTICS_H
