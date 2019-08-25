#ifndef SUMMARIZATION_H
#define SUMMARIZATION_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv/cv.hpp>
// #include <opencv2/video/background_segm.hpp>
// #include <opencv2/contrib/contrib.hpp>

#include <iostream>
#include <string>
#include <vector>

#include "../aiSaacSettings.h"
#include "../utils/json.hpp"
#include "../utils/caffeClassifier.h"
#include "../ObjectAnalytics/objectAnalytics.h"
#include "../FaceAnalytics/faceAnalytics.h"
// #include "../utils/Summarization/summarizeUtils.h"

namespace aiSaac {
class Summarization {
 public:
    Summarization(AiSaacSettings &aiSaacSettings, std::string filePath);
    Summarization(aiSaac::AiSaacSettings &aiSaacSettings,
        std::string videoFilePath,
        aiSaac::CaffeClassifier &sceneClassifier,
        aiSaac::CaffeClassifier &ffObjectClassifier,
        aiSaac::CaffeClassifier &dnObjectClassifier,
        aiSaac::CaffeClassifier &faceGenderClassifier,
        aiSaac::CaffeClassifier &faceAgeClassifier,
        aiSaac::CaffeClassifier &faceRecClassifier,
        aiSaac::ObjectAnalytics &objectAnalytics,
        aiSaac::FaceAnalytics &faceAnalytics);
    ~Summarization();

    void featurizeVideo(int procFPS = -1);
    void summarize();

 private:
    void loadAlgorithmType();
    void computeColorHist(cv::Mat &rawFrame, cv::Mat &sceneHist);
    void computeHistKernel(std::vector<cv::Mat> &hists,
        std::vector<std::vector<float>> &histKernel);
    void computeKernel(std::vector<std::vector<double>> featureVectors,
        std::vector<cv::Mat> hists,
        nlohmann::json &collection);
    void loadCollection(std::string filePath, nlohmann::json &featureFile);
    void saveCollection(std::string collectionFilePath,
        nlohmann::json &collection,
        std::vector<cv::Mat> &hists);

    AiSaacSettings &aiSaacSettings;

    bool loadedAlgos = false;
    std::string videoFilePath;

    aiSaac::CaffeClassifier *sceneClassifier;
    nlohmann::json sceneCollection;
    std::vector<cv::Mat> sceneHists;

    aiSaac::CaffeClassifier *ffObjectClassifier;
    nlohmann::json ffObjectCollection;
    std::vector<cv::Mat> ffObjectHists;

    aiSaac::CaffeClassifier *dnObjectClassifier;
    nlohmann::json dnObjectCollection;
    std::vector<cv::Mat> dnObjectHists;

    aiSaac::CaffeClassifier *faceGenderClassifier;
    nlohmann::json faceGenderCollection;
    std::vector<cv::Mat> faceGenderHists;

    aiSaac::CaffeClassifier *faceAgeClassifier;
    nlohmann::json faceAgeCollection;
    std::vector<cv::Mat> faceAgeHists;

    aiSaac::CaffeClassifier *faceRecClassifier;
    nlohmann::json faceRecCollection;
    std::vector<cv::Mat> faceRecHists;

    aiSaac::ObjectAnalytics *objectAnalytics;
    aiSaac::FaceAnalytics *faceAnalytics;
};
}

#endif  // SUMMARIZATION_H
