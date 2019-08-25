/*
    Copyright 2016 AITOE
*/

#ifndef SRC_FILE_STREAMER_H
#define SRC_FILE_STREAMER_H

#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <vlc/vlc.h>

#include <string>
#include <mutex>
#include <thread>
#include <chrono>

#include "aiVideoStreamer.h"
#include "../logging.h"

class FileStreamer: public aiVideoStreamer {
 public:
    FileStreamer(std::string);
    ~FileStreamer();
    void startStream();
    void stopStream();
    cv::Mat getFrame();
    bool isStreaming();
    void setFilePath(std::string);
    int getFileFPS();
    int getFourCC();
    cv::Size getFrameSize();
    int getNumberOfFrames();
    cv::VideoCapture* cap;

 private:
    // cv::Mat in;
    std::string filePath;
    bool streaming;

    // std::mutex mutex;
};

#endif  // SRC_FILE_STREAMER_H
