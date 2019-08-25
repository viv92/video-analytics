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
#include "../../tools/videoSources/webcamStreamer.h"
// #include "../../tools/mongoLink/mongoLink.h"

int main(int argc, char *argv[]) {
    try {
        if ( argc < 4 ) {
            std::cout << "Usage: ./emotionRecognitionExample [file] [aiSaacSettingsPath] [annotatedVideo] *confidenceThreshold*\n";
            return -1;
        }
        std::string file = argv[1];
        std::string aiSaacSettingsPath = argv[2];

        //double confidenceThreshold = std::atof(argv[4]);
        aiSaac::AiSaacSettings aiSaacSettings = aiSaac::AiSaacSettings(aiSaacSettingsPath);
        aiSaac::FaceAnalytics faceAnalytics = aiSaac::FaceAnalytics(aiSaacSettings);
        aiSaac::EmotionRecognition emotionRecognition = aiSaac::EmotionRecognition(aiSaacSettings);

        if ( file.find("mp4") < file.length() )
        {
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
                cv::Size S = fileStreamer->getFileFrameSize();
                annotatedVideo.open(argv[3], CV_FOURCC('M','J','P','G'), FPS, S, true);

            if ( procFPS < 1 ) {
                procFPS = 1;
            } else if ( procFPS > FPS ) {
                procFPS = FPS;
            }

            fileStreamer = new FileStreamer(file);
            while(true) {
                currentFrame = fileStreamer->getFrame();
                if( currentFrame.data == NULL || currentFrame.empty() || !fileStreamer->isStreaming()) {
                    break;
                }
                if (frameNumber % FPS / procFPS == 0) {
                    faceAnalytics.track(currentFrame, frameNumber);
                    for (int i = 0; i < faceAnalytics.blobContainer.size(); i++) {
                        if (faceAnalytics.blobContainer[i].lastFrameNumber == frameNumber) {
                            //double confidence;
                            //std::string emotionLabel = emotionRecognition.detect(faceAnalytics.blobContainer[i].faceImages.back(),
                            //    confidence);
                            std::string emotionLabel = emotionRecognition.detect(faceAnalytics.blobContainer[i].faceImages.back());
                            //std::cout << faceAnalytics.blobContainer[i].ID << ":" << "confidence: " << confidence << std::endl;
                            //if (confidence >= confidenceThreshold) {
                                faceAnalytics.blobContainer[i].emotion = emotionLabel;
                            //} else {
                                //faceAnalytics.blobContainer[i].emotion = "unassigned";
                            //}
                        }
                    }
                    // mongoObject->emotionAnalyticsMONGO(faceAnalytics.blobContainer, frameNumber, resultTableId);
                }

                if (frameNumber == floor(totalProcessedFrameNumber*0.25)) {
                    std::cout << "AISAAC_LOG: Your video is 25% processed." << std::endl;
                } else if (frameNumber == floor(totalProcessedFrameNumber*0.50)) {
                    std::cout << "AISAAC_LOG: Your video is 50% processed." << std::endl;
                } else if (frameNumber == floor(totalProcessedFrameNumber*0.75)) {
                    std::cout << "AISAAC_LOG: Your video is 75% processed." << std::endl;
                }

                    std::cout << "Annotating frame" << std::endl;
                    annotatedFrame = currentFrame;
                    faceAnalytics.annotate(annotatedFrame);
                    if (annotatedFrame.data != NULL) {
                        cv::imshow("Annotated emotion", annotatedFrame);
                        cv::waitKey(10);
                        annotatedVideo.write(annotatedFrame);
                    }

                frameNumber++;
            }
            std::cout << "AISAAC_LOG: Your video is 100% processed." << std::endl;
        } else {
            cv::Mat image = cv::imread(file, CV_LOAD_IMAGE_COLOR);
            cv::Mat annotatedImage = image;

            if( !image.data ) {
                debugMsg("Unable to open file for reading");
                return -1;
            }

            // mongoLink *mongoObject = new mongoLink(1);
            faceAnalytics.track(image, 1);
            std::cout << "faceAnalytics.blobContainer.size: " << faceAnalytics.blobContainer.size() << std::endl << std::flush;

            if ( !faceAnalytics.blobContainer.size() ) {
                std::cout << "AISAAC_LOG: No faces were detected in the input image." << std::endl;
            } else {
                for (int i = 0; i < faceAnalytics.blobContainer.size(); i++) {
                    faceAnalytics.blobContainer[i].emotion = emotionRecognition.detect(image);
                }
                faceAnalytics.annotate(annotatedImage,1);
                    if (annotatedImage.data != NULL) {
                        cv::imshow("Annotated emotion", annotatedImage);
                        cv::waitKey(0);
                }
                // mongoObject->emotionAnalyticsMONGO(faceAnalytics.blobContainer, 1, resultTableId);
                std::cout << "AISAAC_LOG: Your image has been processed." << std::endl;
            }
        }
        return 0;
    } catch (...) {
        return -1;
    }
}
