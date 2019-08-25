//
// Created by aitoe on 14/09/2017
//

#ifndef COMMON_STREAM_H
#define COMMON_STREAM_H

#include <cv.h>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <mutex>

class commonStream {
    public:
        commonStream();
        ~commonStream();
        void takeFrame(cv::Mat frame);
        cv::Mat giveFrame();
    private:
        cv::Mat frame;
        std::mutex frameMutex;
};

#endif  // COMMON_STREAM_H
