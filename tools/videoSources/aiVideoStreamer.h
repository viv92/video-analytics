/*
    Copyright 2016 AITOE
*/

#ifndef AI_VIDEO_STREAMER_H
#define AI_VIDEO_STREAMER_H

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <opencv2/opencv.hpp>
#include <mutex>

struct ctx {
    cv::Mat* image;
    uchar* pixels;
    std::mutex* mutexObj;
};

// namespace aisl {
class aiVideoStreamer {
 public:
    aiVideoStreamer();
    ~aiVideoStreamer();
    virtual void startStream();
    virtual void stopStream();
    virtual cv::Mat getFrame();
    virtual bool isStreaming();
 // private:
    std::mutex mutex;
    cv::Mat in;
    // bool streaming;
    // bool stop;
};
// }

#endif  // AI_VIDEO_STREAMER_H
