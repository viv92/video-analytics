/*
    Copyright 2016 AITOE
*/

#include <cv.h>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <dirent.h>
#include <fstream>
#include "../../tools/videoSources/fileStreamer.h"
#include "../../tools/videoSources/aiVideoStreamer.h"
#include "../../tools/logging.h"
#include "../../src/aiSaac.h"

#ifdef USE_MONGO
#include "../../tools/mongoLink/mongoLink.h"
#else
#include "../../tools/mongoLink/serialLink.h"
#endif //USE_MONGO

int main(int argc, char *argv[]) {
    try {
        if ( argc < 4 ) {
            std::cout << "Usage: ./sceneJsonifyExec [file] [aiSaacSettingsPath] [mongoResultTableID]";
            return -1;
        }
        std::string file = argv[1];
        std::string aiSaacSettingsPath = argv[2];
        std::string resultTableId = argv[3];

        aiSaac::AiSaacSettings *aiSaacSettings = new aiSaac::AiSaacSettings(aiSaacSettingsPath);
        aiSaac::SceneAnalytics *sceneAnalytics = new aiSaac::SceneAnalytics(*aiSaacSettings);

        if ( file.find("mp4") < file.length() ) {
            FileStreamer *fileStreamer = new FileStreamer(file);
            if(!fileStreamer->isStreaming()) {
                debugMsg("Unable to open file for reading");
                return -1;
            }

            cv::Mat currentFrame, annotatedFrame;
            cv::VideoWriter annotatedVideo;
            int frameNumber = 0;
            int totalProcessedFrameNumber = 0;
            int FPS = fileStreamer->getFileFPS();
            int procFPS = aiSaacSettings->getProcFPS();
            mongoLink *mongoObject = new mongoLink(FPS);

            if ( annotateVideo ) {
                cv::Size S = fileStreamer->getFileFrameSize();
                annotatedVideo.open(storagePath + "/annotated_video.avi", CV_FOURCC('M','J','P','G'), FPS, S, true);
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
                    sceneAnalytics->track(currentFrame, frameNumber);
                    mongoObject->sceneAnalyticsMONGO(sceneAnalytics->blobContainer, frameNumber, resultTableId);
                }

                if (frameNumber == floor(totalProcessedFrameNumber*0.25)) {
                    std::cout << "AISAAC_LOG: Your video is 25% processed." << std::endl;
                } else if (frameNumber == floor(totalProcessedFrameNumber*0.50)) {
                    std::cout << "AISAAC_LOG: Your video is 50% processed." << std::endl;
                } else if (frameNumber == floor(totalProcessedFrameNumber*0.75)) {
                    std::cout << "AISAAC_LOG: Your video is 75% processed." << std::endl;
                }

                if ( annotateVideo ) {
                    annotatedFrame = currentFrame;
                    sceneAnalytics->annotate(annotatedFrame);
                    if (annotatedFrame.data != NULL) {
                        cv::imshow("Annotated Scene", annotatedFrame);
                        cv::waitKey(10);
                        annotatedVideo.write(annotatedFrame);
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

            sceneAnalytics->track(image, 1);

            if ( !sceneAnalytics->blobContainer.size() ) {
                std::cout << "AISAAC_LOG: No scene was detected in the input." << std::endl;
            } else {
                mongoObject->sceneAnalyticsMONGO(sceneAnalytics->blobContainer, 1, resultTableId);
                std::cout << "AISAAC_LOG: Your image has been processed." << std::endl;
            }
        }

        return 0;
    } catch(...) {
        return -1;
    }
}
