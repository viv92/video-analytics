/*
    Copyright 2016 AITOE
*/

#include "fileStreamer.h"

FileStreamer::FileStreamer(std::string filePath)  {
    this->filePath = filePath;
    this->cap = new cv::VideoCapture();
    this->streaming = false;
    this->cap->open(this->filePath);
    std::cout << "opening" << std::endl;
    if(!this->cap->isOpened())
        this->streaming = false;
    else
        this->streaming = true;
}

FileStreamer::~FileStreamer() {
    if (this->cap != NULL) {
        std::cout << "qDebug" << "Releasing capture object in FileStreamer" << std::endl;
        this->cap->release();
    }
    delete this->cap;
}

void FileStreamer::startStream() {}

void FileStreamer::stopStream() {
    // cap->release();
    std::cout << "qDebug" << "FileStreamer stop called" << std::endl;
    if (this->cap != NULL) {
        std::cout << "qDebug" << "Releasing capture object in FileStreamer" << std::endl;
        this->cap->release();
    }
    delete this->cap;
    std::terminate();
}

cv::Mat FileStreamer::getFrame() {
    this->mutex.lock();
    this->streaming = this->cap->read(this->in);
    cv::Mat retFrame = this->in.clone();
    // if (!(retFrame.empty() || retFrame.data == NULL)) {
    //     cv::resize(retFrame,
    //         retFrame,
    //         cv::Size((int)(retFrame.cols * 0.5), (int)(retFrame.rows * 0.5)));
    // }
    this->mutex.unlock();
    return retFrame;
}

bool FileStreamer::isStreaming() {
    return this->streaming;
}

void FileStreamer::setFilePath(std::string filePath) {
    this->filePath = filePath;
}

int FileStreamer::getFileFPS() {
    return this->cap->get(CV_CAP_PROP_FPS);
}

int FileStreamer::getFourCC() {
    return this->cap->get(CV_CAP_PROP_FOURCC);
}

cv::Size FileStreamer::getFrameSize() {
    // return cv::Size(this->cap->get(CV_CAP_PROP_FRAME_WIDTH) * 0.5, this->cap->get(CV_CAP_PROP_FRAME_HEIGHT) * 0.5);
    return cv::Size(this->cap->get(CV_CAP_PROP_FRAME_WIDTH), this->cap->get(CV_CAP_PROP_FRAME_HEIGHT));
}

int FileStreamer::getNumberOfFrames() {
    return this->cap->get(CV_CAP_PROP_FRAME_COUNT);
}
