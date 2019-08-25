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
#include <signal.h>

volatile sig_atomic_t flag = 1;

void handler(int sig){ // can be called asynchronously
  flag = 0; // set flag
}

int main(int argc, char *argv[]) {
    ::google::InitGoogleLogging(argv[0]);
    if (argc < 2) {
        std::cout << "Usage: sceneVideoExample videotoLoad <annotatedVideoPath> <start-time> <end-time>" << endl;
        return -1;
    }
    signal(SIGINT, handler);
    FileStreamer *cam = new FileStreamer(argv[1]);
    aiSaac::AiSaacSettings *aiSaacSettings = new aiSaac::AiSaacSettings(storagePath + 'Text.json');
    aiSaac::TextAnalytics *textAnalytics =  new aiSaac::TextAnalytics(aiSaacSettings);
    cv::VideoWriter annotatedVideo;
    cv::Size S = cam->getFileFrameSize();
    int FPS = cam->getFileFPS();
    if (argc > 2) {
        annotatedVideo.open(argv[2],
            CV_FOURCC('M', 'P', '4', '2'),
            FPS,
            S,
            true);
    }
    cv::Mat currentFrame;
    int frameNumber = 0;
    int procFrameNumber = 0;
    currentFrame = cam->getFrame();
    int procFPS = cam->getFileFPS();

    int startFrame = (argc > 3) ? cam->getFileFPS() * std::atoi(argv[3]) : 0;
    int endFrame = (argc > 4) ? cam->getFileFPS() * std::atoi(argv[4]) : -1;

    std::cout << "Start Time: " << startFrame << std::endl;
    std::cout << "End Time: " << endFrame << std::endl;
    while (frameNumber != startFrame) {
        cam->getFrame();
        frameNumber++;
    }

    std::cout << "starting analytics" << std::endl;
    while (currentFrame.data != NULL && frameNumber != endFrame && flag) {
        cv::Mat blankFrame(currentFrame.rows, currentFrame.cols, CV_8UC3, cv::Scalar(0, 0, 0));
        currentFrame = cam->getFrame();
        // annotatedFrame = currentFrame;
        if (frameNumber % FPS / procFPS == 0) {
            textAnalytics->track(currentFrame, procFrameNumber);
            std::cout << "what is going on" << std::endl;
            procFrameNumber++;
        }
        std::cout << "about to recognize1" << std::endl;
        textAnalytics->recognize();
        textAnalytics->annotate(blankFrame);
        if (currentFrame.data != NULL) {
            cv::imshow("Annotated Scene", blankFrame);
            cv::imshow("Original Scene", currentFrame);
            cv::waitKey(10);
            if (argc > 2) {
                annotatedVideo.write(blankFrame);
            }
        }
        frameNumber++;
    }
    annotatedVideo.release();
}
