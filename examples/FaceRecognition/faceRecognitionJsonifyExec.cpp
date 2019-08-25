#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <dirent.h>
#include <vector>
#include <signal.h>
#include <fstream>
#include "datk.h"
#include "../../src/aiSaacSettings.h"
#include "../../tools/videoSources/fileStreamer.h"

#ifdef USE_MONGO
#include "../../tools/mongoLink/mongoLink.h"
#else
#include "../../tools/mongoLink/serialLink.h"
#endif //USE_MONGO

nlohmann::json blobFace;
nlohmann::json FaceAnalyticsBlobs;
int frameNumber = 0;
int procFPS = 3;
volatile sig_atomic_t flag = 1;

void handler(int sig){ // can be called asynchronously
    flag = 0; // set flag
}

void saveJson(std::string outputFilePath) {
    nlohmann::json data;
    data["FaceRecognition"] = FaceAnalyticsBlobs;
    std::ofstream myFileJson;
    myFileJson.open(outputFilePath);
    myFileJson << data.dump(4) << std::endl;
    myFileJson.close();
}

void jsonify(std::vector<aiSaac::FaceBlob> blobContainer) {
    for (int i = 0; i < blobContainer.size(); i++) {
        try {
            blobFace = FaceAnalyticsBlobs[std::to_string(blobContainer[i].ID)];
        }
        catch (std::out_of_range) {
            std::cout << "Doesnt exist" <<std::endl;
        }
        blobFace["ID"] = blobContainer[i].ID;
        blobFace["startTime"] =
                ((double)blobContainer[i].firstFrameNumber)
                /((double)procFPS);
        blobFace["endTime"] =
                ((double)blobContainer[i].lastFrameNumber)
                /((double)procFPS);
        blobFace["label"] = blobContainer[i].label;
        nlohmann::json obj;

        if (blobContainer[i].lastFrameNumber == frameNumber) {
            obj["x"] = blobContainer[i].lastRectangle.x;
            obj["y"] = blobContainer[i].lastRectangle.y;
            obj["height"] = blobContainer[i].lastRectangle.height;
            obj["width"] = blobContainer[i].lastRectangle.width;
            obj["rectTime"] =
                    ((double)frameNumber)/
                    ((double)procFPS);
            blobFace["rectangles"].push_back(obj);
        }
        FaceAnalyticsBlobs[std::to_string(blobContainer[i].ID)] = blobFace;
    }
}

