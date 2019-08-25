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

#ifdef USE_MONGO
#include "../../tools/mongoLink/mongoLink.h"
#else
#include "../../tools/mongoLink/serialLink.h"
#endif //USE_MONGO

void jsonify(aiSaac::HumanAnalytics *humanAnalytics, int frameNumber, int FPS) {
    nlohmann::json humanAnalyticsJson;
    nlohmann::json humanAnalyticsBlobs;
    for (int i = 0; i < humanAnalytics->blobContainer.size(); i++) {
        nlohmann::json blobHuman;
        try {
            blobHuman = humanAnalyticsBlobs[std::to_string(humanAnalytics->blobContainer[i].ID)];
        } catch (std::out_of_range) {
            std::cout << "Doesn't exist" << std::endl;
        }
        blobHuman["shirtColor"] = humanAnalytics->blobContainer[i].shirtColor;
        blobHuman["startTime"] =
                ((double)humanAnalytics->blobContainer[i].firstFrameNumber)/FPS;
        blobHuman["endTime"] =
                ((double)humanAnalytics->blobContainer[i].lastFrameNumber)/FPS;
        nlohmann::json obj;

        if (humanAnalytics->blobContainer[i].lastFrameNumber == frameNumber) {
            obj["x"] = humanAnalytics->blobContainer[i].lastRectangle.x;
            obj["y"] = humanAnalytics->blobContainer[i].lastRectangle.y;
            obj["height"] = humanAnalytics->blobContainer[i].lastRectangle.height;
            obj["width"] = humanAnalytics->blobContainer[i].lastRectangle.width;
            obj["rectTime"] =
                    ((double)frameNumber)/
                    ((double)FPS);
            blobHuman["rectangles"].push_back(obj);
        }
        humanAnalyticsBlobs[std::to_string(humanAnalytics->blobContainer[i].ID)] = blobHuman;
    }
    humanAnalyticsJson["HumanAnalytics"] = humanAnalyticsBlobs;
    std::ofstream humanJsonFile;
    humanJsonFile.open("build/humanJSON.json");
    humanJsonFile << humanAnalyticsJson.dump(4);
    humanJsonFile.close();
}

int main( int argc, char *argv[]) {
    try {
        if ( argc < 4 ) {
            std::cout << "Usage: ./humanJsonifyExec [file] [aiSaacSettingsPath] [mongoResultTableID]" << std::endl;
            return -1;
        }
        std::string file = argv[1];
        std::string aiSaacSettingsPath = argv[2];
        std::string resultTableId = argv[3];
        bool annotateVideo = true;
        if (argc == 4) {
            annotateVideo = argv[4];
        }
        annotateVideo = true;

        aiSaac::AiSaacSettings *aiSaacSettings = new aiSaac::AiSaacSettings(aiSaacSettingsPath);
        aiSaac::HumanAnalytics *humanAnalytics = new aiSaac::HumanAnalytics(aiSaacSettings);

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
                S = fileStreamer->getFrameSize();
                annotatedVideo.open("/Users/pratik/AITOE/aisaac_lite/build/storageDir/annotated_video.avi", CV_FOURCC('M','J','P','G'), FPS, S, true);
                std::cout << "Opened Video file" << std::endl;
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
                    humanAnalytics->track(currentFrame, frameNumber);
                    // jsonify(humanAnalytics, frameNumber, FPS);
                    mongoObject->humanAnalyticsMONGO(humanAnalytics->blobContainer, frameNumber, resultTableId);
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
                    humanAnalytics->annotate(annotatedFrame);
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

            humanAnalytics->track(image, 1);

            if ( !humanAnalytics->blobContainer.size() ) {
                std::cout << "AISAAC_LOG: No humans were detected in the input." << std::endl;
            } else {
                mongoObject->humanAnalyticsMONGO(humanAnalytics->blobContainer, 1, resultTableId);
                std::cout << "AISAAC_LOG: Your image has been processed." << std::endl;
            }
        }
        return 0;
    } catch(...) {
        std::cout << "There was an exception" << std::endl;
        return -1;
    }
}
