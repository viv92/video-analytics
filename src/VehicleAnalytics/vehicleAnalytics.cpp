/*
    Copyright 2016 AITOE
*/

#include "vehicleAnalytics.h"

aiSaac::VehicleAnalytics::VehicleAnalytics(AiSaacSettings *aiSaacSettings) {
    this->aiSaacSettings = aiSaacSettings;
    this->initialize();
}

aiSaac::VehicleAnalytics::~VehicleAnalytics() {}

void aiSaac::VehicleAnalytics::initialize() {
    this->analyzedFrameNumber = 0;
    this->ID = 0;
    // this->localizedRectangles.clear();
    this->blobContainer.clear();
    this->loadAlgorithmType();
    std::cout << "loaded and initialized vehicleAnalytics" << std::endl;
}

int aiSaac::VehicleAnalytics::detect(const cv::Mat &rawFrame,
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

void aiSaac::VehicleAnalytics::localize(const cv::Mat &rawFrame,
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

void aiSaac::VehicleAnalytics::track(const cv::Mat &rawFrame,
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

    // Create new VehicleBlob
    for (int i = 0; i < localizedRectangles.size(); i++) {
        if (!rectTaken[i]) {
            aiSaac::VehicleBlob newVehicleBlob;
            this->ID++;
            newVehicleBlob.ID = ID;
            newVehicleBlob.frameCount = 1;
            newVehicleBlob.firstFrameNumber = frameNumber;
            newVehicleBlob.lastFrameNumber = frameNumber;
            newVehicleBlob.firstRectangle = localizedRectangles[i];
            newVehicleBlob.lastRectangle = localizedRectangles[i];
            cv::Point position;
            position.x = newVehicleBlob.lastRectangle.x + newVehicleBlob.lastRectangle.width / 2;
            position.y = newVehicleBlob.lastRectangle.y + newVehicleBlob.lastRectangle.height / 2;
            newVehicleBlob.pastPositions.push_back(position);

            this->blobContainer.push_back(newVehicleBlob);
            // std::cout << "Added a new blob.. " << this->blobContainer.size() << std::endl;
        }
    }
}

void aiSaac::VehicleAnalytics::annotate(cv::Mat &rawFrame) {
    this->annotate(rawFrame, this->analyzedFrameNumber);
}

void aiSaac::VehicleAnalytics::annotate(cv::Mat &rawFrame, int frameNumber) {
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

void aiSaac::VehicleAnalytics::loadAlgorithmType() {
    std::string vehicleAlgo = this->aiSaacSettings->getVehicleAnalyticsAlgo();
    std::cout << "Vehicle Analytics Algo: " << vehicleAlgo << std::endl;
    if (vehicleAlgo == "HAAR") {
        if (!this->vehicleCascadeClassifier.load(this->aiSaacSettings->getHaarCascadeFilePath()
            + "cars.xml")) {
            std::cout << "Sorry cascade not found at: "
                << this->aiSaacSettings->getHaarCascadeFilePath() + "cars.xml"
                << std::endl;
        }
    }
}

void aiSaac::VehicleAnalytics::runAlgo(const cv::Mat &rawFrame,
    int frameNumber,
    std::vector<cv::Rect> &localizedRectangles) {
    std::string vehicleAlgo = this->aiSaacSettings->getVehicleAnalyticsAlgo();
    std::vector<int> minSize = this->aiSaacSettings->getMinimumVehicleSize();
    std::vector<int> maxSize = this->aiSaacSettings->getMaximumVehicleSize();

    localizedRectangles.clear();
    if (vehicleAlgo == "HAAR") {
        this->vehicleCascadeClassifier.detectMultiScale(rawFrame,
            localizedRectangles,
            1.1,  // algo param
            4,  // algo param
            CV_HAAR_SCALE_IMAGE,
            cv::Size(minSize[0], minSize[1]),
            cv::Size(maxSize[0], maxSize[1]));
    }

    this->analyzedFrameNumber = frameNumber;
}

void aiSaac::VehicleAnalytics::preProcessBlobContainer(int frameNumber) {
    if (this->blobContainer.size() > 400) {  //add to aiSaac settings
        this->blobContainer.erase(this->blobContainer.begin(), this->blobContainer.begin() + 200);
    }

    // Remove old objects with few frames
    for (std::vector<aiSaac::VehicleBlob>::iterator it = this->blobContainer.begin();
        it != this->blobContainer.end();) {
        if (frameNumber - it->lastFrameNumber > this->aiSaacSettings->getVehicleBlobDeleteTime()
            && it->frames.size() < this->aiSaacSettings->getMinimumVehicleBlobFrames()) {
            it = this->blobContainer.erase(it);
        } else {
            ++it;
        }
    }
}
