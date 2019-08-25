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
        if (argc != 3) {
            std::cout << "Usage: sceneVideoExample videotoLoad <annotatedVideoPath>" << endl;
            return -1;
        }

        signal(SIGINT, handler);
        FileStreamer *cam = new FileStreamer(argv[1]);
        aiSaac::AiSaacSettings aiSaacSettings =
            aiSaac::AiSaacSettings(storagePath +
                "/objectAnalyticsSettingsDN.json");

        aiSaac::SceneAnalytics *sceneAnalytics = NULL;
        std::cout << "here i am 1" << std::endl;
        sceneAnalytics = new aiSaac::SceneAnalytics(aiSaacSettings);
        std::cout << "here i am 2" << std::endl;
        cv::VideoWriter annotatedVideo;
        std::cout << "here i am 3" << std::endl;
        cv::Size S = cam->getFileFrameSize();
        int FPS = cam->getFileFPS();
        if (argc > 2) {
            annotatedVideo.open(argv[2], CV_FOURCC('M', 'J', 'P', 'G') , FPS, S, true);
        }

        cv::Mat currentFrame, annotatedFrame;
        int frameNumber = 0;
        int procFPS = FPS;
        std::cout << "here i am 4" << std::endl;

        while (true) {
            currentFrame = cam->getFrame();
            annotatedFrame = currentFrame;
            if(currentFrame.data == NULL ||
                currentFrame.empty() ||
                !cam->isStreaming() ||
                !flag) {
                break;
            }
            if (frameNumber % (FPS/procFPS) == 0) {
                sceneAnalytics->track(currentFrame, frameNumber);
            }

            sceneAnalytics->annotate(annotatedFrame);
            if (annotatedFrame.data != NULL) {
                cv::imshow("Annotated Scene", annotatedFrame);
                cv::waitKey(10);
                if (argc > 2) {
                    annotatedVideo.write(annotatedFrame);
                }
            }
            frameNumber++;
        }
        annotatedVideo.release();
        return 0;
    } catch(...) {
        return -1;
    }
}
