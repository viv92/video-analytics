/*
    Copyright 2016 AITOE
*/

#include "sceneAnalytics.h"

aiSaac::SceneAnalytics::SceneAnalytics(aiSaac::AiSaacSettings &aiSaacSettings) : aiSaacSettings(aiSaacSettings) {
    // this->aiSaacSettings = aiSaacSettings;
    this->initialize();
}

aiSaac::SceneAnalytics::SceneAnalytics(aiSaac::AiSaacSettings &aiSaacSettings, aiSaac::CaffeClassifier &caffeClassifier) : aiSaacSettings(aiSaacSettings) {
    // this->aiSaacSettings = aiSaacSettings;
    this->initialize(caffeClassifier);
}

aiSaac::SceneAnalytics::~SceneAnalytics() {
    this->blobContainer.clear();
}

void aiSaac::SceneAnalytics::initialize() {
    this->analyzedFrameNumber = 0;
    this->ID = 0;
    this->blobContainer.clear();
    // this->caffeClassifier = &caffeClassifier;
    this->loadAlgorithmType();
    std::cout << "loaded and initialized sceneAnalytics" << std::endl;
}


void aiSaac::SceneAnalytics::initialize(aiSaac::CaffeClassifier &caffeClassifier) {
    this->analyzedFrameNumber = 0;
    this->ID = 0;
    this->blobContainer.clear();
    this->caffeClassifier = &caffeClassifier;
    // this->loadAlgorithmType();
    std::cout << "loaded and initialized sceneAnalytics" << std::endl;
}

std::vector<std::string> aiSaac::SceneAnalytics::detect(const cv::Mat &rawFrame,
    int frameNumber,
    cv::Rect areaOfInterest) {
    std::vector<aiSaac::SceneBlob> detectedScenes;

    if (areaOfInterest.area() != 0) {
        cv::Mat subRawFrame = rawFrame(areaOfInterest);
        this->runAlgo(subRawFrame, frameNumber, detectedScenes);
    } else {
        this->runAlgo(rawFrame, frameNumber, detectedScenes);
    }

    std::vector<std::string> predictionLabels;
    for (int i = 0; i < detectedScenes.size(); i++) {
        std::cout << "DetectedSceneLabel: " << detectedScenes[i].label << std::endl;
        predictionLabels.push_back(detectedScenes[i].label);
    }
    return predictionLabels;
}

void aiSaac::SceneAnalytics::track(const cv::Mat &rawFrame,
    int frameNumber,
    cv::Rect areaOfInterest) {
    this->preProcessBlobContainer(frameNumber);

    std::vector<aiSaac::SceneBlob> detectedScenes;
    if (areaOfInterest.area() != 0) {
        cv::Mat subRawFrame = rawFrame(areaOfInterest);
        this->runAlgo(subRawFrame, frameNumber, detectedScenes);
    } else {
        this->runAlgo(rawFrame, frameNumber, detectedScenes);
    }

    std::vector<int> sceneTaken(detectedScenes.size(), 0);
    for (int i = 0; i < this->blobContainer.size(); i++) {
        for (int j = 0; j < detectedScenes.size(); j++) {
            if (sceneTaken[j] == 1) {
                continue;
            }

            if (this->blobContainer[i].label == detectedScenes[j].label) {
                sceneTaken[j] = 1;
                this->blobContainer[i].lastFrameNumber = frameNumber;
                this->blobContainer[i].frameCount++;
                // time(&this->blobContainer[i].endTime);
                std::cout << "Appended existing blob" << std::endl;
                break;
            }
        }
    }

    // Create new SceneBlob
    for (int i = 0; i < detectedScenes.size(); i++) {
        if (!sceneTaken[i]) {
            aiSaac::SceneBlob newSceneBlob;
            this->ID++;
            newSceneBlob.ID = ID;
            newSceneBlob.frameCount = 1;
            newSceneBlob.label = detectedScenes[i].label;
            newSceneBlob.firstFrameNumber = frameNumber;
            newSceneBlob.lastFrameNumber = frameNumber;
            std::cout << "The detected label here is " << newSceneBlob.label << "\n" << std::flush;
            this->blobContainer.push_back(newSceneBlob);
            // std::cout << "Added a new blob.. " << this->blobContainer.size() << std::endl;
        }
    }
}

