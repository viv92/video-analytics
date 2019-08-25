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

int main( int argc, char *argv[]) {
    try {
        if (argc < 3) {
            std::cout << "Usage: ./bgSubJsonifyExec [videoFile] [annotatedVideoPath] [aiSaacSettingsPath] [ProcFPS]";
            return -1;
        }
        std::string videoFile = argv[1];
        std::string outputFilePath = argv[2];
        std::string aiSaacSettingsPath = argv[3];
        int procFPS = atoi(argv[4]);
        cv::VideoWriter annotatedVideo;

        FileStreamer *fileStreamer = new FileStreamer(videoFile);
        if(!fileStreamer->isStreaming()) {
            debugMsg("Unable to open file for reading");
            return -1;
        }

        aiSaac::AiSaacSettings aiSaacSettings(aiSaacSettingsPath);
        aiSaac::BgSubAnalytics *bgSubAnalytics = new aiSaac::BgSubAnalytics(aiSaacSettings);
        cv::Size S = fileStreamer->getFrameSize();
        int FPS = fileStreamer->getFileFPS();
        //int procFPS = aiSaacSettings.getProcFPS();
        cv::Mat currentFrame;
        cv::Mat annotatedFrame;
        if (argc > 2) {
            annotatedVideo.open(outputFilePath,
                CV_FOURCC('M','J','P','G'),
                FPS,
                S,
                true);
        }

        int frameNumber = 0;
        while (true) {
            currentFrame = fileStreamer->getFrame();
            annotatedFrame = currentFrame.clone();
            if(currentFrame.data == NULL || currentFrame.empty() || !fileStreamer->isStreaming()) {
                break;
            }
            if (frameNumber % (FPS / procFPS) == 0) {
              bgSubAnalytics->track(currentFrame, frameNumber);
            }
            bgSubAnalytics->annotate(annotatedFrame);
            if (annotatedFrame.data != NULL) {
                cv::imshow("Annotated Object", annotatedFrame);
                cv::waitKey(10);
                if (argc > 2) {
                    annotatedVideo.write(annotatedFrame);
                }
            }
            frameNumber++;
        }
        return 0;
    } catch(...) {
        return -1;
    }
}
