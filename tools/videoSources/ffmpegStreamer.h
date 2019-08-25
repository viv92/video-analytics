/*
    Copyright 2016 AITOE
*/

#ifndef SRC_FFMPEG_STREAMER_H
#define SRC_FFMPEG_STREAMER_H

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

class FfmpegStreamer: public aiVideoStreamer {
 public:
    FfmpegStreamer(std::string);
    ~FfmpegStreamer();
    void startStream();
    void stopStream();
    cv::Mat getFrame();
    bool isStreaming();

 private:
    // cv::Mat in;
    cv::VideoCapture* cap;
    std::string rtspUrl;
    bool streaming;
    bool stop;

    // std::mutex mutex;
};

#endif  // SRC_FFMPEG_STREAMER_H
