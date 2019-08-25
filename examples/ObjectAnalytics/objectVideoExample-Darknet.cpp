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
    try {
        if (argc < 4) {
            std::cout << "Usage: objectVideoExampleDarknet <videotoLoad> <annotatedVideoPath> <aiSaacSettings>" << endl;
            return -1;
        }
        FileStreamer *fileStreamer = new FileStreamer(argv[1]);
        aiSaac::AiSaacSettings aiSaacSettings(argv[3]);

        signal(SIGINT, handler);
        aiSaac::ObjectAnalytics *objectAnalytics = new aiSaac::ObjectAnalytics(aiSaacSettings);
        cv::VideoWriter annotatedVideo;
        cv::Size S = fileStreamer->getFileFrameSize();
        int FPS = fileStreamer->getFileFPS();
        if (argc > 2) {
            annotatedVideo.open(argv[2],
                CV_FOURCC('M', 'J', 'P', 'G'),
                FPS,
                S,
                true);
        }
        cv::Mat currentFrame, annotatedFrame;
        int frameNumber = 0;
        currentFrame = fileStreamer->getFrame();
        int procFPS = fileStreamer->getFileFPS();

        while (true) {
            currentFrame = fileStreamer->getFrame();
            annotatedFrame = currentFrame;
            if(currentFrame.data == NULL ||
                currentFrame.empty() ||
                !fileStreamer->isStreaming() ||
                !flag) {
                break;
            }
            if (frameNumber % (FPS / procFPS) == 0) {
                clock_t time;
                time = clock();
                objectAnalytics->track(currentFrame, frameNumber);

                std::cout << "Track Time: " << sec(clock() - time) << std::endl;
            }
            objectAnalytics->annotate(annotatedFrame);
            if (annotatedFrame.data != NULL) {
                cv::imshow("Annotated Objects", annotatedFrame);
                cv::waitKey(10);
                if (argc > 2) {
                    std::cout << "writing to video file" << std::endl;
                    annotatedVideo.write(annotatedFrame);
                }
            }
            frameNumber++;
        }
        annotatedVideo.release();
        delete fileStreamer;
        delete objectAnalytics;
        // delete aiSaacSettings;
        return 0;
    } catch(...) {
        return -1;
    }
}
