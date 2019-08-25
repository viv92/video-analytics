/*
    Copyright 2016 AITOE
*/

#ifndef FACE_ANALYTICS_H
#define FACE_ANALYTICS_H

#include <opencv2/objdetect/objdetect.hpp>

#include <iostream>
#include <string>
#include <vector>

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

// has to be defined before dlib makes a redefinition of CBLAS functions

#include "../utils/NPDFaceDetection/npddetect.h"

#include "../utils/dlib/image_processing/frontal_face_detector.h"
#include "../utils/dlib/image_processing/render_face_detections.h"
#include "../utils/dlib/image_processing.h"
#include "../utils/dlib/image_transforms.h"
#include "../utils/dlib/opencv.h"

#include <datk.h>

#include "../aiSaacSettings.h"
#include "faceBlob.h"

namespace aiSaac {
class FaceAnalytics {
 public:
    explicit FaceAnalytics(AiSaacSettings &aiSaacSettings);
    ~FaceAnalytics();

    void initialize();

    int detect(const cv::Mat &rawFrame, int frameNumber, cv::Rect areaOfInterest = cv::Rect());
    void localize(const cv::Mat &rawFrame,
        std::vector<cv::Rect> &localizedRectangles,
        int frameNumber = 0,
        cv::Rect areaOfInterest = cv::Rect());
    void track(const cv::Mat &rawFrame, int frameNumber, cv::Rect areaOfInterest = cv::Rect());
    void recognize();

    void annotate(cv::Mat &rawFrame);
    void annotate(cv::Mat &rawFrame, int frameNumber);

    std::vector<aiSaac::FaceBlob> blobContainer;

 private:
     int ID;
     int analyzedFrameNumber;

     void loadAlgorithmType();
     void runAlgo(const cv::Mat &rawFrame,
         int frameNumber,
         std::vector<cv::Rect> &localizedRectangles);
     void preProcessBlobContainer(int frameNumber);
     float pearsonSimilarity(std::vector<float> features1, std::vector<float> features2);
     float mean(std::vector<float> vector);

     cv::CascadeClassifier faceCascadeClassifier;
     npd::npddetect npd;
     dlib::frontal_face_detector dlibDetector;
     AiSaacSettings &aiSaacSettings;
};
}  // namespace aiSaac

#endif  // HEAD_ANALYTICS_H
