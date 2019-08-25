//
// Created by aitoe on 14/09/2017
//

#include "commonStream.h"

commonStream::commonStream() {
    std::cout << "In commonStream constructor" << std::endl;
    this->frame = cv::Mat::zeros(1280, 720, CV_8UC3);
}

commonStream::~commonStream() {
    std::cout << "Called commonStream destructor" << std::endl;
}

void commonStream::takeFrame(cv::Mat frame) {
    frameMutex.lock();
    this->frame = frame;
    frameMutex.unlock();
}

cv::Mat commonStream::giveFrame() {
    cv::Mat temp;
    frameMutex.lock();
    temp = this->frame;
    frameMutex.unlock();
    return temp;
}
