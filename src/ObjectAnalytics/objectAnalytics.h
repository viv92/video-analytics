/*
    Copyright 2016 AITOE
*/

#ifndef OBJECT_ANALYTICS_H
#define OBJECT_ANALYTICS_H

#include <iostream>
#include <vector>
#include <fstream>
#include <iomanip> // to format image names using setw() and setfill()
//#include <io.h>    // to check file existence using POSIX function access(). On Linux include <unistd.h>.
#include <unistd.h>
#include <set>

#include "opencv2/video/tracking.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "../aiSaacSettings.h"
#include "../utils/colorClassifier.h"
#include "../utils/dnnClassifier.h"
#include "../utils/Hungarian.h"
#include "../utils/KalmanTracker.h"
#ifdef CAFFE
#include "../utils/caffeClassifier.h"
#endif
#ifdef DARKNET
#include "../utils/darknetClassifier.h"
#endif
#include "objectBlob.h"

extern int personCount, carCount; //remove later

namespace aiSaac {
class ObjectAnalytics {
 public:
    ObjectAnalytics(AiSaacSettings &aiSaacSettings);
    #ifdef CAFFE
        ObjectAnalytics(AiSaacSettings &aiSaacSettings, aiSaac::CaffeClassifier &caffeClassiifer);
    #endif
    #ifdef DARKNET
        ObjectAnalytics(AiSaacSettings &aiSaacSettings, aiSaac::DarknetClassifier &darknetClassifier);
    #endif
    #ifdef DNN_CLASSIFIER
        ObjectAnalytics(AiSaacSettings &aiSaacSettings, aiSaac::DNNClassifier &dnnClassifier);
    #endif
    // codeReview(Anurag): deprecated constructor. Use `ObjectAnalytics(AiSaacSettings &aiSaacSettings, aiSaac::CaffeClassifier &caffeClassiifer);` or  `ObjectAnalytics(AiSaacSettings &aiSaacSettings, aiSaac::DarknetClassifier &darknetClassifier);`
    [[deprecated("deprecated constructor. Use `ObjectAnalytics(AiSaacSettings &aiSaacSettings, aiSaac::CaffeClassifier &caffeClassiifer);` or  `ObjectAnalytics(AiSaacSettings &aiSaacSettings, aiSaac::DarknetClassifier &darknetClassifier);`")]]
    #ifdef CAFFE_DARKNET
        ObjectAnalytics(AiSaacSettings &aiSaacSettings, aiSaac::CaffeClassifier &caffeClassifier, aiSaac::DarknetClassifier &darknetClassifier);
    #endif
    ~ObjectAnalytics();

    void initialize();

    std::vector<std::string> detect(const cv::Mat &rawFrame,
        int frameNumber = 0,
        cv::Rect areaOfInterest = cv::Rect());
    void localize(const cv::Mat &rawFrame,
        std::vector<std::pair<std::string, cv::Rect>> &localizedLabeledRectangles,
        int frameNumber = 0,
        cv::Rect areaOfInterest = cv::Rect());
    void track(const cv::Mat &rawFrame, int frameNumber, cv::Rect areaOfInterest = cv::Rect());
    void track(const cv::Mat &rawFrame, int frameNumber, int motionFrameNumber, cv::Rect areaOfInterest = cv::Rect());
    void trackSort(const cv::Mat &rawFrame, int frameNumber, cv::Rect areaOfInterest = cv::Rect());

    void annotateSort(cv::Mat &rawFrame);
    void annotateSort(cv::Mat &rawFrame, int frameNumber);
    void annotate(cv::Mat &rawFrame);
    void annotate(cv::Mat &rawFrame, int frameNumber);
    void annotate(cv::Mat &rawFrame, const std::vector<std::string> &labels);
    void annotate(cv::Mat &rawFrame,
    const std::vector<std::pair<std::string, cv::Rect>> &localizedLabeledRectangles);

    std::vector<aiSaac::ObjectBlob> blobContainer;

 private:
    int ID;
    int analyzedFrameNumber;
    bool loadedAlgos = false;

    void loadAlgorithmType();
    void preProcessBlobContainer(int frameNumber);
    void preProcessBlobContainerMotion(int frameNumber);
    void runAlgo(const cv::Mat &rawFrame,
        int frameNumber,
        std::vector<aiSaac::ObjectBlob> &detectedObjects);

    AiSaacSettings &aiSaacSettings;
    ColorClassifier *colorClassifier;
    #ifdef CAFFE
        CaffeClassifier *caffeClassifier;
    #endif
    #ifdef DARKNET
        DarknetClassifier *darknetClassifier;
    #endif
    #ifdef DNN_CLASSIFIER
        DNNClassifier *dnnClassifier;
    #endif
};
}

#endif  // OBJECT_ANALYTICS_H
