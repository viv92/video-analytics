/*
    Copyright 2016 AITOE
*/

#include "webcamStreamer.h"

WebcamStreamer::WebcamStreamer()  {
    this->cap = new cv::VideoCapture();
    this->stop = true;
    this->streaming = false;
}

WebcamStreamer::~WebcamStreamer() {
    if (cap != NULL) {
        std::cout << "qDebug" << "Releasing capture object in WebcamStreamer" << std::endl;
        this->cap->release();
    }
    delete this->cap;
}

void WebcamStreamer::startStream() {

    std::cout << "opening" << std::endl;
    this->cap->open(0);
    this->stop = false;
    while (!this->stop) {
        // std::cout << "hello" << std::endl;
        this->mutex.lock();
        // std::cout << "reading";
        this->cap->read(this->in);
        this->mutex.unlock();
        if (this->in.rows > 0 && this->in.cols > 0) {
            this->streaming = true;
            // cv::imshow("Display window",in.clone());
            // std::cout << this->in.size() << std::endl;
            cv::waitKey(5);
        } else {
            std::cout << "cannot find frame" << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        /*((vigilant*)parent)->mutex.lock();
        ((vigilant*)parent)->in = in;
        ((vigilant*)parent)->mutex.unlock();*/
        // cvWaitKey(24);
        // this->msleep(1);
    }
    this->cap->release();
    this->in.data = NULL;
    std::cout << "qDebug"
 << "WebcamStreamer came out of while loop" << std::endl;;
    std::terminate();
}

void WebcamStreamer::stopStream() {
    // cap->release();
    std::cout << "qDebug" << "WebcamStreamer stop called" << std::endl;
    this->streaming = false;
    this->stop = true;
}

cv::Mat WebcamStreamer::getFrame() {
    this->mutex.lock();
    cv::Mat retFrame = this->in.clone();
    if (!(retFrame.empty() || retFrame.data == NULL)) {
        cv::resize(retFrame,
            retFrame,
            cv::Size((int)(retFrame.cols * 0.5), (int)(retFrame.rows * 0.5)));
    }
    this->mutex.unlock();
    return retFrame;
}

bool WebcamStreamer::isStreaming() {
    return this->streaming;
}
