#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <dirent.h>
#include <vector>
#include <signal.h>
#include <fstream>
#include <chrono>
//#include <ctime>
#include "../../src/CustomAnalytics/caSettings.h"
#include "../../src/CustomAnalytics/customAnalytics.h"
#include "../../src/FaceAnalytics/faceAnalytics.h"
#include "../../src/aiSaacSettings.h"
#include "../../src/utils/datk/src/datk.h"
#include "../../src/aiSaacSettings.h"
#include "../../tools/videoSources/fileStreamer.h"

int main(int argc, char const *argv[]) {

    if (argc < 3) {
        std::cout << "Usage: faceRecognitionSSPExec [videoFilePath] [outputFilePath] [aiSaacSettingsPath] [caSettingsFilePath]" << std::endl;
        return -1;
    }

    std::string videoFilePath = argv[1];
    std::string outputFilePath = argv[2];
    std::string aiSaacSettingsPath = argv[3];
    std::string caSettingsFilePath = argv[4];

    cv::Mat readFrame;
    cv::Mat currentFrame;
    cv::Mat annotatedFrame;

    aiSaac::AiSaacSettings* aiSaacSettings = new aiSaac::AiSaacSettings(aiSaacSettingsPath);
    aiSaac::FaceAnalytics* faceAnalytics = new aiSaac::FaceAnalytics(*aiSaacSettings);
    aiSaac::CASettings* caSettings = new aiSaac::CASettings(caSettingsFilePath);
    aiSaac::CustomAnalytics* ca1 = new aiSaac::CustomAnalytics(caSettings);

    std::cout << "Performing FaceRecognition on: " << videoFilePath << std::endl;
    std::cout << "Calling CustomAnalytics train" << std::endl;
    auto trainStart = std::chrono::high_resolution_clock::now();
    ca1->train();
    auto trainEnd = std::chrono::high_resolution_clock::now();
    std::cout << "Completed CustomAnalytics train" << std::endl;
    std::cout << "Time taken to train: " << std::chrono::duration_cast<std::chrono::milliseconds>(trainEnd - trainStart).count() << std::endl << std::flush;

    FileStreamer* fileStreamer = new FileStreamer(videoFilePath);
    if (!fileStreamer->isStreaming()) {
       std::cout << "Unable to open file for reading" << std::endl;
       return -1;
    }

    int FPS = fileStreamer->getFileFPS();
    int procFPS = aiSaacSettings->getProcFPS();
    cv::Size S = fileStreamer->getFrameSize();
    int frameNumber = 0;
    int fontFace = cv::FONT_HERSHEY_SIMPLEX;
    double fontScale = 1;
    int thickness = 2;

    if ( procFPS < 1 ) {
        procFPS = 1;
    } else if ( procFPS > FPS ) {
        procFPS = FPS;
    }
    int k = procFPS / 10; //hyperparam for recognition repetition
    if (k < 1) {
      k = 1;
    }

    while (!fileStreamer->isStreaming()) {
        // Waiting for file to start streaming.
    }

    //cv::VideoWriter outputFile = cv::VideoWriter(outputFilePath, CV_FOURCC('M','J','P','G'), procFPS, cv::Size(320, 180), true);
    cv::VideoWriter outputFile = cv::VideoWriter(outputFilePath, CV_FOURCC('M','J','P','G'), procFPS, S, true);

    auto inferenceStart = std::chrono::high_resolution_clock::now();
    while (true) {
        readFrame = fileStreamer->getFrame();
        frameNumber++;
        if( readFrame.data == NULL || readFrame.empty() || !fileStreamer->isStreaming()) {
            break;
        }
        std::cout << "before resize" << std::endl << std::flush;
        //cv::resize(readFrame, currentFrame, cv::Size(320, 180));
        cv::resize(readFrame, currentFrame, S);
        std::cout << "readframe: (" << readFrame.cols << "," << readFrame.rows << ")" << std::endl << std::flush;
        std::cout << "currentframe: (" << currentFrame.cols << "," << currentFrame.rows << ")" << std::endl << std::flush;
        // localizedHumans.clear();
        if (frameNumber % (FPS / procFPS) == 0) {
            auto locStart = std::chrono::high_resolution_clock::now();
            std::cout << "before track" << std::endl << std::flush;
            faceAnalytics->track(currentFrame, frameNumber);
            auto locEnd = std::chrono::high_resolution_clock::now();
            std::cout << "Time taken to localise and track: " << std::chrono::duration_cast<std::chrono::milliseconds>(locEnd - locStart).count() << std::endl << std::flush;

            for (int i = 0; i < faceAnalytics->blobContainer.size(); i++) {
                if ((faceAnalytics->blobContainer[i].lastFrameNumber == frameNumber) && ((frameNumber % (FPS / k) == 0) || (faceAnalytics->blobContainer[i].frameCount == 1))) {
                  std::cout << "Recognising ..." << std::endl << std::flush;
                  auto recStart = std::chrono::high_resolution_clock::now();
                  std::string label = ca1->detect(currentFrame(faceAnalytics->blobContainer[i].lastRectangle));
                  std::cout << "Recognised: " << label << std::endl << std::flush;
                  auto recEnd = std::chrono::high_resolution_clock::now();
                  std::cout << "Time taken to recognise: " << std::chrono::duration_cast<std::chrono::milliseconds>(recEnd - recStart).count() << std::endl << std::flush;
                  faceAnalytics->blobContainer[i].label = label;
                }
            }
        }
        annotatedFrame = currentFrame.clone();
        std::cout << "currentframe: (" << currentFrame.cols << "," << currentFrame.rows << ")" << std::endl << std::flush;
        std::cout << "annotatedframe: (" << annotatedFrame.cols << "," << annotatedFrame.rows << ")" << std::endl << std::flush;
        std::cout << "before annotate" << std::endl << std::flush;
        faceAnalytics->annotate(annotatedFrame);
        cv::imshow("Annotated Object", annotatedFrame);
        cv::waitKey(2);
        std::cout << "before write" << std::endl << std::flush;
        outputFile.write(annotatedFrame);
    }
    auto inferenceEnd = std::chrono::high_resolution_clock::now();
    std::cout << "Time taken for inference on video: " << std::chrono::duration_cast<std::chrono::milliseconds>(inferenceEnd - inferenceStart).count() << std::endl << std::flush;
    outputFile.release();
    std::cout << "Released annotated video." << std::endl;

    delete fileStreamer;
    delete ca1;
    delete caSettings;
    delete aiSaacSettings;

    return 0;
}
