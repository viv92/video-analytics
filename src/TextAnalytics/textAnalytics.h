/*
    Copyright 2016 AITOE
*/

#ifndef TEXT_ANALYTICS_H
#define TEXT_ANALYTICS_H

#include <opencv2/objdetect/objdetect.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <ctype.h>

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "tesseract/baseapi.h"

#include "ccv.h"

#include "../aiSaacSettings.h"

#include "textBlob.h"

namespace aiSaac {
class TextAnalytics {
 public:
    explicit TextAnalytics(AiSaacSettings &aiSaacSettings);
    ~TextAnalytics();

    void initialize();

    int detect(const cv::Mat &rawFrame, int frameNumber, cv::Rect areaOfInterest = cv::Rect());
    void localize(const cv::Mat &rawFrame,
        std::vector<cv::Rect> &localizedRectangles,
        int frameNumber = 0,
        cv::Rect areaOfInterest = cv::Rect());
    void track(const cv::Mat &rawFrame, int frameNumber, cv::Rect areaOfInterest = cv::Rect());
    void recognize();
    std::string recognize(const cv::Mat &rawFrame);

    void annotate(cv::Mat &rawFrame);
    void annotate(cv::Mat &rawFrame, int frameNumber);
    void annotate(cv::Mat &rawFrame, std::vector<cv::Rect> localizedRectangles);
    void annotate(cv::Mat &rawFrame,
        std::vector<std::pair<std::string, cv::Rect>> localizedLabeledRectangles);

    std::vector<aiSaac::TextBlob> blobContainer;

 private:
    int ID;
    int analyzedFrameNumber;

    void loadAlgorithmType();
    void runAlgo(const cv::Mat &rawFrame,
        int frameNumber,
        std::vector<cv::Rect> &localizedRectangles);
     void preProcessBlobContainer(int frameNumber);

    tesseract::TessBaseAPI *tess;
    AiSaacSettings aiSaacSettings;
};
}  // namespace aiSaac

#endif  // TEXT_ANALYTICS_H
