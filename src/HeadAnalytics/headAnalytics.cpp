/*
    Copyright 2016 AITOE
*/

/*
    Todo:
    1. Add all algorithm parameters in runAlgo() to aiSaacSettings
*/

#include "headAnalytics.h"

aiSaac::HeadAnalytics::HeadAnalytics(AiSaacSettings *aiSaacSettings) {
    this->aiSaacSettings = aiSaacSettings;
    this->initialize();
}

aiSaac::HeadAnalytics::~HeadAnalytics() {}

void aiSaac::HeadAnalytics::initialize() {
    this->analyzedFrameNumber = 0;
    this->ID = 0;
    // this->localizedRectangles.clear();
    this->blobContainer.clear();
    this->loadAlgorithmType();
    std::cout << "loaded and initialized headAnalytics" << std::endl;
}

int aiSaac::HeadAnalytics::detect(const cv::Mat &rawFrame,
    int frameNumber,
    cv::Rect areaOfInterest) {
    std::vector<cv::Rect> localizedRectangles;

    if (areaOfInterest.area() != 0) {
        cv::Mat subRawFrame = rawFrame(areaOfInterest);
        this->runAlgo(subRawFrame, frameNumber, localizedRectangles);
    } else {
        this->runAlgo(rawFrame, frameNumber, localizedRectangles);
    }

    return localizedRectangles.size();
}

void aiSaac::HeadAnalytics::localize(const cv::Mat &rawFrame,
    int frameNumber,
    std::vector<cv::Rect> &localizedRectangles,
    cv::Rect areaOfInterest) {
    if (areaOfInterest.area() != 0) {
        cv::Mat subRawFrame = rawFrame(areaOfInterest);
        this->runAlgo(subRawFrame, frameNumber, localizedRectangles);
        for (int i = 0; i < localizedRectangles.size(); i++) {
            localizedRectangles[i].x += areaOfInterest.x;
            localizedRectangles[i].y += areaOfInterest.y;
        }
    } else {
        this->runAlgo(rawFrame, frameNumber, localizedRectangles);
    }
}

void aiSaac::HeadAnalytics::track(const cv::Mat &rawFrame,
    int frameNumber,
    cv::Rect areaOfInterest) {
    this->preProcessBlobContainer(frameNumber);
    std::vector<cv::Rect> localizedRectangles;
    this->localize(rawFrame, frameNumber, localizedRectangles, areaOfInterest);

    std::vector<int> rectTaken(localizedRectangles.size(), 0);
    for (int i = 0; i < this->blobContainer.size(); i++) {
        cv::Point latestPosition = this->blobContainer[i].pastPositions.back();
        for (int j = 0; j < localizedRectangles.size(); j++) {
            if (rectTaken[j] == 1) {
                continue;
            }
            cv::Point potentialPosition(localizedRectangles[j].x
                + localizedRectangles[j].width/2,
                localizedRectangles[j].y + localizedRectangles[j].height/2);

            if ((abs(potentialPosition.x - latestPosition.x) < (rawFrame.size().width / 3))
                && (abs(potentialPosition.y - latestPosition.y) < (rawFrame.size().height / 3))) {
                rectTaken[j] = 1;
                this->blobContainer[i].lastFrameNumber = frameNumber;
                this->blobContainer[i].frameCount++;
                this->blobContainer[i].lastRectangle = localizedRectangles[j];

                if (this->blobContainer[i].pastPositions.size() == 50) {
                    this->blobContainer[i].pastPositions.erase(
                        this->blobContainer[i].pastPositions.begin());
                }
                cv::Point position;
                this->blobContainer[i].pastPositions.push_back(potentialPosition);
                // time(&this->blobContainer[i].endTime);
                std::cout << "Appended existing blob" << std::endl;
                break;
            }
        }
    }

    // Create new HeadBlob
    for (int i = 0; i < localizedRectangles.size(); i++) {
        if (!rectTaken[i]) {
            aiSaac::HeadBlob newHeadBlob;
            this->ID++;
            newHeadBlob.ID = ID;
            newHeadBlob.frameCount = 1;
            newHeadBlob.firstFrameNumber = frameNumber;
            newHeadBlob.lastFrameNumber = frameNumber;
            newHeadBlob.firstRectangle = localizedRectangles[i];
            newHeadBlob.lastRectangle = localizedRectangles[i];
            cv::Point position;
            position.x = newHeadBlob.lastRectangle.x + newHeadBlob.lastRectangle.width / 2;
            position.y = newHeadBlob.lastRectangle.y + newHeadBlob.lastRectangle.height /2;
            newHeadBlob.pastPositions.push_back(position);

            this->blobContainer.push_back(newHeadBlob);
            // std::cout << "Added a new blob.. " << this->blobContainer.size() << std::endl;
        }
    }
}

