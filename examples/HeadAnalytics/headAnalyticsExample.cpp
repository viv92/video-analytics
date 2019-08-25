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
        if ( argc < 4 ) {

            std::cout << "Usage: ./headAnalyticsExample [sourcefile] [outputFilePath] [aiSaacSettingsPath] " << std::endl;

            return -1;
        }
        std::string file = argv[1];
        std::string outputFilePath = argv[2];
        std::string aiSaacSettingsPath = argv[3];

        aiSaac::AiSaacSettings *aiSaacSettings = new aiSaac::AiSaacSettings(aiSaacSettingsPath);
        aiSaac::HeadAnalytics headAnalytics = aiSaac::HeadAnalytics(aiSaacSettings);

        if ( file.find("mp4") < file.length() ||  file.find("mpg") < file.length() ) {

            FileStreamer *fileStreamer = new FileStreamer(file);
            if(!fileStreamer->isStreaming()) {
                debugMsg("Unable to open file for reading");
                return -1;
            } else {
                std::cout << "Found file for reading" << std::endl;
            }

            cv::Mat currentFrame, annotatedFrame;
            cv::VideoWriter annotatedVideo;
            int frameNumber = 0;
            int totalProcessedFrameNumber = 0;
            int FPS = fileStreamer->getFileFPS();
            int procFPS = FPS;
            cv::Size S;

            std::cout << "here2" << std::endl;

            S = fileStreamer->getFrameSize();
            annotatedVideo.open(outputFilePath, CV_FOURCC('M','J','P','G'), FPS, S, true);
            if ( procFPS < 1 ) {
                procFPS = 1;
            } else if ( procFPS > FPS ) {
                procFPS = FPS;
            }

            while (true) {
                currentFrame = fileStreamer->getFrame();
                if( currentFrame.data == NULL || currentFrame.empty() || !fileStreamer->isStreaming()) {
                    break;
                }
                totalProcessedFrameNumber++;
            }

            fileStreamer = new FileStreamer(file);
            while (true) {
                currentFrame = fileStreamer->getFrame();
                if( currentFrame.data == NULL || currentFrame.empty() || !fileStreamer->isStreaming()) {
                    break;
                }
                if (frameNumber % FPS / procFPS == 0) {
                    headAnalytics.track(currentFrame, frameNumber);
                }

                if (frameNumber == floor(totalProcessedFrameNumber*0.25)) {
                    std::cout << "AISAAC_LOG: Your video is 25% processed." << std::endl;
                } else if (frameNumber == floor(totalProcessedFrameNumber*0.50)) {
                    std::cout << "AISAAC_LOG: Your video is 50% processed." << std::endl;
                } else if (frameNumber == floor(totalProcessedFrameNumber*0.75)) {
                    std::cout << "AISAAC_LOG: Your video is 75% processed." << std::endl;
                }

                annotatedFrame = currentFrame;
                headAnalytics.annotate(annotatedFrame);
                if (annotatedFrame.data != NULL) {
                    cv::imshow("Annotated Scene", annotatedFrame);
                    cv::waitKey(10);
                    annotatedVideo.write(annotatedFrame);
                }
                frameNumber++;
            }
            annotatedVideo.release();
            std::cout << "AISAAC_LOG: Your video is 100% processed." << std::endl;
        } else {
            cv::Mat image = cv::imread(file, CV_LOAD_IMAGE_COLOR);

            if( !image.data ) {
                debugMsg("Unable to open file for reading");
                return -1;
            }

            headAnalytics.track(image, 1);

            if ( !headAnalytics.blobContainer.size() ) {
                std::cout << "AISAAC_LOG: No heads were detected in the input." << std::endl;
            } else {
                cv::Mat annotatedFrame = image;
                headAnalytics.annotate(annotatedFrame);
                cv::imwrite(outputFilePath, annotatedFrame);
                std::cout << "AISAAC_LOG: Your image has been processed." << std::endl;
            }
        }
        return 0;
    } catch(...) {
        std::cout << "There was an exception" << std::endl;
        return -1;
    }
}
