#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <dirent.h>
#include <vector>
#include <signal.h>
#include <fstream>
#include <chrono>
#include "../../src/CustomAnalytics/caSettings.h"
#include "../../src/CustomAnalytics/customAnalytics.h"
#include "../../src/ObjectAnalytics/objectAnalytics.h"
#include "../../src/aiSaacSettings.h"
#include "../../src/utils/datk/src/datk.h"
#include "../../src/aiSaacSettings.h"
#include "../../tools/videoSources/fileStreamer.h"

int main(int argc, char const *argv[]) {

    if (argc < 3) {
        std::cout << "Usage: humanAttributeRecognitionExample [videoFilePath] [outputFilePath] [aiSaacSettingsPath] [caSettingsFilePath]" << std::endl;
        return -1;
    }

    std::string videoFilePath = argv[1];
    std::string outputFilePath = argv[2];
    std::string aiSaacSettingsPath = argv[3];
    std::string caSettingsFilePath = argv[4];

    cv::Mat currentFrame;
    cv::Mat annotatedFrame;

    aiSaac::AiSaacSettings* aiSaacSettings = new aiSaac::AiSaacSettings(aiSaacSettingsPath);
    aiSaac::ObjectAnalytics* objectAnalytics = new aiSaac::ObjectAnalytics(*aiSaacSettings);
    aiSaac::CASettings* caSettings = new aiSaac::CASettings(caSettingsFilePath);
    aiSaac::CustomAnalytics* ca1 = new aiSaac::CustomAnalytics(caSettings);

    std::cout << "Performing humanAttributeRecognition on: " << videoFilePath << std::endl;
    std::cout << "Calling CustomAnalytics train" << std::endl;
    ca1->train();
    std::cout << "Completed CustomAnalytics train" << std::endl;

    FileStreamer* fileStreamer = new FileStreamer(videoFilePath);
    if (!fileStreamer->isStreaming()) {
       std::cout << "Unable to open file for reading" << std::endl;
       return -1;
    }

    int FPS = fileStreamer->getFileFPS();
    int procFPS = aiSaacSettings->getProcFPS();
    int frameNumber = 0;
    int fontFace = cv::FONT_HERSHEY_SIMPLEX;
    double fontScale = 1;
    int thickness = 2;
    std::vector<std::string> labels;
    // std::vector<cv::Rect> localizedHumans;

    while (!fileStreamer->isStreaming()) {
        // Waiting for file to start streaming.
    }

    cv::VideoWriter outputFile = cv::VideoWriter(outputFilePath, CV_FOURCC('M','J','P','G'), procFPS, cv::Size(320, 180), true);

    while (true) {
        currentFrame = fileStreamer->getFrame();
        frameNumber++;
        if( currentFrame.data == NULL || currentFrame.empty() || !fileStreamer->isStreaming()) {
            break;
        }
        labels.clear();
        // localizedHumans.clear();
        if (frameNumber % FPS / procFPS == 0) {
            cv::resize(currentFrame, currentFrame, cv::Size(320, 180));
            auto locStart = std::chrono::high_resolution_clock::now();
            objectAnalytics->track(currentFrame, frameNumber);
            auto locEnd = std::chrono::high_resolution_clock::now();
            std::cout << "Time taken to localise: " << std::chrono::duration_cast<std::chrono::milliseconds>(locEnd - locStart).count() << std::endl;

            for (int i = 0; i < objectAnalytics->blobContainer.size(); i++) {
                std::cout << "Inside Recognise Loop" << std::endl;
                if (objectAnalytics->blobContainer[i].lastFrameNumber == frameNumber) {
                    if (objectAnalytics->blobContainer[i].label == "person") {
                        auto recStart = std::chrono::high_resolution_clock::now();
                        std::string label = ca1->detect(currentFrame(objectAnalytics->blobContainer[i].lastRectangle));
                        auto recEnd = std::chrono::high_resolution_clock::now();
                        std::cout << "Time taken to recognise: " << std::chrono::duration_cast<std::chrono::milliseconds>(recEnd - recStart).count() << std::endl;
                        labels.push_back(label);
                    }
                }
            }

            annotatedFrame = currentFrame;
            if (annotatedFrame.data != NULL) {
                int labelCount = -1;
                std::cout << "Annotating frame " << frameNumber << std::endl << std::flush;
                for (int i = 0; i < objectAnalytics->blobContainer.size(); i++) {
                    if (objectAnalytics->blobContainer[i].label == "person") {
                        cv::rectangle(annotatedFrame, objectAnalytics->blobContainer[i].lastRectangle, cv::Scalar(255, 255, 255), 2);
                        // labelCount++;
                        // std::string label = labels[labelCount];
                        // int baseline = 0;
                        // cv::Size textSize = cv::getTextSize(label, fontFace, fontScale, thickness, &baseline);
                        // baseline += thickness;
                        // cv::Point textOrg(objectAnalytics->blobContainer[i].lastRectangle.x + (objectAnalytics->blobContainer[i].lastRectangle.width / 2) - (textSize.width / 2), objectAnalytics->blobContainer[i].lastRectangle.y - 5);
                        // if (textOrg.x < 0) {
                        //     textOrg.x = 0;
                        // }
                        // if (textOrg.y < 0) {
                        //     textOrg.y = 0;
                        // }
                        // cv::putText(annotatedFrame, label, textOrg, fontFace, fontScale, cv::Scalar(127, 255, 0), thickness);
                    }
                }

                cv::imshow("Annotated Object", annotatedFrame);
                cv::waitKey(10);
                // if (argc > 2) {
                    outputFile.write(annotatedFrame);
                // }
          }
        }
    }
    outputFile.release();
    std::cout << "Released annotated video." << std::endl;

    delete fileStreamer;
    delete ca1;
    delete caSettings;
    delete objectAnalytics;
    delete aiSaacSettings;

    return 0;
}