void aiSaac::HeadAnalytics::annotate(cv::Mat &rawFrame) {
    this->annotate(rawFrame, this->analyzedFrameNumber);
}

void aiSaac::HeadAnalytics::annotate(cv::Mat &rawFrame, int frameNumber) {
    for (int i = 0; i < this->blobContainer.size(); i++) {
        if (this->blobContainer[i].lastFrameNumber == frameNumber) {
            cv::rectangle(rawFrame,
                this->blobContainer[i].lastRectangle,
                cv::Scalar(255, 255, 255), 2);

            // for (int j = 1; j < blobContainer[i].pastPositions.size(); j++) {
            //     cv::Point pointOne((this->blobContainer[i].pastPositions[j - 1].x),
            //         (this->blobContainer[i].pastPositions[j - 1].y));
            //     cv::Point pointTwo((this->blobContainer[i].pastPositions[j].x),
            //         (blobContainer[i].pastPositions[j].y));
            //     cv::line(rawFrame, pointOne, pointTwo, cv::Scalar(0, 0, 255), 1, 8);
            // }
        }
    }
}

void aiSaac::HeadAnalytics::loadAlgorithmType() {
    std::string headAlgo = this->aiSaacSettings->getHeadAnalyticsAlgo();
    std::cout << "Head Analytics Algo: " << headAlgo << std::endl;
    if (headAlgo == "HAAR") {
        if (!this->headCascadeClassifier.load(this->aiSaacSettings->getHaarCascadeFilePath()
            + "haarcascade_head.xml")) {
            std::cout << "Sorry, cascade not found at: "
                << this->aiSaacSettings->getHaarCascadeFilePath() + "haarcascade_head.xml"
                << std::endl;
        }
    }
}

void aiSaac::HeadAnalytics::runAlgo(const cv::Mat &rawFrame,
    int frameNumber,
    std::vector<cv::Rect> &localizedRectangles) {
    std::string headAlgo = this->aiSaacSettings->getHeadAnalyticsAlgo();
    std::vector<double> minSize = this->aiSaacSettings->getMinimumHeadSize();
    std::vector<double> maxSize = this->aiSaacSettings->getMaximumHeadSize();
    double scaleFactor = this->aiSaacSettings->getHeadScaleFactor();
    int minNeighbours = this->aiSaacSettings->getHeadMinNeighbours();

    localizedRectangles.clear();
    if (headAlgo == "HAAR") {
        this->headCascadeClassifier.detectMultiScale(rawFrame,
            localizedRectangles,
            scaleFactor,  // algo param
            minNeighbours,  // algo param
            CV_HAAR_SCALE_IMAGE,
            cv::Size(minSize[0] * rawFrame.cols, minSize[1] * rawFrame.rows),
            cv::Size(maxSize[0] * rawFrame.cols, maxSize[1] * rawFrame.rows));
    }

    this->analyzedFrameNumber = frameNumber;
}

void aiSaac::HeadAnalytics::preProcessBlobContainer(int frameNumber) {
    if (this->blobContainer.size() > 400) {  //add to aiSaac settings
        this->blobContainer.erase(this->blobContainer.begin(), this->blobContainer.begin() + 200);
    }

    // Remove old objects with few frames
    for (std::vector<aiSaac::HeadBlob>::iterator it = this->blobContainer.begin();
        it != this->blobContainer.end();) {
        if (frameNumber - it->lastFrameNumber > this->aiSaacSettings->getHeadBlobDeleteTime()
            && it->frames.size() < this->aiSaacSettings->getMinimumHeadBlobFrames()) {
            it = this->blobContainer.erase(it);
        } else {
            ++it;
        }
    }
}
