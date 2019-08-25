/*
    Copyright 2016 AITOE
*/

#include "genderRecognition.h"

aiSaac::GenderRecognition::GenderRecognition(aiSaac::AiSaacSettings &aiSaacSettings) : aiSaacSettings(aiSaacSettings) {
    // if (caffeClassifier == NULL) {
    {
        std::cout << "Initializing new caffe classifier" << std::endl;
        this->caffeClassifier = CaffeClassifier(
            this->aiSaacSettings.getGenderPrototxtPath(),
            this->aiSaacSettings.getGenderCaffemodelPath(),
            this->aiSaacSettings.getGenderMeanFilePath(),
            this->aiSaacSettings.getGenderLabelFilePath());
    }
    std::cout << "Gender recognition initialized" << std::endl;
}

aiSaac::GenderRecognition::GenderRecognition(aiSaac::AiSaacSettings &aiSaacSettings, aiSaac::CaffeClassifier &caffeClassifier) : aiSaacSettings(aiSaacSettings) {
    std::cout << "Initializing new caffe classifier" << std::endl;
    this->initialize(caffeClassifier);
    std::cout << "Gender recognition initialized" << std::endl;
}

aiSaac::GenderRecognition::~GenderRecognition() {
    // std::cout << "Deleting caffe classifier" << std::endl;
    // delete this->caffeClassifier;
}

void aiSaac::GenderRecognition::initialize(CaffeClassifier &caffeClassifier) {
    // if (caffeClassifier == NULL) {
    //     this->caffeClassifier = new CaffeClassifier(
    //         this->aiSaacSettings.getGenderPrototxtPath(),
    //         this->aiSaacSettings.getGenderCaffemodelPath(),
    //         this->aiSaacSettings.getGenderMeanFilePath(),
    //         this->aiSaacSettings.getGenderLabelFilePath());
    // }
    // else {
    //     this->caffeClassifier = caffeClassifier;
    // }
    this->caffeClassifier = caffeClassifier;
}

std::string aiSaac::GenderRecognition::detect(const cv::Mat &rawFrame) {
    return runAlgo(rawFrame);
}

std::string aiSaac::GenderRecognition::runAlgo(const cv::Mat &rawFrame) {
    std::vector<std::pair<std::string, float>> prediction =
        this->caffeClassifier.Classify(rawFrame);
    std::string predictedClass;
    if (prediction[0].second >=
        this->aiSaacSettings.getGenderRecognitionThreshold()) {
        predictedClass = prediction[0].first;
    } else {
        predictedClass = "unassigned";
    }
    return predictedClass;
}
