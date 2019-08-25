/*
    Copyright 2016 AITOE
*/

/*
 *  TODO
 * 1) Add PBAS algorithm
*/
#include "bgSubAnalytics.h"

aiSaac::BgSubAnalytics::BgSubAnalytics(AiSaacSettings &aiSaacSettings): aiSaacSettings(aiSaacSettings) {
    std::cout << "Inside bgConstructor" << std::endl;
    this->initialize();
}

aiSaac::BgSubAnalytics::~BgSubAnalytics() {}

void aiSaac::BgSubAnalytics::initialize() {
    this->analyzedFrameNumber = 0;
    this->lastBGFrameRecorded = 0;
    this->noMotionFrameCount = 0;
    this->ID = 0;
    this->backgroundFrame = cv::Mat();
    this->blobContainer.clear();
    this->loadAlgorithmType();
    std::cout << "loaded and initialized bgSubAnalytics" << std::endl;
}

bool aiSaac::BgSubAnalytics::detect(const cv::Mat &rawFrame, int frameNumber) {
    if (frameNumber <= this->analyzedFrameNumber) {
        return this->isMotion;
    }

    this->runAlgo(rawFrame);

    cv::Rect boundingRectangle;
    this->isMotion = false;
    int minAreaThreshold = rawFrame.rows * rawFrame.cols
        * this->aiSaacSettings.getMinimumWindowFraction();
    int maxAreaThreshold = rawFrame.rows * rawFrame.cols
        *  this->aiSaacSettings.getMaximumWindowFraction();

    for (int i = 0; i < this->contours.size(); i++) {
        // Obtain ROI from bounding rectangle of contours
        boundingRectangle = cv::boundingRect(this->contours[i]);
        /* Code review make sure that the blob is not the entire frame */
        if (boundingRectangle.area() >= minAreaThreshold
            && boundingRectangle.area() <= maxAreaThreshold) {
            this->isMotion = true;
            break;
        } else {
            this->contours.erase(this->contours.begin() + i);
            i--;
        }
    }

    if (this->isMotion) {
        this->noMotionFrameCount = 0;
    } else {
        this->noMotionFrameCount++;
        if (this->noMotionFrameCount > (int) this->aiSaacSettings.getNoMotionBackgroundUpdateTime()) {
            this->lastBGFrameRecorded = frameNumber;
            this->backgroundFrame = rawFrame.clone();
        }
    }

    int diff = this->analyzedFrameNumber - this->lastBGFrameRecorded;
    if (diff > (int) this->aiSaacSettings.getBackgroundUpdateTime()) {
        this->lastBGFrameRecorded = frameNumber;
        this->backgroundFrame = rawFrame.clone();
    }

    this->analyzedFrameNumber = frameNumber;
    return this->isMotion;
}

void aiSaac::BgSubAnalytics::localize(const cv::Mat &rawFrame,
    int frameNumber,
    std::vector<cv::Rect> &localizedRectangles) {
    if (!this->detect(rawFrame, frameNumber)) {
        localizedRectangles.clear();
        return;
    }

    cv::Rect boundingRectangle;
    int minAreaThreshold = rawFrame.rows * rawFrame.cols
        * this->aiSaacSettings.getMinimumWindowFraction();
    int maxAreaThreshold = rawFrame.rows * rawFrame.cols
        * this->aiSaacSettings.getMaximumWindowFraction();

    for (int i = 0; i < this->contours.size(); i++) {
        boundingRectangle = cv::boundingRect(this->contours[i]);
        /* Code review make sure that the blob is not the entire frame */
        if (boundingRectangle.area() >= minAreaThreshold
            && boundingRectangle.area() <= maxAreaThreshold) {
            localizedRectangles.push_back(boundingRectangle);
        } else {
            this->contours.erase(this->contours.begin() + i);
            i--;
        }
    }

    // std::cout << "Got a bunch of localizedRectangles"
    // << localizedRectangles.size() << std::endl;
}

