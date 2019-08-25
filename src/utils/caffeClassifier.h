/*
    Copyright 2016 AITOE
*/

#ifndef CAFFE_CLASSIFIER_H
#define CAFFE_CLASSIFIER_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <algorithm>
#include <iosfwd>
#include <string>
#include <vector>
#include <utility>
#include <memory>

#include <caffe/caffe.hpp>

// typedef std::pair<std::string, float> Prediction;

namespace aiSaac {
class CaffeClassifier {
 public:
    // CaffeClassifier(const std::string &network_file, const std::string &trained_file);
    CaffeClassifier();
    CaffeClassifier(const std::string& network_file,
        const std::string& trained_file,
        const std::string& mean_file = "",
        const std::string& label_file = "");

    std::vector<std::pair<std::string, float>> Classify(const cv::Mat& img,
        int N = 5);
    std::vector<float> Predict(const cv::Mat &img, std::string layer = "", const bool &verbose = false);
    std::vector<float> getLayerOutput(std::string layer);
    bool isEmpty();
    // codeReview(Anurag): net_ to be made private
 private:
    void WrapInputLayer(std::vector<cv::Mat> *input_channels);
    void Preprocess(const cv::Mat& img, std::vector<cv::Mat> *input_channels);
    void SetMean(const std::string& mean_file);


    caffe::shared_ptr<caffe::Net<float>> net_;
    cv::Size input_geometry_;
    int num_channels_;
    cv::Mat mean_;
    std::vector<std::string> labels_;
    bool empty = true;
};
}  // namespace aiSaac

#endif  // CAFFE_CLASSIFIER_H
