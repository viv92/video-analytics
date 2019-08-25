/*
    Copyright 2016 AITOE
*/

#include "ageRecognition.h"

aiSaac::AgeRecognition::AgeRecognition(aiSaac::AiSaacSettings &aiSaacSettings) : aiSaacSettings(aiSaacSettings) {
    // if (caffeClassifier == NULL) {
    {
        this->caffeClassifier = CaffeClassifier(
            this->aiSaacSettings.getAgePrototxtPath(),
            this->aiSaacSettings.getAgeCaffemodelPath(),
            this->aiSaacSettings.getAgeMeanFilePath(),
            this->aiSaacSettings.getAgeLabelFilePath());
    }
}

aiSaac::AgeRecognition::AgeRecognition(aiSaac::AiSaacSettings &aiSaacSettings, aiSaac::CaffeClassifier &caffeClassifier) : aiSaacSettings(aiSaacSettings) {
    std::cout << "Initializing new caffe classifier" << std::endl;
    this->initialize(caffeClassifier);
    std::cout << "Age recognition initialized" << std::endl;
}

void aiSaac::AgeRecognition::initialize(CaffeClassifier &caffeClassifier) {
    // if (caffeClassifier == NULL) {
    //     this->caffeClassifier = new CaffeClassifier(
    //         this->aiSaacSettings.getAgePrototxtPath(),
    //         this->aiSaacSettings.getAgeCaffemodelPath(),
    //         this->aiSaacSettings.getAgeMeanFilePath(),
    //         this->aiSaacSettings.getAgeLabelFilePath());
    // }
    // else {
    //     this->caffeClassifier = &caffeClassifier;
    // }
    this->caffeClassifier = caffeClassifier;
}

std::string aiSaac::AgeRecognition::detect(const cv::Mat &rawFrame) {
    return runAlgo(rawFrame);
}

std::string aiSaac::AgeRecognition::runAlgo(const cv::Mat &rawFrame) {
    std::vector<std::pair<std::string, float>> prediction =
        this->caffeClassifier.Classify(rawFrame);
    std::string predictedClass;
    // if (prediction[0].second >=
    //     this->aiSaacSettings.getAgeRecognitionThreshold()) {
    //     predictedClass = prediction[0].first;
    // } else {
    //     predictedClass = "unassigned";
    // }
    // return predictedClass;
    return prediction[0].first;
}

double aiSaac::AgeRecognition::detectExpected(const cv::Mat &rawFrame) {
    std::vector<std::pair<std::string, float>> prediction =
        this->caffeClassifier.Classify(rawFrame, 101);
    double age = 0;
    for (int i = 0; i < prediction.size(); i++) {
        int pred = 0;
        std::stringstream convert(prediction[i].first);
        convert>>pred;
        age = age + pred*prediction[i].second;
        std::cout << i << " " << prediction[i].second << std::endl;
    }
    return age;
}
