/*
    Copyright 2016 AITOE
*/

#ifndef SRC_WEBCAM_STREAMER_H
#define SRC_WEBCAM_STREAMER_H

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

class WebcamStreamer: public aiVideoStreamer {
 public:
    WebcamStreamer();
    ~WebcamStreamer();
    void startStream();
    void stopStream();
    cv::Mat getFrame();
    bool isStreaming();

 private:
    // cv::Mat in;
    cv::VideoCapture* cap;
    bool streaming;
    bool stop;

    // std::mutex mutex;
};

#endif  // SRC_WEBCAM_STREAMER_H
