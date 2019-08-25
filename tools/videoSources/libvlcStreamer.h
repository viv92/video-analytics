/*
    Copyright 2016 AITOE
*/

#ifndef SRC_LIBVLC_STREAMER_H
#define SRC_LIBVLC_STREAMER_H

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

class LibvlcStreamer: public aiVideoStreamer {
 public:
    LibvlcStreamer(std::string rtspUrl, int vlcFPS);
    ~LibvlcStreamer();
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

    cv::Size getFrameSize(const char* path);
    libvlc_instance_t *vlcInstance;
    libvlc_media_player_t *vlcMediaPlayer;
    libvlc_media_t *vlcMedia;
    struct ctx* context;
};

#endif  // SRC_LIBVLC_STREAMER_H
