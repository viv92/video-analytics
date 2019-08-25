#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <dirent.h>
#include <vector>
#include <signal.h>
#include <fstream>
#include "../../src/aiSaac.h"
#include "datk.h"
#include "../../src/aiSaacSettings.h"
#include "../../tools/videoSources/fileStreamer.h"

volatile sig_atomic_t flag = 1;

void handler(int sig){ // can be called asynchronously
  flag = 0; // set flag
}

void analyticsFunction(FileStreamer *videoStreamer,
    aiSaac::AiSaacSettings *aiSaacSettings,
    aiSaac::CustomAnalytics *ca1) {
    std::string outputFilePath = storagePath + "/data/FaceRecData1/friends.mp4";
    cv::VideoWriter outputFile;
    outputFile.open(outputFilePath.c_str(),
        videoStreamer->getFourCC(),
        videoStreamer->getFileFPS(),
        videoStreamer->getFileFrameSize(),
        true);
    aiSaac::BgSubAnalytics *bgSubAnalytics = new aiSaac::BgSubAnalytics(aiSaacSettings);
    aiSaac::FaceAnalytics *faceAnalytics = new aiSaac::FaceAnalytics(aiSaacSettings);
    cv::Mat currentFrame;
    int frameNumber = 0;
    cv::namedWindow("Display window", CV_WINDOW_AUTOSIZE);
    int fontFace = cv::FONT_HERSHEY_SIMPLEX;
    double fontScale = 1;
    int thickness = 2;
    int count;
    for (count = 0; count < videoStreamer->getFileFPS() * 10; count++) {
        videoStreamer->getFrame();
    }
    while (flag) {
        count++;
        if(!videoStreamer->isStreaming()) {
            std::cout << "Seems to be done" <<std::endl;
            break;
        }

        currentFrame = videoStreamer->getFrame();
        if(currentFrame.data == NULL) {
            std::cout << "Seems to be done" <<std::endl;
            break;
        }

        // std::vector<cv::Rect> localizedRectangles = std::vector<cv::Rect>();

        clock_t time;
        time = clock();

        // bgSubAnalytics->localize(currentFrame, frameNumber, localizedRectangles);

        // std::cout << "BG SUB Prediction Time: " << sec(clock() - time) << std::endl;
        time = clock();

        std::vector<cv::Rect> allFaces = std::vector<cv::Rect>();
        std::vector<std::string> labels = std::vector<std::string>();

        std::vector<cv::Rect> localizedFaces = std::vector<cv::Rect>();
        faceAnalytics->localize(currentFrame, localizedFaces);

        for (int j = 0; j < localizedFaces.size(); j++) {
            // TODO c1.detect and annotate
            int topLeftX = localizedFaces[j].x
                - (localizedFaces[j].width / 4);
            (topLeftX >= 0) ? topLeftX = topLeftX : topLeftX = 0;
            int topLeftY = localizedFaces[j].y
                - (localizedFaces[j].height / 2);
            (topLeftY >= 0) ? topLeftY = topLeftY : topLeftY = 0;
            cv::Point topLeft(topLeftX, topLeftY);

            int bottomRightX = localizedFaces[j].x
                + localizedFaces[j].width
                + (localizedFaces[j].width / 4);
            (bottomRightX <= currentFrame.cols) ? bottomRightX = bottomRightX :
                bottomRightX = currentFrame.cols;
            int bottomRightY = localizedFaces[j].y
                + localizedFaces[j].height +
                (localizedFaces[j].height / 4);
            (bottomRightY <= currentFrame.rows) ? bottomRightY = bottomRightY :
                bottomRightY = currentFrame.rows;
            cv::Point bottomRight(bottomRightX, bottomRightY);
            cv::Rect largerLocalization(topLeft, bottomRight);
            std::string label = ca1->detect(currentFrame(largerLocalization));

            if (label != "Unknown") {
                allFaces.push_back(largerLocalization);
                labels.push_back(label);
            }
        }
        std::cout << "FACE Prediction Time: " << sec(clock() - time) << std::endl;
        frameNumber++;
        cv::Mat annotatFrame;
        for (int i = 4; i > 0; i--) {
            if (!videoStreamer->isStreaming()) {
                std::cout << "Seems to be done" <<std::endl;
                break;
            }
            annotatFrame = videoStreamer->getFrame();
            if (annotatFrame.data == NULL) {
                std::cout << "Seems to be done" <<std::endl;
                break;
            }
            if (annotatFrame.data != NULL) {
                std::cout << "Annotating frame " <<frameNumber << std::endl << std::flush;
                for (int c = 0; c < allFaces.size(); c++) {
                    cv::rectangle(annotatFrame, allFaces[c], cv::Scalar(255, 255, 255), 2);
                    std::string label = labels[c];
                    int baseline = 0;
                    cv::Size textSize = cv::getTextSize(label,
                        fontFace,
                        fontScale,
                        thickness,
                        &baseline);
                    baseline += thickness;
                    cv::Point textOrg(allFaces[c].x
                        + (allFaces[c].width / 2)
                        - (textSize.width / 2),
                        allFaces[c].y - 5);
                    if (textOrg.x < 0) {
                        textOrg.x = 0;
                    }
                    if (textOrg.y < 0) {
                        textOrg.y = 0;
                    }
                    cv::putText(annotatFrame,
                        label,
                        textOrg,
                        fontFace,
                        fontScale,
                        cv::Scalar(127, 255, 0),
                        thickness);
                }
                cv::imshow("Display window", annotatFrame);
                outputFile.write(annotatFrame);
                cv::waitKey(2);
            }
            std::cout << "video streamer fps * 190 " << videoStreamer->getFileFPS() * 190 << std::endl;
            // if (count >= videoStreamer->getFileFPS() * 139) {
            //     break;
            // }
        }
    }
    outputFile.release();
}