void aiSaac::SceneAnalytics::annotate(cv::Mat &rawFrame) {
    this->annotate(rawFrame, this->analyzedFrameNumber);
}

void aiSaac::SceneAnalytics::annotate(cv::Mat &rawFrame, int frameNumber) {
    int count = 0;
    for (int i = 0; i < this->blobContainer.size(); i++) {
        if (this->blobContainer[i].lastFrameNumber == frameNumber) {
            if (this->aiSaacSettings.getSceneAnalyticsAlgo() == "CAFFE") {
                int baseline = 0;
                int fontScale = ceil(rawFrame.cols * 0.001);
                int thickness = ceil(rawFrame.cols * 0.002);

                cv::Size textSize = cv::getTextSize(this->blobContainer[i].label,
                     cv::FONT_HERSHEY_SIMPLEX,
                    fontScale,
                    thickness,
                    &baseline);

                cv::putText(rawFrame,
                    this->blobContainer[i].label,
                    cv::Point(30 , textSize.height * 2 * (count + 1)),
                    cv::FONT_HERSHEY_SIMPLEX,
                    fontScale,
                    cv::Scalar(127, 255, 0),
                    thickness);
                count++;
            }
        }
    }
}

void aiSaac::SceneAnalytics::annotate(cv::Mat &rawFrame, std::vector<std::string>& labels) {
    for (int i = 0; i < labels.size(); i++) {
        cv::putText(rawFrame,
            labels[i],
            cv::Point(20, 20 * (i + 1)),
            cv::FONT_HERSHEY_PLAIN,
            1.0,
            CV_RGB(124, 252, 0),
            2.0);
    }
}

void aiSaac::SceneAnalytics::runAlgo(const cv::Mat &rawFrame,
    int frameNumber,
    std::vector<aiSaac::SceneBlob> &detectedScenes) {
    if (this->aiSaacSettings.getSceneAnalyticsAlgo() == "CAFFE") {
        std::vector<std::pair<std::string, float>> predictions
            = this->caffeClassifier->Classify(rawFrame);

        for (int i = 0; i < predictions.size(); i++) {
            if (predictions[i].second >=
                this->aiSaacSettings.getSceneAnalyticsConfidenceThreshold()) {
                aiSaac::SceneBlob newScene;
                newScene.ID = i;
                newScene.label = predictions[i].first;
                newScene.firstFrameNumber = frameNumber;
                newScene.lastFrameNumber = frameNumber;
                newScene.frameCount = 1;
                detectedScenes.push_back(newScene);
            }
        }
    }
    this->analyzedFrameNumber = frameNumber;
}

void aiSaac::SceneAnalytics::preProcessBlobContainer(int frameNumber) {
    if (this->blobContainer.size() > 400) {  //add to aiSaac settings
        this->blobContainer.erase(this->blobContainer.begin(), this->blobContainer.begin() + 200);
    }

    // Remove old objects with few frames
    for (std::vector<aiSaac::SceneBlob>::iterator it = this->blobContainer.begin();
        it != this->blobContainer.end();) {
        if (frameNumber - it->lastFrameNumber
            > this->aiSaacSettings.getSceneBlobDeleteTime()
            && it->frames.size() < this->aiSaacSettings.getMinimumSceneBlobFrames()) {
            it = this->blobContainer.erase(it);
        } else {
            it++;
        }
    }
}

void aiSaac::SceneAnalytics::loadAlgorithmType() {
    std::cout << "loading scene algo" << std::endl;
    if (this->aiSaacSettings.getSceneAnalyticsAlgo() == "CAFFE") {
        this->caffeClassifier = new CaffeClassifier(
            this->aiSaacSettings.getScenePrototxtPath(),
            this->aiSaacSettings.getSceneCaffemodelPath(),
            this->aiSaacSettings.getSceneMeanFilePath(),
            this->aiSaacSettings.getSceneLabelFilePath());
    }
}
