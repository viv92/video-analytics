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

#ifdef USE_MONGO
#include "../../tools/mongoLink/mongoLink.h"
#else
#include "../../tools/mongoLink/serialLink.h"
#endif //USE_MONGO

int main(int argc, char *argv[]) {
    try {
        if ( argc < 4 ) {
            std::cout << "Usage: ./textRecognitionJsonifyExec [file] [aiSaacSettingsPath] [mongoResultTableID]";
            return -1;
        }
        std::string file = argv[1];
        std::string aiSaacSettingsPath = argv[2];
        std::string resultTableId = argv[3];

        aiSaac::AiSaacSettings *aiSaacSettings = new aiSaac::AiSaacSettings(aiSaacSettingsPath);
        aiSaac::TextAnalytics *textAnalytics =  new aiSaac::TextAnalytics(aiSaacSettings);

        if ( file.find("mp4") < file.length() ) {
            FileStreamer *fileStreamer = new FileStreamer(file);
            if (!fileStreamer->isStreaming()) {
                debugMsg("Unable to open file for reading");
                return -1;
            }

            cv::Mat currentFrame;
            int frameNumber = 0;
            int totalProcessedFrameNumber = 0;
            int FPS = fileStreamer->getFileFPS();
            int procFPS = aiSaacSettings->getProcFPS();
            mongoLink *mongoObject = new mongoLink(FPS);

            if ( annotateVideo ) {
                cv::Mat annotatedFrame;
                cv::VideoWriter annotatedVideo;
                cv::Size S = fileStreamer->getFileFrameSize();
                annotatedVideo.open(filePath + "annotated_video.avi", CV_FOURCC('M','J','P','G'), FPS, S, true);
            }

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
                    textAnalytics->track(currentFrame, frameNumber);
                    textAnalytics->recognize();
                    mongoObject->textAnalyticsMONGO(textAnalytics->blobContainer, frameNumber, resultTableId);
                }

                if (frameNumber == floor(totalProcessedFrameNumber*0.25)) {
                    std::cout << "AISAAC_LOG: Your video is 25% processed." << std::endl;
                } else if (frameNumber == floor(totalProcessedFrameNumber*0.5)) {
                    std::cout << "AISAAC_LOG: Your video is 50% processed." << std::endl;
                } else if (frameNumber == floor(totalProcessedFrameNumber*0.75)) {
                    std::cout << "AISAAC_LOG: Your video is 75% processed." << std::endl;
                }

                if ( annotateVideo ) {
                    cv::Mat blankFrame(currentFrame.rows, currentFrame.cols, CV_8UC3, cv::Scalar(0, 0, 0));
                    textAnalytics->annotate(blankFrame);
                    if (currentFrame.data != NULL) {
                        cv::imshow("Annotated Scene", blankFrame);
                        cv::imshow("Original Scene", currentFrame);
                        cv::waitKey(10);
                        annotatedVideo.write(blankFrame);
                    }
                }
                frameNumber++;
            }
            if ( annotateVideo ) {
                annotatedVideo.release();
            }
            std::cout << "AISAAC_LOG: Your video is 100% processed." << std::endl;
        } else {
            cv::Mat image = cv::imread(file, CV_LOAD_IMAGE_COLOR);

            if( !image.data ) {
                debugMsg("Unable to open file for reading");
                return -1;
            }

            mongoLink *mongoObject = new mongoLink(1);

            textAnalytics->track(image, 1);

            if ( !textAnalytics->blobContainer.size() ) {
                std::cout << "AISAAC_LOG: No text was detected in the input." << std::endl;
            } else {
                textAnalytics->recognize();
                mongoObject->textAnalyticsMONGO(textAnalytics->blobContainer, 1, resultTableId);
                std::cout << "AISAAC_LOG: Your image has been processed." << std::endl;
            }
        }
        return 0;
    } catch(...) {
        return -1;
    }
}
