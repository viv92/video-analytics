/*
    Copyright 2016 AITOE
*/

#include <cv.h>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include "../../tools/videoSources/fileStreamer.h"
#include "../../tools/videoSources/aiVideoStreamer.h"
#include "../../tools/logging.h"
#include "../../src/aiSaac.h"

int main(int argc, char *argv[]) {
    ::google::InitGoogleLogging(argv[0]);
    if (argc < 3) {
        std::cout << "Usage: objectVideoExampleCaffe [videotoLoad] [annotatedVideoPath] [aiSaacSettings]" << endl;
        return -1;
    }
    FileStreamer *cam = new FileStreamer(argv[1]);
    aiSaac::AiSaacSettings aiSaacSettings = aiSaac::AiSaacSettings(argv[3]);
    aiSaac::ObjectAnalytics objectAnalytics = aiSaac::ObjectAnalytics(aiSaacSettings);
    cv::VideoWriter annotatedVideo;
    cv::Size S = cam->getFileFrameSize();
    int FPS = cam->getFileFPS();
    if (argc > 2) {
        annotatedVideo.open(argv[2], CV_FOURCC('M','J','P','G'), FPS, S, true);
    }
    cv::Mat currentFrame, annotatedFrame;
    int frameNumber = 0;
    int procFrameNumber = 0;
    currentFrame = cam->getFrame();
    int procFPS = cam->getFileFPS();
    while (currentFrame.data != NULL) {
        currentFrame = cam->getFrame();
        annotatedFrame = currentFrame;
        if (frameNumber % FPS / procFPS == 0) {
            objectAnalytics.track(currentFrame, procFrameNumber);
            procFrameNumber++;
        }
        objectAnalytics.annotate(annotatedFrame);
        if (annotatedFrame.data != NULL) {
            cv::imshow("Annotated Object", annotatedFrame);
            cv::waitKey(10);
            if (argc > 2) {
                annotatedVideo.write(annotatedFrame);
            }
        }
        frameNumber++;
    }
    annotatedVideo.release();
}