int main(int argc, char *argv[]) {
    signal(SIGINT, handler);
    aiSaac::CASettings *caSettings = new aiSaac::CASettings(storagePath, "FaceRecognition");
    caSettings->setTrainingDataPath(storagePath + "/data/FaceRecData1/Train");
    caSettings->setTestingDataPath(storagePath + "/data/FaceRecData1/Test");
    caSettings->setLabelFilePath(storagePath + "/data/FaceRecData1/labels.txt");
    caSettings->setTrainingJsonDataPath(storagePath
        + "/data/FaceRecognitionJSON/trainingData_VGG.json");
    caSettings->setTestingJsonDataPath(storagePath
        + "/data/FaceRecognitionJSON/testingData_VGG.json");
    caSettings->setIsTrained(true);
    caSettings->setIsTested(false);
    caSettings->setDATKSaveModelPath(storagePath + "/data/FaceRecData1/FaceRecModel.datk");
    aiSaac::CustomAnalytics *ca1 = new aiSaac::CustomAnalytics(caSettings);
    ca1->train();
    // ca1.loadDatkModel();
    //std::string label = ca1.detect(cv::imread(
    //    "/Users/anurag/Documents/projects/git-stuff/temp-stash/aiSaac/build/storageDir/faceStorage/Test/10/face-4.png"));

    debugMsg("Camera Initialized\n");
    if (argc > 1) {
        filePath = argv[1];
        if (argc > 2) {
            storagePath = argv [2];
        }
        debugMsg("Developer Name " + developerName);
        debugMsg("RTSP URL "+rtspURL);
    }

    // Use file Streamer
     FileStreamer *cam = new FileStreamer(filePath);
     if (!cam->isStreaming()) {
        std::cout << "Unable to open file for reading" << std::endl;
        debugMsg("Unable to open file for reading");
        return 0;
     }
     std::cout << "File Frame rate" << cam->getFileFPS() << std::endl;

    while (!cam->isStreaming()) {}

    std::cout << "camera running now " << cam->getFrame().size() << std::endl;

    aiSaac::AiSaacSettings *aiSaacSettings = new aiSaac::AiSaacSettings(storagePath, "config1");
    analyticsFunction(cam, aiSaacSettings, ca1);
    return 0;
}
