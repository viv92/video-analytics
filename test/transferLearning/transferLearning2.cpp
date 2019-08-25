#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <dirent.h>
#include <vector>
#include <fstream>
#include "../../src/aiSaac.h"
#include "datk.h"

int main() {
    aiSaac::CASettings *caSettings = new aiSaac::CASettings(
        storagePath, "FaceRecognition");
    caSettings->setTrainingDataPath(storagePath + "/data/FaceRecData1/Train");
    caSettings->setTestingDataPath(storagePath + "/data/FaceRecData1/Test");
    caSettings->setLabelFilePath(storagePath + "/data/FaceRecData1/labels.txt");
    caSettings->setTrainingJsonDataPath(storagePath + "/data/FaceRecognitionJSON/trainingData_VGG.json");
    caSettings->setTestingJsonDataPath(storagePath + "/data/FaceRecognitionJSON/testingData_VGG.json");
    caSettings->setIsTrained(false);
    caSettings->setIsTested(false);
    caSettings->setDATKSaveModelPath(storagePath + "/data/FaceRecData1/FaceRecModel.datk");

    aiSaac::CustomAnalytics ca1(caSettings);
    ca1.train();
    // ca1.loadDatkModel();
    ca1.test();
    std::string label = ca1.detect(cv::imread(
        "/Users/anurag/Documents/projects/git-stuff/temp-stash/aiSaac/build/storageDir/faceStorage/Test/10/face-4.png"));
}
