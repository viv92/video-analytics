#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <dirent.h>
#include <vector>
#include <fstream>
#include "../../src/aiSaac.h"
#include "datk.h"
#include "../../tools/videoSources/fileStreamer.h"

aiSaac::BgSubAnalytics *bgSubAnalytics = NULL;
aiSaac::FaceAnalytics *faceAnalytics = NULL;

void analyticsFunction(FileStreamer *videoStreamer, aiSaac::AiSaacSettings *aiSaacSettings, aiSaac::CustomAnalytics *caAge, aiSaac::CustomAnalytics *caGender) {
    std::string outputFilePath = "../data/AgeData1/ageAnnotatedFile.avi";
    cv::VideoWriter outputFile;
    outputFile.open(outputFilePath.c_str(), CV_FOURCC('M', 'J', 'P', 'G') , videoStreamer->getFileFPS(), videoStreamer->getFileFrameSize(), true);
    aiSaac::BgSubAnalytics *bgSubAnalytics = new aiSaac::BgSubAnalytics(aiSaacSettings);
    aiSaac::FaceAnalytics *faceAnalytics = new aiSaac::FaceAnalytics(aiSaacSettings);
    aiSaac::AgeRecognition * ageRecognition = new aiSaac::AgeRecognition(aiSaacSettings);
    cv::Mat currentFrame;
    int frameNumber = 0;
    cv::namedWindow("Display window", CV_WINDOW_AUTOSIZE);
    int fontFace = cv::FONT_HERSHEY_SIMPLEX;
    double fontScale = 0.5;
    int thickness = 2;
    while (true) {
        if(!videoStreamer->isStreaming()) {
            std::cout << "Seems to be done" <<std::endl;
            break;
        }
        currentFrame = videoStreamer->getFrame();
        if(currentFrame.data == NULL) {
            std::cout << "Seems to be done" <<std::endl;
            break;
        }
        std::vector<cv::Rect> localizedRectangles = std::vector<cv::Rect>();
        clock_t time;
        time = clock();
        bgSubAnalytics->localize(currentFrame, frameNumber, localizedRectangles);
        std::cout << "BG SUB Prediction Time: " << sec(clock() - time) << std::endl;
        time = clock();
        std::vector<cv::Rect> allFaces = std::vector<cv::Rect>();
        std::vector<std::string> labels =std::vector<std::string>();
        for (int i = 0; i < localizedRectangles.size(); i++) {
            //faceAnalytics->track(currentFrame, frameNumber, localizedRectangles[i]);
            std::vector<cv::Rect> localizedFaces = std::vector<cv::Rect>();
            faceAnalytics->localize(currentFrame, localizedFaces, frameNumber, localizedRectangles[i]);
            for (int j = 0; j < localizedFaces.size(); j++) {
               int topLeftX = localizedFaces[j].x - (localizedFaces[j].width / 4);
               (topLeftX >= j) ? topLeftX = topLeftX : topLeftX = 0;
               int topLeftY = localizedFaces[j].y - (localizedFaces[j].height / 2);
               (topLeftY >= 0) ? topLeftY = topLeftY : topLeftY = 0;
               cv::Point topLeft(topLeftX, topLeftY);

               int bottomRightX = localizedFaces[j].x + localizedFaces[j].width + (localizedFaces[j].width / 4);
               (bottomRightX <= currentFrame.cols) ? bottomRightX = bottomRightX : bottomRightX = currentFrame.cols;
               int bottomRightY = localizedFaces[j].y + localizedFaces[j].height + (localizedFaces[j].height / 4);
               (bottomRightY <= currentFrame.rows) ? bottomRightY = bottomRightY : bottomRightY = currentFrame.rows;
               cv::Point bottomRight(bottomRightX, bottomRightY);
               cv::Rect largerLocalization(topLeft, bottomRight);
               // std::string labelAge = caAge->detect(currentFrame(largerLocalization));
               int age = ageRecognition->detectExpected(currentFrame(largerLocalization));
               std::stringstream ss;
               ss << age;
               std::string labelAge = ss.str();
               std::string labelGender = caGender->detect(currentFrame(largerLocalization));
               std::string label = labelAge + ", " + labelGender;
               allFaces.push_back(localizedFaces[j]);
               labels.push_back(label);
            }
            std::cout << "FACE Prediction Time: " << sec(clock() - time) << std::endl;
        }
        frameNumber++;
        cv::Mat annotatFrame;
        for (int i = 4; i>0;i--) {
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
                    cv::rectangle(annotatFrame, allFaces[c], cv::Scalar(0,0,255), 2);
                    std::string label = labels[c];
                    int baseline=0;
                    cv::Size textSize = cv::getTextSize(label, fontFace,
                            fontScale, thickness, &baseline);
                    baseline += thickness;
                    cv::Point textOrg(allFaces[c].x + (allFaces[c].width/2) - (textSize.width/2) , allFaces[c].y - 2);
                    if (textOrg.x < 0)
                        textOrg.x = 0;
                    if (textOrg.y < 0)
                        textOrg.y = 0;
                    cv::putText(annotatFrame, label, textOrg, fontFace, fontScale,
            cv::Scalar(255,0,0), thickness);
                }
                cv::imshow("Display window", annotatFrame);
                outputFile.write(annotatFrame);
                cv::waitKey(2);
            }
        }
    }
    outputFile.release();
}



int main(int argc, char *argv[]) {
    aiSaac::CASettings *caSettingsAge = new aiSaac::CASettings(
        storagePath, "AgeGenderRecognition");
    caSettingsAge->setTrainingDataPath("../data/AgeData1/Train");
    caSettingsAge->setLabelFilePath("../data/AgeData1/labels.txt");
    caSettingsAge->setTrainingJsonDataPath("AgeJSON/trainingData_VGG_FULL.json");
    caSettingsAge->setIsTrained(true);
    caSettingsAge->setDATKSaveModelPath(storagePath + "../data/AgeData1/AgeRecModel.datk");
    aiSaac::CustomAnalytics *caAge = new aiSaac::CustomAnalytics(caSettingsAge);
    // caAge->train();

    aiSaac::CASettings *caSettingsGender = new aiSaac::CASettings(
        storagePath, "GenderRecognition");
    caSettingsGender->setTrainingDataPath(storagePath + "../data/GenderData1/Train");
    caSettingsGender->setLabelFilePath(storagePath + "../data/GenderData1/labels.txt");
    caSettingsGender->setTrainingJsonDataPath("GenderJSon/trainingData_VGG.json");
    caSettingsGender->setIsTrained(true);
    caSettingsGender->setDATKSaveModelPath(storagePath + "../data/GenderData1/GenderRecModel.datk");
    aiSaac::CustomAnalytics *caGender = new aiSaac::CustomAnalytics(caSettingsGender);
    caGender->train();
    debugMsg("Camera Initialized\n");
    char* filePath;
    if (argc > 1) {
        filePath = argv[1];
        if (argc >2) {
            storagePath = argv [2];
        }
    }
    FileStreamer *cam = new FileStreamer(filePath);
     if (!cam->isStreaming()) {
        std::cout << "Unable to open file for reading" << std::endl;
        debugMsg("Unable to open file for reading");
        return 0;
     }
     std::cout << "File Frame rate" << cam->getFileFPS() << std::endl;
    while (!cam->isStreaming()) {}
    std::cout << "camera running now " << cam->getFrame().size() << std::endl;
    aiSaac::AiSaacSettings *aiSaacSettings =
        new aiSaac::AiSaacSettings(storagePath, "config1");
    analyticsFunction(cam, aiSaacSettings, caAge, caGender);
    return 0;
}
