/*
    Copyright 2016 AITOE
*/

#include "emotionRecognition.h"

aiSaac::EmotionRecognition::EmotionRecognition(aiSaac::AiSaacSettings &aiSaacSettings) : aiSaacSettings(aiSaacSettings) {
    this->loadedAlgos = true;
    this->caffeClassifier = new CaffeClassifier(
        this->aiSaacSettings.getEmotionPrototxtPath(),
        this->aiSaacSettings.getEmotionCaffemodelPath(),
        this->aiSaacSettings.getEmotionMeanFilePath(),
        this->aiSaacSettings.getEmotionLabelFilePath());
}

aiSaac::EmotionRecognition::EmotionRecognition(aiSaac::AiSaacSettings &aiSaacSettings, aiSaac::CaffeClassifier &caffeClassifier) : aiSaacSettings(aiSaacSettings) {
    this->initialize(caffeClassifier);
}

aiSaac::EmotionRecognition::~EmotionRecognition() {
    if (loadedAlgos) {
        delete this->caffeClassifier;
    }
}

void aiSaac::EmotionRecognition::initialize(CaffeClassifier &caffeClassifier) {
    // if (caffeClassifier == NULL) {
    //     this->caffeClassifier = new CaffeClassifier(
    //        this->aiSaacSettings.getEmotionPrototxtPath(),
    //        this->aiSaacSettings.getEmotionCaffemodelPath(),
    //        this->aiSaacSettings.getEmotionMeanFilePath(),
    //        this->aiSaacSettings.getEmotionLabelFilePath());
    // }
    // else {
    //     this->caffeClassifier = &caffeClassifier;
    // }
    this->caffeClassifier = &caffeClassifier;
}

std::string aiSaac::EmotionRecognition::detect(const cv::Mat &rawFrame) {
    return runAlgo(rawFrame);
}

std::string aiSaac::EmotionRecognition::runAlgo(const cv::Mat &rawFrame) {
    std::vector<std::pair<std::string, float>> prediction =
        this->caffeClassifier->Classify(rawFrame);
    std::cout << "predicted emotion!" << std::endl;
    for (size_t i = 0; i < 1; ++i) {
        std::pair<std::string, float> p = prediction[i];
        std::cout << std::fixed << std::setprecision(4) << p.first << std::endl;
    }
    return prediction[0].first;
}