void aiSaac::BgSubAnalytics::track(
    const cv::Mat &rawFrame,
    int frameNumber) {
    std::vector<cv::Rect> localizedRectangles;
    this->localize(rawFrame, frameNumber, localizedRectangles);
    this->preProcessBlobContainer(frameNumber);

    if (!localizedRectangles.size()) {
        return;
    }

    // Detect collisions and append object to object containers
    for (int i = 0; i < this->blobContainer.size(); i++) {
        // Clean contact contours
        // blobContainer[i].contactContours.clear();
        blobContainer[i].contactRectsIndex.clear();
        blobContainer[i].collision = 0;
        // Loop contours
        for (int j = 0; j < localizedRectangles.size(); j++) {
            // Get distance
            float distance =
                sqrt(pow((this->blobContainer[i].lastRectangle.x
                + this->blobContainer[i].lastRectangle.width / 2.0)
                - (localizedRectangles[j].x
                + localizedRectangles[j].width / 2.0), 2.0)
                + pow((this->blobContainer[i].lastRectangle.y
                + this->blobContainer[i].lastRectangle.height / 2.0)
                - (localizedRectangles[j].y
                + localizedRectangles[j].height / 2.0), 2.0));

            // Detect collisions
            if (distance < min(this->blobContainer[i].lastRectangle.width,
                this->blobContainer[i].lastRectangle.height)
                && frameNumber - this->blobContainer[i].lastFrameNumber < 60
                && max(localizedRectangles[j].width,
                    localizedRectangles[j].height)
                > max(rawFrame.cols, rawFrame.rows) / 10) {
                    /*Code review. What is 30 and 3?*/
                this->blobContainer[i].contactRectsIndex.push_back(j);
                /*Code review: What is contactCountours?*/
            }
        }
    }

    std::vector<int> rectTaken(localizedRectangles.size(), 0);

    for (int i = 0; i < this->blobContainer.size(); i++) {
        int maxArea = 0;
        int selectedContactRect = -1;
        for (unsigned int j = 0;
            j < this->blobContainer[i].contactRectsIndex.size();
            j++) {
            int contactRectsIndex = this->blobContainer[i].contactRectsIndex[j];
            //if (rectTaken[contactRectsIndex] == 1)
              //continue;
            // int contourArea = cv::boundingRect(contours[coi]).width*cv::boundingRect(contours[coi]).height;
            int rectArea = localizedRectangles[contactRectsIndex].area();
            if ((rectArea > maxArea) && (rectArea < rawFrame.rows * rawFrame.cols)) {
                maxArea = rectArea;
                selectedContactRect = contactRectsIndex;
            }
        }
        // blobContainer[bli].contactContours.clear();
        // Append blob with largest area

        if (selectedContactRect != -1 && rectTaken[selectedContactRect] == 0) {
            rectTaken[selectedContactRect] = 1;
            //this->blobContainer[i].contactRectsIndex.push_back(selectedContactRect);

            // Get contour properties
            // boundingRectangle = cv::boundingRect(contours[selectedContactRect]);
            // roiFrameMask = foregroundFrameBuffer(localizedRectangles[selectedContactRect]).clone();
            // roiFrame = foregroundFrameBuffer(localizedRectangles[selectedContactRect]).clone();
            // roiFrameBuffer = rawFrame(localizedRectangles[selectedContactRect]).clone();
            // roiFrameBuffer.copyTo(roiFrame, roiFrameMask);

            // Append objects
            this->blobContainer[i].frameCount++;
            if (this->blobContainer[i].frameCount == this->aiSaacSettings.getMinimumBlobFrames()) {
                this->blobContainer[i].isValid = true;
            }
            this->blobContainer[i].lastFrameNumber = frameNumber;
            this->blobContainer[i].lastRectangle =
                    localizedRectangles[selectedContactRect];
            if (this->blobContainer[i].pastPositions.size() == 50) {
                this->blobContainer[i].pastPositions.erase(
                    this->blobContainer[i].pastPositions.begin());
            }
            cv::Point position;
            position.x = blobContainer[i].lastRectangle.x
                + this->blobContainer[i].lastRectangle.width / 2;
            position.y = blobContainer[i].lastRectangle.y
                + this->blobContainer[i].lastRectangle.height / 2;
            this->blobContainer[i].pastPositions.push_back(position);
            time(&this->blobContainer[i].endTime);
            // blobContainer[i].avgWidth = .8 * blobContainer[i].avgWidth + .2*roiFrame.size().width;
            // blobContainer[i].avgHeight = .8*blobContainer[i].avgHeight + .2*roiFrame.size().height;
            // blobContainer[i].maxWidth = max(blobContainer[i].maxWidth,roiFrame.size().width);
            // blobContainer[i].maxHeight = max(blobContainer[i].maxHeight,roiFrame.size().height);
        }

        if (this->blobContainer[i].contactRectsIndex.size() > 1) {
            this->blobContainer[i].collision = 1;
        }
    }

    // Create objects for the rest of the contours
    // std::cout << "About to add contours ..." << std::endl;
    for (int i = 0; i < localizedRectangles.size(); i++) {
        // boundingRectangle = cv::boundingRect(contours[coi]);
        // cout << "Bounding rectangle width = " << boundingRectangle.width << " height = " << boundingRectangle.height << endl;
        if (!rectTaken[i] &&
                max(localizedRectangles[i].width,
                localizedRectangles[i].height)
            > max(rawFrame.cols, rawFrame.rows) / 10) {
            // cout << "Qualified to get added" << endl;
            // Get contour properties
            // boundingRectangle = cv::boundingRect(contours[coi]);
            // roiFrameMask =  foregroundFrameBuffer(boundingRectangle).clone();
            // roiFrame = foregroundFrameBuffer(boundingRectangle).clone();
            // roiFrameBuffer = rawFrame(boundingRectangle).clone();
            // roiFrameBuffer.copyTo(roiFrame, roiFrameMask);

            // Create objects
            // add rectangle only if it is big egnough
            // cout << "Is it big enough? Area = " << boundingRectangle.area() << " minWindowFrac = " << minWindowFrac << " maxWindowFrac = " << maxWindowFrac << endl;
            // if ((localizedRectangles[i].area() >= rawFrame.rows * rawFrame.cols * (double)advanceSettings->settings["advanceSettings"]["cameraTamperingThreshold"])
            //         ||((boundingRectangle.area() >= rawFrame.rows*rawFrame.cols*((double)advanceSettings->settings["advanceSettings"]["minimumWindowFraction"]))
            //            && (boundingRectangle.area() <= rawFrame.rows*rawFrame.cols*((double)advanceSettings->settings["advanceSettings"]["maximumWindowFraction"])))){
                // cout << "Made it finally" << endl;
            ID++;
            BlobObject newObject;
            newObject.ID = ID;
            newObject.frameCount = 1;
            newObject.firstFrameNumber = frameNumber;
            newObject.lastFrameNumber = frameNumber;
            newObject.lastHumanFrameNumber = frameNumber;
            newObject.firstRectangle = localizedRectangles[i];
            newObject.lastRectangle = localizedRectangles[i];
            cv::Point position;
            position.x = newObject.lastRectangle.x +
                    newObject.lastRectangle.width / 2;
            position.y = newObject.lastRectangle.y +
                    newObject.lastRectangle.height /2;
            newObject.pastPositions.push_back(position);
            // newObject.avgWidth = roiFrame.size().width;
            // newObject.avgHeight = roiFrame.size().height;
            // newObject.maxWidth = roiFrame.size().width;
            // newObject.maxHeight = roiFrame.size().height;
            newObject.humanDetCount = 0;
            newObject.histDetCount = 0;
            // newObject.objLoiteringNotified = false;
            // newObject.objLoiteringNotifiedCount = 0;
            // newObject.camTamperingNotified = false;
            // newObject.objEntryNotified = false;
            // newObject.humanLoiteringNotified = false;
            // newObject.humanLoiteringNotifiedCount = 0;
            // newObject.humanEntryNotified = false;
            // newObject.assetTamperingNotified=false;
            time(&newObject.startTime);
            time(&newObject.endTime);
            newObject.isCounted = 0;
            newObject.currTime = 0;
            // newObject.objEntryRecorded = false;
            // newObject.objLoiteringRecorded = false;
            // newObject.camTamperingRecorded = false;
            // newObject.humanEntryRecorded = false;
            // newObject.humanLoiteringRecorded = false;
            // newObject.assetTamperingRecorded = false;
            newObject.isValid = false;
            this->blobContainer.push_back(newObject);
            // }
        }
    }
}

