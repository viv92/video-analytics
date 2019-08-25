/*
    Copyright 2016 AITOE
*/

#include "cmtTracking.h"

aiSaac::CmtTracking::CmtTracking(AiSaacSettings *aiSaacSettings) {
    this->aiSaacSettings = aiSaacSettings;
    this->initialize();
}

void aiSaac::CmtTracking::initialize() {
    this->currentRect = cv::Rect();
    this->cmtObj = new cmt::CMT();
    this->analyzedFrameNumber = 0;
    this->cmtObj->selectionDone = false;
}

int aiSaac::CmtTracking::detect(cv::Mat &rawFrame, cv::Rect selection) {
    cv::Mat frame_gray;
    cv::cvtColor(rawFrame, frame_gray, CV_BGR2GRAY);
    this->cmtObj->initialize(frame_gray, selection);
    this->cmtObj->selectionDone = true;
    return this->cmtObj->initial_active_points;
}

void aiSaac::CmtTracking::track(cv::Mat &rawFrame, int frameNumber) {
    cv::Mat frame_gray;
    cv::cvtColor(rawFrame, frame_gray, CV_BGR2GRAY);
    if (this->cmtObj->selectionDone) {
        this->cmtObj->processFrame(frame_gray);
        // std::cout << "Points active" << this->cmtObj->points_active.size() << std::endl;
        Point2f vertices[4];
        this->cmtObj->bb_rot.points(vertices);
        float minX = rawFrame.cols;
        float maxX = 0;
        float minY = rawFrame.rows;
        float maxY = 0;
        for (int i = 0; i < 4; i++) {
            float currentX = vertices[i].x;
            float currentY = vertices[i].y;
            if (minX > currentX)
                minX = currentX;
            if (maxX < currentX)
                maxX = currentX;
            if (minY > currentY)
                minY = currentY;
            if (maxY < currentY)
                maxY = currentY;
         }
         if (minX < 0)
             minX = 0;
         if (maxX > rawFrame.cols)
             maxX = rawFrame.cols;
         if (minY < 0)
             minY = 0;
         if (maxY > rawFrame.rows)
             maxY = rawFrame.rows;
         this->cmtObj->boundingRectangle = cv::Rect(minX, minY, (maxX - minX), (maxY - minY));
         this->currentRect = this->cmtObj->boundingRectangle;
         if (this->cmtObj->pastPositions.size() == 50) {
             this->cmtObj->pastPositions.erase(
                     this->cmtObj->pastPositions.begin());
         }
         cv::Point position;
         position.x = this->cmtObj->boundingRectangle.x + this->cmtObj->boundingRectangle.width / 2;
         position.y = this->cmtObj->boundingRectangle.y
                 + this->cmtObj->boundingRectangle.height / 2;
         this->cmtObj->pastPositions.push_back(position);
    }
    this->analyzedFrameNumber = frameNumber;
}

void aiSaac::CmtTracking::annotate(cv::Mat &rawFrame) {
    this->annotate(rawFrame, this->analyzedFrameNumber);
}

void aiSaac::CmtTracking::annotate(cv::Mat &rawFrame, int frameNumber) {
    cv::rectangle(rawFrame, this->currentRect, cv::Scalar(0, 255, 0), 2);
}

cv::Rect aiSaac::CmtTracking::localize() {
    return this->currentRect;
}
