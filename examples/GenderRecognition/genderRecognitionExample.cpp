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
// #include "../../tools/mongoLink/mongoLink.h"

int main(int argc, char *argv[]) {
    try {
        if ( argc < 3 ) {
            std::cout << "Usage: ./genderRecognitionJsonifyExec [file] [aiSaacSettingsPath] [annotatedVideo] confidenceThreshold" << std::endl;
            return -1;
        }
        std::string file = argv[1];
        std::string aiSaacSettingsPath = argv[2];

        double confidenceThreshold = std::atof(argv[4]);

        aiSaac::AiSaacSettings aiSaacSettings = aiSaac::AiSaacSettings(aiSaacSettingsPath);
        aiSaac::FaceAnalytics faceAnalytics = aiSaac::FaceAnalytics(aiSaacSettings);
        aiSaac::GenderRecognition genderRecognition = aiSaac::GenderRecognition(aiSaacSettings);

        if ( file.find("mp4") < file.length() ) {
            FileStreamer *fileStreamer = new FileStreamer(file);
            if (!fileStreamer->isStreaming()) {
                debugMsg("Unable to open file for reading");
                return -1;
            }

            std::cout << "File opened" << std::endl;
            cv::Mat currentFrame, annotatedFrame;
            cv::VideoWriter annotatedVideo;
            int frameNumber = 0;
            int totalProcessedFrameNumber = fileStreamer->getNumberOfFrames();
            int FPS = fileStreamer->getFileFPS();
            int procFPS = aiSaacSettings.getProcFPS();
            // mongoLink *mongoObject = new mongoLink(FPS);
            std::vector<cv::Rect> localizedRectangles = std::vector<cv::Rect>();

            std::cout << "Opening annotated video" << std::endl;
            if (argc == 4) {
                cv::Size S = fileStreamer->getFileFrameSize();
                annotatedVideo.open(argv[3], CV_FOURCC('M','J','P','G'), FPS, S, true);
            }

            if ( procFPS < 1 ) {
                procFPS = 1;
            } else if ( procFPS > FPS ) {
                procFPS = FPS;
            }

            std::cout << "Analyzing frames" << std::endl;
            while (true) {
                std::cout << "Getting frame" << std::endl;
                currentFrame = fileStreamer->getFrame();
                if( currentFrame.data == NULL || currentFrame.empty() || !fileStreamer->isStreaming()) {
                    break;
                }
                if (frameNumber % (FPS / procFPS) == 0) {
                    std::cout << "Performing track" << std::endl;
                    faceAnalytics.track(currentFrame, frameNumber);
                    std::cout << "Assigning gender labels to blobs" << std::endl;
                    for (int i = 0; i < faceAnalytics.blobContainer.size(); i++) {
                        if (faceAnalytics.blobContainer[i].lastFrameNumber == frameNumber) {
                            double confidence;
                            std::string genderLabel = genderRecognition.detect(faceAnalytics.blobContainer[i].faceImages.back(),
                                confidence);
                            std::cout << faceAnalytics.blobContainer[i].ID << ":" << "confidence: " << confidence << std::endl;
                            if (confidence >= confidenceThreshold) {
                                faceAnalytics.blobContainer[i].gender = genderLabel;
                            } else {
                                faceAnalytics.blobContainer[i].gender = "unassigned";
                            }
                        }
                    }
                    // mongoObject->genderAnalyticsMONGO(faceAnalytics.blobContainer, frameNumber, resultTableId);
                }

                if (frameNumber == floor(totalProcessedFrameNumber*0.25)) {
                    std::cout << "AISAAC_LOG: Your video is 25% processed." << std::endl;
                } else if (frameNumber == floor(totalProcessedFrameNumber*0.5)) {
                    std::cout << "AISAAC_LOG: Your video is 50% processed." << std::endl;
                } else if (frameNumber == floor(totalProcessedFrameNumber*0.75)) {
                    std::cout << "AISAAC_LOG: Your video is 75% processed." << std::endl;
                }

                if ( argc == 5 ) {
                    std::cout << "Annotating frame" << std::endl;
                    annotatedFrame = currentFrame;
                    faceAnalytics.annotate(annotatedFrame);
                    if (annotatedFrame.data != NULL) {
                        cv::imshow("Annotated Gender", annotatedFrame);
                        cv::waitKey(10);
                        annotatedVideo.write(annotatedFrame);
                    }
                }
                frameNumber++;
            }
            std::cout << "Releasing annotated video" << std::endl;
            if ( argc == 4 ) {
                annotatedVideo.release();
            }
            std::cout << "AISAAC_LOG: Your video is 100% processed." << std::endl;
        } else {
            cv::Mat image = cv::imread(file, CV_LOAD_IMAGE_COLOR);

            if( !image.data ) {
                debugMsg("Unable to open file for reading");
                return -1;
            }

            // mongoLink *mongoObject = new mongoLink(1);

            faceAnalytics.track(image, 1);

            if ( !faceAnalytics.blobContainer.size() ) {
                std::cout << "AISAAC_LOG: No faces were detected in the input image." << std::endl;
            } else {
                for (int i = 0; i < faceAnalytics.blobContainer.size(); i++) {
                    faceAnalytics.blobContainer[i].gender = genderRecognition.detect(image);
                }
                // mongoObject->genderAnalyticsMONGO(faceAnalytics.blobContainer, 1, resultTableId);
                std::cout << "AISAAC_LOG: Your image has been processed." << std::endl;
            }
        }
        return 0;
    } catch (...) {
        return -1;
    }
}