void aiSaac::BgSubAnalytics::annotate(cv::Mat &rawFrame) {
    this->annotate(rawFrame, this->analyzedFrameNumber);
}

void aiSaac::BgSubAnalytics::annotate(cv::Mat &rawFrame, int frameNumber) {
    for (int i = 0; i < this->blobContainer.size(); i++) {
        if (this->blobContainer[i].lastFrameNumber == frameNumber && this->blobContainer[i].isValid) {
            cv::rectangle(rawFrame,
                this->blobContainer[i].lastRectangle,
                cv::Scalar(0, 0, 255), 1);

            int baseline = 0;
            int fontScale = ceil(rawFrame.cols * 0.001);
            //int fontScale = 1;
            int thickness = ceil(rawFrame.cols * 0.002);

            cv::Size textSize = cv::getTextSize(std::to_string(this->blobContainer[i].ID),
                 cv::FONT_HERSHEY_PLAIN,
                fontScale,
                thickness,
                &baseline);
            baseline += thickness;
            cv::Point textOrg(this->blobContainer[i].lastRectangle.x
                + (this->blobContainer[i].lastRectangle.width / 2)
                - (textSize.width / 2),
                this->blobContainer[i].lastRectangle.y - 5);
            if (textOrg.x < 0) {
                textOrg.x = 0;
            }
            if (textOrg.y < 0) {
                textOrg.y = 0;
            }
            cv::putText(rawFrame,
                std::to_string(this->blobContainer[i].ID),
                textOrg,
                cv::FONT_HERSHEY_PLAIN,
                fontScale,
                cv::Scalar(0, 0, 255),
                thickness);

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

// const std::vector<aiSaac::BlobObject>& aiSaac::BgSubAnalytics::getBlobContainer() {
//     return this->blobContainer;
// }

bool aiSaac::BgSubAnalytics::getMotion() {
    return this->isMotion;
}

void aiSaac::BgSubAnalytics::loadAlgorithmType() {
    std::string bgAlgo = this->aiSaacSettings.getBackgroundSubtractionAlgo();
    if (bgAlgo == "MOG") {
        std::cout << "BGSUB Algo: MOG" << std::endl;
        this->pMOG = createBackgroundSubtractorMOG();
        this->pMOG->setNMixtures(2);
    } else if (bgAlgo == "MOG2") {
        std::cout << "BGSUB Algo: MOG2" << std::endl;
        this->pMOG2 = createBackgroundSubtractorMOG2();
        this->pMOG2->setDetectShadows(0);
        this->pMOG2->setNMixtures(2);
    } else if (bgAlgo == "GMG") {
        std::cout << "BGSUB Algo: GMG" << std::endl;
        this->pGMG = createBackgroundSubtractorGMG();
    } else if (bgAlgo == "VIBEBG") {
        // std::cout << "BGSUB Algo: vibe" << std::endl;
        // this->firstFrame = true;
        // this->vibe = VIBE(3, 20, 4, 17, 2, 16);
        std::cout <<"Disabled VIBE due to OpenCV3 Migration" << std::endl;
    } else {
         std::cout << "BGSUB Algo: MLBGS" << std::endl;
         this->bgs.firstTime = true;
         this->bgs.frameNumber = 0;
        //std::cout <<"Disabled MLBGS due to OpenCV3 Migration" << std::endl;
    }
}

void aiSaac::BgSubAnalytics::runAlgo(const cv::Mat &rawFrame) {
    std::string bgAlgo = this->aiSaacSettings.getBackgroundSubtractionAlgo();

    if (bgAlgo == "MOG") {
        this->pMOG->apply(rawFrame, this->foregroundFrame, -1);
    } else if (bgAlgo == "MOG2") {
        this->pMOG2->apply(rawFrame, this->foregroundFrame, -1);
        cv::threshold(this->foregroundFrame,
                this->foregroundFrame, 130, 255, cv::THRESH_BINARY);
        cv::medianBlur(this->foregroundFrame, this->foregroundFrame, 3);
        cv::erode(this->foregroundFrame, this->foregroundFrame, cv::Mat());
        cv::dilate(this->foregroundFrame, this->foregroundFrame, cv::Mat());
    } else if (bgAlgo == "GMG") {
        pGMG->apply(rawFrame, this->foregroundFrame, -1);
        cv::threshold(this->foregroundFrame,
                this->foregroundFrame, 130, 255, cv::THRESH_BINARY);
        cv::medianBlur(this->foregroundFrame, this->foregroundFrame, 3);
        cv::erode(this->foregroundFrame, this->foregroundFrame, cv::Mat());
        cv::dilate(this->foregroundFrame, this->foregroundFrame, cv::Mat());
    } else if (bgAlgo == "VIBEBG") {
        // if (this->firstFrame) {
        //     // Instantiation of ViBe.
        //     this->vibe.init(rawFrame);
        //     this->firstFrame = false;
        // }
        // // Segmentation and update.
        // this->vibe.update(rawFrame);
        // this->foregroundFrame = this->vibe.getMask().clone();
        // cv::threshold(this->foregroundFrame,
        //         this->foregroundFrame, 130, 255, cv::THRESH_BINARY);
        // cv::medianBlur(this->foregroundFrame, this->foregroundFrame, 3);
        // cv::erode(this->foregroundFrame, this->foregroundFrame, cv::Mat());
        // cv::dilate(this->foregroundFrame, this->foregroundFrame, cv::Mat());
        std::cout <<"Disabled VIBE due to OpenCV3 Migration" << std::endl;
    } else {
         cv::Mat bgImg;
         this->bgs.process(rawFrame, this->foregroundFrame, bgImg);
         cv::threshold(this->foregroundFrame,
                 this->foregroundFrame, 130, 255, cv::THRESH_BINARY);
         cv::medianBlur(this->foregroundFrame, this->foregroundFrame, 3);
         cv::erode(this->foregroundFrame, this->foregroundFrame, cv::Mat());
         cv::dilate(this->foregroundFrame, this->foregroundFrame, cv::Mat());
        //std::cout <<"Disabled MLBGS due to OpenCV3 Migration" << std::endl;
    }

    cv::findContours(this->foregroundFrame,
        this->contours,
        this->hierarchy,
        CV_RETR_EXTERNAL,
        CV_CHAIN_APPROX_SIMPLE,
        cv::Point(0, 0));
}

void aiSaac::BgSubAnalytics::preProcessBlobContainer(int frameNumber) {
    if (this->blobContainer.size() > 400) {  // add to aiSaac settings
        this->blobContainer.erase(blobContainer.begin(),
                this->blobContainer.begin() + 200);
    }

    // Remove old objects with few frames
    for (std::vector<BlobObject>::iterator it = this->blobContainer.begin();
        it != this->blobContainer.end();) {
        if (frameNumber - it->lastFrameNumber >
                this->aiSaacSettings.getBlobDeleteTime()
            && it->frames.size() <
                this->aiSaacSettings.getMinimumBlobFrames()) {
            it = blobContainer.erase(it);
        } else {
            ++it;
        }
    }
}
