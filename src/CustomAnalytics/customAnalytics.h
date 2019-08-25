/*
    Copyright 2016 AITOE
*/

#ifndef CUSTOM_ANALYTICS_H
#define CUSTOM_ANALYTICS_H

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <dirent.h>

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "caSettings.h"
#include "../utils/caffeTrainer.h"
#include "../utils/caffeClassifier.h"
#include "../utils/json.hpp"
#include "../utils/datk/src/datk.h"

namespace aiSaac {
class CustomAnalytics {
 public:
    CustomAnalytics(CASettings *caSettings, CaffeClassifier *caffeClassifier = NULL);
    ~CustomAnalytics();

    void loadDatkModel();

    std::string detect(const cv::Mat &rawFrame);
    void train(bool loadData=true, int *numCorrect = NULL, int *numTotal = NULL);
    void trainWithCrossVal();
    void test(bool loadData=true, int *numCorrect = NULL, int *numTotal = NULL);

    void loadData(std::string filePath, nlohmann::json &jsonData);
    void saveData(std::string filePath, nlohmann::json jsonData);

    void extractFeatures(std::string dataPath,
        std::vector<std::vector<float>> &featureVectors,
        datk::Vector &intLabels,
        std::vector<std::pair<int, std::string>> &stringToIntLabels,
        nlohmann::json &jsonData,
        bool storeJson = false);

    void extractFeatures(std::string dataPath,
        std::vector<std::vector<float>> &featureVectors,
        datk::Vector &intLabels,
        std::vector<std::pair<int, std::string>> &stringToIntLabels,
        nlohmann::json &jsonData,
        nlohmann::json &jsonNamesData,
        bool storeJson = false);

    CASettings *caSettings;

    nlohmann::json trainingJsonData;
    std::vector<std::vector<float>> trainingFeatureVectors;
    datk::Vector trainingIntLabels;
    std::vector<std::pair<int, std::string>> trainingStringToIntLabels;

    nlohmann::json testingJsonData;
    std::vector<std::vector<float>> testingFeatureVectors;
    datk::Vector testingIntLabels;
    std::vector<std::pair<int, std::string>> testingStringToIntLabels;

    datk::Classifiers<datk::SparseFeature> *model;

 private:

    double predictAccuracy(std::vector<datk::SparseFeature>& testFeatures,
        datk::Vector& ytest);


    CaffeClassifier *caffeClassifier;
    CaffeTrainer *caffeTrainer;

};
}

#endif  // CUSTOM_ANALYTICS_H
