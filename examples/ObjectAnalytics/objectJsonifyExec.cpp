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
#include "../../src/utils/colorClassifier.h"

#ifdef USE_MONGO
#include "../../tools/mongoLink/mongoLink.h"
#else
#include "../../tools/mongoLink/serialLink.h"
#endif //USE_MONGO


void jsonify(aiSaac::ObjectAnalytics *objectAnalytics, int frameNumber, int FPS) {
    nlohmann::json objectAnalyticsJson;
    nlohmann::json ObjectAnalyticsBlobs;
    for (int i = 0; i < objectAnalytics->blobContainer.size(); i++) {
        nlohmann::json blobObject;
        try {
            blobObject = ObjectAnalyticsBlobs[std::to_string(objectAnalytics->blobContainer[i].ID)];
        } catch (std::out_of_range) {
            std::cout << "Doesn't exist" << std::endl;
        }
        blobObject["label"] = objectAnalytics->blobContainer[i].label;
        blobObject["startTime"] =
                ((double)objectAnalytics->blobContainer[i].firstFrameNumber)/FPS;
        blobObject["endTime"] =
                ((double)objectAnalytics->blobContainer[i].lastFrameNumber)/FPS;
        nlohmann::json obj;

        if (objectAnalytics->blobContainer[i].lastFrameNumber == frameNumber) {
            obj["x"] = objectAnalytics->blobContainer[i].lastRectangle.x;
            obj["y"] = objectAnalytics->blobContainer[i].lastRectangle.y;
            obj["height"] = objectAnalytics->blobContainer[i].lastRectangle.height;
            obj["width"] = objectAnalytics->blobContainer[i].lastRectangle.width;
            obj["rectTime"] =
                    ((double)frameNumber)/
                    ((double)FPS);
            blobObject["rectangles"].push_back(obj);
        }
        ObjectAnalyticsBlobs[std::to_string(objectAnalytics->blobContainer[i].ID)] = blobObject;
    }
    objectAnalyticsJson["ObjectAnalytics"] = ObjectAnalyticsBlobs;
    std::ofstream objectJsonFile;
    objectJsonFile.open("build/objectJSON.json");
    objectJsonFile << objectAnalyticsJson.dump(4);
    objectJsonFile.close();
}

int main( int argc, char *argv[]) {
    try {
        if ( argc < 4 ) {
            std::cout << "Usage: ./objectJsonifyExec [file] [aiSaacSettingsPath] [mongoResultTableID]" << std::endl;
            return -1;
        }
        std::string file = argv[1];
        std::string aiSaacSettingsPath = argv[2];
        std::string resultTableId = argv[3];

        aiSaac::AiSaacSettings *aiSaacSettings = new aiSaac::AiSaacSettings(aiSaacSettingsPath);
        aiSaac::ObjectAnalytics *objectAnalytics = new aiSaac::ObjectAnalytics(*aiSaacSettings);
        aiSaac::ColorClassifier *colorClassifier = new aiSaac::ColorClassifier();

        if ( file.find("mp4") < file.length() ) {
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
            mongoLink *mongoObject = new mongoLink(FPS);
            cv::Size S;

            std::cout << "here2" << std::endl;

            if ( annotateVideo ) {
                S = fileStreamer->getFileFrameSize();
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
                    objectAnalytics->track(currentFrame, frameNumber);
                    // jsonify(objectAnalytics, frameNumber, FPS);
                    for(int i = 0; i < objectAnalytics->blobContainer.size(); i++) {
                        if (objectAnalytics->blobContainer[i].lastFrameNumber == frameNumber) {
                            std::string color =
                                colorClassifier->patchColor(
                                    currentFrame,
                                    objectAnalytics->blobContainer[i].lastRectangle
                                );
                            objectAnalytics->blobContainer[i].metaData =
                                color + " " + objectAnalytics->blobContainer[i].label;
                                std::cout << "Color from Blob: " << objectAnalytics->blobContainer[i].metaData << std::endl;
                        }
                    }
                    mongoObject->objectAnalyticsMONGO(objectAnalytics->blobContainer, frameNumber, resultTableId);
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
                    objectAnalytics->annotate(annotatedFrame);
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

            objectAnalytics->track(image, 1);

            if ( !objectAnalytics->blobContainer.size() ) {
                std::cout << "AISAAC_LOG: No objects were detected in the input." << std::endl;
            } else {
                mongoObject->objectAnalyticsMONGO(objectAnalytics->blobContainer, 1, resultTableId);
                std::cout << "AISAAC_LOG: Your image has been processed." << std::endl;
            }
        }
        return 0;
    } catch(...) {
        std::cout << "There was an exception" << std::endl;
        return -1;
    }
}
