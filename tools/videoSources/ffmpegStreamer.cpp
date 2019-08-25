/*
    Copyright 2016 AITOE
*/

#include "ffmpegStreamer.h"

FfmpegStreamer::FfmpegStreamer(std::string rtspUrl)  {
    this->cap = new cv::VideoCapture();
    this->stop = true;
    this->streaming = false;
    this->rtspUrl = rtspUrl;
}


FfmpegStreamer::~FfmpegStreamer() {
    if (cap != NULL) {
        std::cout << "qDebug" << "Releasing capture object in FfmpegStreamer" << std::endl;
        this->cap->release();
    }
    delete this->cap;
}

void FfmpegStreamer::startStream() {
    this->cap->open(this->rtspUrl);
    this->stop = false;
    while (!this->stop) {
        cv::Mat frame;
        this->cap->read(frame);
        this->mutex.lock();
        this->in = frame.clone();
        this->mutex.unlock();
        // std::cout << this->in.size() << std::endl;
        if (in.rows > 0 && in.cols > 0) {
            // std::cout << this->in.size() << std::endl;
            this->streaming = true;
            // cv::imshow("Display window",in.clone());

            cv::waitKey(5);
        } else {
            // std::cout << "no image" << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    in.data = NULL;
    std::cout << "qDebug"
 << "FfmpegStreamer came out of while loop" << std::endl;;
    std::terminate();
}

void FfmpegStreamer::stopStream() {
    // cap->release();
    std::cout << "qDebug" << "FfmpegStreamer stop called" << std::endl;
    this->streaming = false;
    this->stop = true;
}

cv::Mat FfmpegStreamer::getFrame() {
    aiVideoStreamer::mutex.lock();
    cv::Mat retFrame = this->in.clone();
    if (!(retFrame.empty() || retFrame.data == NULL)) {
        cv::resize(retFrame,
            retFrame,
            cv::Size((int)(retFrame.cols * 0.5), (int)(retFrame.rows * 0.5)));
    }
    this->mutex.unlock();
    return retFrame;
}

bool FfmpegStreamer::isStreaming() {
    return this->streaming;
}
