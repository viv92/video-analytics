/*
    Copyright 2016 AITOE
*/

#ifndef DNN_CLASSIFIER_H
#define DNN_CLASSIFIER_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "opencv2/dnn.hpp"
#include "opencv2/dnn/shape_utils.hpp"
#include <iostream>
//#include <algorithm>
//#include <iosfwd>
#include <string>
#include <vector>
//#include <utility>
//#include <memory>
#include <fstream>

namespace aiSaac {
class DNNClassifier {
 public:
    DNNClassifier();
    DNNClassifier(const std::string& network_file,
        const std::string& trained_file,
        const std::string& mean_file = "",
        const std::string& label_file = "",
        float threshold = 0.3);
    void Classify(const cv::Mat &rawFrame, std::vector<std::pair<std::string, cv::Rect>> &results);
    bool isEmpty();
    // codeReview(Anurag): net_ to be made private
 private:
    cv::Mat Preprocess(const cv::Mat &rawFrame);
    cv::Mat Predict(const cv::Mat &rawFrame);
    std::vector<std::string> labels;
    float dnnThreshold;
    cv::dnn::Net dnnNet;
    bool empty = true;
};
}  // namespace aiSaac

#endif  // DNN_CLASSIFIER_H