int main(int argc, char *argv[]) {
    // codeReview(Anurag): argument parser needed
    try {
        if (argc < 6) {
            std::cout << "usage: ./faceRecognitionExampleJsonify [filePath]  "
                << "[aiSaacSettingsPath] [caSettingsFilePath] [resultTableID] "
                << "[resultTableFeature] [isCropped(optional)]" << std::endl;
            return -1;
        }
        std::string file = argv[1];
        std::string aiSaacSettingsPath = argv[2];
        std::string caSettingsPath = argv[3];
        std::string resultTableId = argv[4];
        std::string resultTableFeature = argv[5];
        signal(SIGINT, handler);

        aiSaac::CASettings *caSettings = new aiSaac::CASettings(caSettingsPath);
        aiSaac::AiSaacSettings aiSaacSettings = aiSaac::AiSaacSettings(aiSaacSettingsPath);
        aiSaac::FaceAnalytics *faceAnalytics = new aiSaac::FaceAnalytics(aiSaacSettings);
        caSettings->setIsTrained(true);
        aiSaac::CustomAnalytics *ca1 = new aiSaac::CustomAnalytics(caSettings);
        // codeReview(Anurag): no need to train if DATK Model is ready
        ca1->train();

        if ( file.find("mp4") < file.length() ) {
            FileStreamer *fileStreamer = new FileStreamer(file);
            if (!fileStreamer->isStreaming()) {
               std::cout << "Unable to open file for reading" << std::endl;
               debugMsg("Unable to open file for reading");
               return -1;
            }
            cv::Mat currentFrame;
            int frameNumber = 0;
            int totalProcessedFrameNumber = 0;
            int FPS = fileStreamer->getFileFPS();
            int procFPS = aiSaacSettings.getProcFPS();
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
            while (true) {
                currentFrame = fileStreamer->getFrame();
                if( currentFrame.data == NULL || currentFrame.empty() || !fileStreamer->isStreaming()) {
                    break;
                }
                if (frameNumber % FPS / procFPS == 0) {
                    faceAnalytics->track(currentFrame, frameNumber);
                    for (int i = 0; i < faceAnalytics->blobContainer.size(); i++) {
                            faceAnalytics->blobContainer[i].label = ca1->detect(currentFrame(faceAnalytics->blobContainer[i].lastRectangle));
                    }
                    mongoObject->faceAnalyticsMONGO(faceAnalytics->blobContainer, frameNumber, resultTableId, resultTableFeature);
                }

                if (frameNumber == floor(totalProcessedFrameNumber*0.25)) {
                    std::cout << "AISAAC_LOG: Your video is 25% processed." << std::endl;
                } else if (frameNumber == floor(totalProcessedFrameNumber*0.5)) {
                    std::cout << "AISAAC_LOG: Your video is 50% processed." << std::endl;
                } else if (frameNumber == floor(totalProcessedFrameNumber*0.75)) {
                    std::cout << "AISAAC_LOG: Your video is 75% processed." << std::endl;
                }
                frameNumber++;
                std::cout << "AISAAC_LOG: Your video is 100% processed." << std::endl;
            }
        } else {
            bool isCropped;
            cv::Mat image = cv::imread(file, CV_LOAD_IMAGE_COLOR);

            if( !image.data ) {
                debugMsg("Unable to open file for reading");
                return -1;
            }

            if (argc >= 7) {
                std::string param = argv[6];
                if (param == "true")
                    isCropped = true;
                else
                    isCropped = false;
            }
            else
                isCropped = false;

            mongoLink *mongoObject = new mongoLink(1);
            if (!isCropped) {
                std::cout << "AISAAC_LOG: Detecting faces" << std::endl;
                faceAnalytics->track(image, 1);

                if ( !faceAnalytics->blobContainer.size() ) {
                    std::cout << "AISAAC_LOG: No faces were detected in the input." << std::endl;
                }else {
                    for (int i = 0; i < faceAnalytics->blobContainer.size(); i++) {
                        faceAnalytics->blobContainer[i].label = ca1->detect(image(faceAnalytics->blobContainer[i].lastRectangle));
                    }
                    mongoObject->faceAnalyticsMONGO(faceAnalytics->blobContainer, 1, resultTableId, resultTableFeature);
                    std::cout << "AISAAC_LOG: Your image has been processed." << std::endl;
                }
            }
            else {
                std::cout << "AISAAC_LOG: Detecting faces" << std::endl;
                std::vector<aiSaac::FaceBlob> blobContainerSingle;
                aiSaac::FaceBlob imageFace;
                imageFace.ID = 1;
                imageFace.firstFrameNumber = 1;
                imageFace.lastFrameNumber = 1;
                imageFace.frameCount = 1;
                imageFace.label = ca1->detect(image);
                cv::Rect imageRect(0, 0, image.cols, image.rows);
                imageFace.firstRectangle = imageRect;
                imageFace.lastRectangle = imageRect;
                blobContainerSingle.push_back(imageFace);
                std::cout << "AISAAC_RESULT: " << imageFace.label << std::endl;
                mongoObject->faceAnalyticsMONGO(blobContainerSingle, 1, resultTableId, resultTableFeature);
                std::cout << "AISAAC_LOG: Your image has been processed." << std::endl;
            }
        }
        return 0;
    } catch (...) {
        return -1;
    }
}
