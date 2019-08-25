#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <dirent.h>
#include <vector>
#include <fstream>
#include "../../src/aiSaac.h"
#include "datk.h"
#include "../../tools/videoSources/fileStreamer.h"

int main(int argc, char *argv[]) {
    aiSaac::AiSaacSettings *aiSaacSettings = new aiSaac::AiSaacSettings(storagePath, "config1");
    aiSaac::CASettings *caSettingsAge = new aiSaac::CASettings(
        storagePath, "AgeGenderRecognition");
    caSettingsAge->setTrainingDataPath("../data/AgeData1/Train");
    caSettingsAge->setLabelFilePath("../data/AgeData1/labels.txt");
    caSettingsAge->setTrainingJsonDataPath("AgeJSON/trainingData_VGG_FULL.json");
    caSettingsAge->setIsTrained(true);
    caSettingsAge->setDATKSaveModelPath(storagePath + "../data/AgeData1/AgeRecModel.datk");
    aiSaac::CustomAnalytics *caAge = new aiSaac::CustomAnalytics(caSettingsAge);
    caAge->train();

    aiSaac::CASettings *caSettingsGender = new aiSaac::CASettings(
        storagePath, "GenderRecognition");
    caSettingsGender->setTrainingDataPath(storagePath + "../data/GenderData1/Train");
    caSettingsGender->setLabelFilePath(storagePath + "../data/GenderData1/labels.txt");
    caSettingsGender->setTrainingJsonDataPath("GenderJSon/trainingData_VGG.json");
    caSettingsGender->setIsTrained(true);
    caSettingsGender->setDATKSaveModelPath(storagePath + "../data/GenderData1/GenderRecModel.datk");
    aiSaac::CustomAnalytics *caGender = new aiSaac::CustomAnalytics(caSettingsGender);
    caGender->train();
    if (argc != 3) {
        std::cout << "Usage: ageGenderImageExampleNN imageToLoad imageToWrite" << endl;
        return -1;
    }
    cv::Mat frame;
    frame = cv::imread(argv[1], CV_LOAD_IMAGE_COLOR);   // Read the file
    if (!frame.data) {
        std::cout << "Could not open or find the image" << std::endl;
        return -1;
    }
    cv::namedWindow("Display window", CV_WINDOW_AUTOSIZE);
    cv::Mat annotatedFrame = frame;
    aiSaac::FaceAnalytics *faceAnalytics = new aiSaac::FaceAnalytics(aiSaacSettings);
    std::vector<cv::Rect> allFaces = std::vector<cv::Rect>();
    std::vector<std::string> labels =std::vector<std::string>();
    std::vector<cv::Rect> localizedFaces = std::vector<cv::Rect>();
    faceAnalytics->localize(frame, localizedFaces);
    int fontFace = cv::FONT_HERSHEY_SIMPLEX;
    double fontScale = 0.5;
    int thickness = 2;
    for (int j = 0; j < localizedFaces.size(); j++) {
        int topLeftX = localizedFaces[j].x - (localizedFaces[j].width / 4);
        (topLeftX >= j) ? topLeftX = topLeftX : topLeftX = 0;
        int topLeftY = localizedFaces[j].y - (localizedFaces[j].height / 2);
        (topLeftY >= 0) ? topLeftY = topLeftY : topLeftY = 0;
        cv::Point topLeft(topLeftX, topLeftY);
        int bottomRightX = localizedFaces[j].x + localizedFaces[j].width + (localizedFaces[j].width / 4);
        (bottomRightX <= frame.cols) ? bottomRightX = bottomRightX : bottomRightX = frame.cols;
        int bottomRightY = localizedFaces[j].y + localizedFaces[j].height + (localizedFaces[j].height / 4);
        (bottomRightY <= frame.rows) ? bottomRightY = bottomRightY :
        bottomRightY = frame.rows;
        cv::Point bottomRight(bottomRightX, bottomRightY);
        cv::Rect largerLocalization(topLeft, bottomRight);
        std::string labelAge = caAge->detect(frame(largerLocalization));
        std::string labelGender = caGender->detect(frame(largerLocalization));
        std::string label = labelAge + ", " + labelGender;
        cv::rectangle(annotatedFrame, largerLocalization, cv::Scalar(255,255,255), 2);
        int baseline=0;
        cv::Size textSize = cv::getTextSize(label, fontFace, fontScale, thickness, &baseline);
        baseline += thickness;
        cv::Point textOrg(largerLocalization.x + (largerLocalization.width/2) - (textSize.width/2) , largerLocalization.y - 2);
        if (textOrg.x < 0)
            textOrg.x = 0;
        if (textOrg.y < 0)
            textOrg.y = 0;
        cv::putText(annotatedFrame, label, textOrg, fontFace, fontScale, cv::Scalar(255,255,255), thickness);
    }
    if (annotatedFrame.data != NULL) {
        cv::imshow("Display window", annotatedFrame);
        // cv::waitKey(0);
        cv::imwrite(argv[2], annotatedFrame);
    }
    return 0;
}
