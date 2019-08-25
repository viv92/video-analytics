/*
    Copyright 2016 AITOE
*/

#include "NSFWAnalytics.h"

aiSaac::NSFWAnalytics::NSFWAnalytics(aiSaac::AiSaacSettings *aiSaacSettings) {
    this->aiSaacSettings = aiSaacSettings;
    this->initialize();
}

void aiSaac::NSFWAnalytics::initialize() {
    this->loadAlgorithmType();
    std::cout << "loaded and initialized NSFWAnalytics" << std::endl;
}

bool aiSaac::NSFWAnalytics::detect(const cv::Mat &rawFrame) {
    std::string detectedNSFW;

    // if (areaOfInterest.area() != 0) {
    //     cv::Mat subRawFrame = rawFrame(areaOfInterest);
    //     this->runAlgo(subRawFrame, detectedNSFW);
    // } else {
        detectedNSFW = this->runAlgo(rawFrame);
    // }
    // std::vector<std::string> predictionLabels;
    // for (int i = 0; i < detectedNSFW.size(); i++) {
    std::cout << "Detected NSFW: " << detectedNSFW << std::endl;
    if (detectedNSFW == "True") {
        return true;
    } else {
        return false;
    }
}

// void aiSaac::NSFWAnalytics::annotate(cv::Mat &rawFrame) {
//     this->annotate(rawFrame, this->analyzedFrameNumber);
// }
//
// void aiSaac::NSFWAnalytics::annotate(cv::Mat &rawFrame, int frameNumber) {
//     for (int i = 0; i < this->blobContainer.size(); i++) {
//         if (this->blobContainer[i].lastFrameNumber == frameNumber) {
//             if (this->aiSaacSettings->getNSFWAnalyticsAlgo() == "CAFFE") {
//                 cv::putText(rawFrame,
//                     this->blobContainer[i].label,
//                     cv::Point(20, 20 * (i + 1)),
//                     cv::FONT_HERSHEY_PLAIN,
//                     1.0,
//                     CV_RGB(124, 252, 0),
//                     2.0);
//             }
//         }
//     }
// }

std::string aiSaac::NSFWAnalytics::runAlgo(const cv::Mat &rawFrame) {
    if (this->aiSaacSettings->getNSFWAnalyticsAlgo() == "CAFFE") {
        std::vector<std::pair<std::string, float>> predictions
            = this->caffeClassifier->Classify(rawFrame);
        for (int i = 0; i < predictions.size(); i++) {
            std::cout << "prediction: " << predictions[i].first << " - " << predictions[i].second;
            // if (predictions[i].second >=
            //     this->aiSaacSettings->getNSFWAnalyticsConfidenceThreshold()) {
            //     std::string nsfwLabel;
            //     nsfwLabel = predictions[i].first;
            //     return nsfwLabel;
            // }
        }
    }
    return "Unknown";
}

void aiSaac::NSFWAnalytics::loadAlgorithmType() {
    if (this->aiSaacSettings->getNSFWAnalyticsAlgo() == "CAFFE") {
        this->caffeClassifier = new CaffeClassifier(
            this->aiSaacSettings->getNSFWCaffePrototxtPath(),
            this->aiSaacSettings->getNSFWCaffeCaffemodelPath(),
            "",
            this->aiSaacSettings->getNSFWCaffeLabelFilePath());
    }
}
