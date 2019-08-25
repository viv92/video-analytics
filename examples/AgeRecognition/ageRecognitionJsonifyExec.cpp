/*
    Copyright 2016 AITOE
*/

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <dirent.h>
#include <vector>
#include <fstream>
#include "../../src/aiSaac.h"
#include "../../tools/videoSources/fileStreamer.h"

#ifdef USE_MONGO
#include "../../tools/mongoLink/mongoLink.h"
#else
#include "../../tools/mongoLink/serialLink.h"
#endif //USE_MONGO

int main(int argc, char *argv[]) {
    try {
        if ( argc < 4 ) {
            std::cout << "Usage: ./ageRecognitionJsonifyExec [file] [aiSaacSettingsPath] [mongoResultTableID]";
            return -1;
        }
        std::string file = argv[1];
        std::string aiSaacSettingsPath = argv[2];
        std::string resultTableId = argv[3];

        aiSaac::AiSaacSettings *aiSaacSettings = new aiSaac::AiSaacSettings(aiSaacSettingsPath);
        aiSaac::FaceAnalytics *faceAnalytics = new aiSaac::FaceAnalytics(*aiSaacSettings);
        aiSaac::AgeRecognition *ageRecognition = new aiSaac::AgeRecognition(*aiSaacSettings);

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
            while(true) {
                currentFrame = fileStreamer->getFrame();
                if( currentFrame.data == NULL || currentFrame.empty() || !fileStreamer->isStreaming()) {
                    break;
                }
                if (frameNumber % FPS / procFPS == 0) {
                    faceAnalytics->track(currentFrame, frameNumber);
                    for (int i = 0; i < faceAnalytics->blobContainer.size(); i++) {
                        faceAnalytics->blobContainer[i].age = ageRecognition->detect(currentFrame);
                    }
                    mongoObject->ageAnalyticsMONGO(faceAnalytics->blobContainer, frameNumber, resultTableId);
                }

                if (frameNumber == floor(totalProcessedFrameNumber*0.25)) {
                    std::cout << "AISAAC_LOG: Your video is 25% processed." << std::endl;
                } else if (frameNumber == floor(totalProcessedFrameNumber*0.50)) {
                    std::cout << "AISAAC_LOG: Your video is 50% processed." << std::endl;
                } else if (frameNumber == floor(totalProcessedFrameNumber*0.75)) {
                    std::cout << "AISAAC_LOG: Your video is 75% processed." << std::endl;
                }
                frameNumber++;
            }
            std::cout << "AISAAC_LOG: Your video is 100% processed." << std::endl;
        } else {
            cv::Mat image = cv::imread(file, CV_LOAD_IMAGE_COLOR);

            if( !image.data ) {
                debugMsg("Unable to open file for reading");
                return -1;
            }

            mongoLink *mongoObject = new mongoLink(1);

            faceAnalytics->track(image, 1);

            if ( !faceAnalytics->blobContainer.size() ) {
                std::cout << "AISAAC_LOG: No faces were detected in the input image." << std::endl;
            } else {
                for (int i = 0; i < faceAnalytics->blobContainer.size(); i++) {
                    faceAnalytics->blobContainer[i].age = ageRecognition->detect(image);
                }
                mongoObject->ageAnalyticsMONGO(faceAnalytics->blobContainer, 1, resultTableId);
                std::cout << "AISAAC_LOG: Your image has been processed." << std::endl;
            }
        }
        return 0;
    } catch (...) {
        return -1;
    }
}
