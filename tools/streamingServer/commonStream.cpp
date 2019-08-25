//
// Created by aitoe on 14/09/2017
//

#include "commonStream.h"

commonStream::commonStream() {
    std::cout << "In commonStream constructor" << std::endl;
    this->frame = cv::Mat::zeros(1080, 1920, CV_8UC3);
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

std::vector<uchar>* commonStream::getEncodedFrame() {
    std::vector<uchar> tempFrame;
    cv::Mat tempDispFrame = giveFrame();
    std::vector<int> params = std::vector<int>();
    params.push_back(CV_IMWRITE_JPEG_QUALITY);
    params.push_back(30);
    cv::imencode(".jpg", tempDispFrame, tempFrame, params);
    std::vector<uchar> *encodedImg = new std::vector<uchar>(tempFrame.begin(), tempFrame.end());
    return encodedImg;
}
