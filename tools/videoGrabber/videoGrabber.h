//
// Created by aitoe on 22/08/2017
//

#ifndef AISAAC_VIDEOGRABBER_H
#define AISAAC_VIDEOGRABBER_H

#include <iostream>
// #include <cv.h>
#include <opencv2/videoio.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <string>
#include <ctime>
#include "../../src/encoderCameraSettings.h"
#include "../logAlert/logAlert.h"

#define MAXLEN 20

class videoGrabber {

public:
    videoGrabber(encoderCameraSettings* camSettings, cv::Size frameSize, std::string outputFolderPath, std::string prefix);
    ~videoGrabber();
    void initialize();
    void releaseVideo();
    std::string acceptFrame(cv::Mat& frame);
    void saveFrame();
    std::string formatTime(std::time_t& time);
    void convertVideo();
    bool activeVideoChunk();
    int videoChunkFrameNumber();

private:
    int FPS;
    bool writerReady;
    int videoChunkFrameCount;
    std::string outputFolderPath;
    std::string prefix;
    std::string cameraRTSP;
    char outTime[MAXLEN];
    std::time_t startTime;
    std::time_t endTime;
    cv::Size frameSize;
    cv::Mat frame;
    cv::VideoWriter* videoChunk;
    logAlert logger;
};

#endif //AISAAC_VIDEOGRABBER_H
