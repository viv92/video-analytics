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

int main(int argc, char *argv[]) {
    // codeReview(Anurag): argument parser needed
    if (argc != 4) {
        std::cout << "usage: ./faceRecognitionExampleTrainingTesting "
            << "[aiSaacSettingsPath] [caSettingsFilePath] [isCroppedFaces? (true/false)]"
            << std::endl;
        return -1;
    }

    signal(SIGINT, handler);

    std::string aiSaacSettingsPath = argv[1];
    std::string caSettingsPath = argv[2];
    std::string param3 = argv[3];
    bool isCroppedData = true;
    if (param3.compare("false") == 0)
        isCroppedData = false;


    std::string aiSaacSettingsFolderPath;
    std::string aiSaacSettingsFileName;

    aiSaac::CASettings *caSettings = new aiSaac::CASettings(caSettingsPath);

    for (int i = aiSaacSettingsPath.length() - 1; i >= 0; i--) {
        if (aiSaacSettingsPath[i] == '/') {
            aiSaacSettingsFolderPath = aiSaacSettingsPath.substr(0, i + 1);
            aiSaacSettingsFileName = aiSaacSettingsPath.substr(i + 1,
                aiSaacSettingsPath.length() - i);
            break;
        }
    }

    aiSaac::AiSaacSettings aiSaacSettings = aiSaac::AiSaacSettings(aiSaacSettingsFolderPath,
        aiSaacSettingsFileName);

    if (!isCroppedData) {
        aiSaac::FaceAnalytics faceAnalytics = aiSaac::FaceAnalytics(aiSaacSettings);

        //TODO: clean data
        int status = std::rename(caSettings->getTrainingDataPath().c_str(),
                (caSettings->getTrainingDataPath() + "_original").c_str() );
        if (status == -1) {
            std::cout << "Unable to crop faces" << std::endl;
            return 0;
        }


        DIR *trainingFaceStorageDir;
        struct dirent *direntTrainingFaceStorageDir;
        DIR *trainingPersonPath;
        struct dirent *direntTrainingPersonPath;
        trainingFaceStorageDir = opendir(
                (caSettings->getTrainingDataPath() + "_original").c_str());
        std::string trainingPersonFolder;
        std::string trainingPersonName;
        std::string trainingFaceImagePath;

        std::system(("mkdir \"" + caSettings->getTrainingDataPath() + "\"").c_str());

        while((direntTrainingFaceStorageDir = readdir(trainingFaceStorageDir)) != NULL) {
            trainingPersonFolder = (caSettings->getTrainingDataPath() + "_original").c_str();
            if (direntTrainingFaceStorageDir->d_type == DT_DIR
                && std::strcmp(direntTrainingFaceStorageDir->d_name, ".")
                && std::strcmp(direntTrainingFaceStorageDir->d_name, "..")) {
                trainingPersonName = direntTrainingFaceStorageDir->d_name;
                trainingPersonFolder = trainingPersonFolder + "/" + direntTrainingFaceStorageDir->d_name;
                std::system(("mkdir \"" + caSettings->getTrainingDataPath()
                        + "/" + trainingPersonName + "\"").c_str());
                trainingPersonPath = opendir(trainingPersonFolder.c_str());
                while ((direntTrainingPersonPath = readdir(trainingPersonPath))) {
                    if (direntTrainingPersonPath->d_type == DT_REG) {
                        std::string fileName(direntTrainingPersonPath->d_name);
                        if (fileName.substr(fileName.find_last_of(".") + 1) != "DS_Store") {
                            trainingFaceImagePath = trainingPersonFolder + "/" + fileName;
                            cv::Mat srcImage = cv::imread(trainingFaceImagePath);
                            std::vector<cv::Rect> localizedRectangles;
                            faceAnalytics.localize(srcImage, localizedRectangles);
                            if (localizedRectangles.size() == 1) {
                                // Write cropped image
                                std::string destPath = caSettings->getTrainingDataPath()
                                        + "/" + trainingPersonName
                                        + "/" + fileName;
                                cv::imwrite(destPath, srcImage(localizedRectangles[0]));
                            }

                        } else {
                            std::cout << "Ignoring " << fileName << std::endl;
                        }
                    }
                }
            }
        }


        status = std::rename(caSettings->getTestingDataPath().c_str(),
                (caSettings->getTestingDataPath() + "_original").c_str() );
        if (status == -1) {
            std::cout << "Unable to crop faces" << std::endl;
            return 0;
        }

        DIR *testingFaceStorageDir;
        struct dirent *direntTestingFaceStorageDir;
        DIR *testingPersonPath;
        struct dirent *direntTestingPersonPath;
        testingFaceStorageDir = opendir(
                (caSettings->getTestingDataPath() + "_original").c_str());
        std::string testingPersonFolder;
        std::string testingPersonName;
        std::string testingFaceImagePath;

        std::system(("mkdir \"" + caSettings->getTestingDataPath() + "\"").c_str());

        while((direntTestingFaceStorageDir = readdir(testingFaceStorageDir)) != NULL) {
            // std::cout << direntTestingFaceStorageDir->d_name;
            testingPersonFolder = (caSettings->getTestingDataPath() + "_original").c_str();
            if (direntTestingFaceStorageDir->d_type == DT_DIR
                && std::strcmp(direntTestingFaceStorageDir->d_name, ".")
                && std::strcmp(direntTestingFaceStorageDir->d_name, "..")) {
                testingPersonName = direntTestingFaceStorageDir->d_name;
                testingPersonFolder = testingPersonFolder + "/" + direntTestingFaceStorageDir->d_name;
                std::system(("mkdir \"" + caSettings->getTestingDataPath()
                        + "/" + testingPersonName + "\"").c_str());
                testingPersonPath = opendir(testingPersonFolder.c_str());
                while ((direntTestingPersonPath = readdir(testingPersonPath))) {
                    if (direntTestingPersonPath->d_type == DT_REG) {
                        std::string fileName(direntTestingPersonPath->d_name);
                        if (fileName.substr(fileName.find_last_of(".") + 1) != "DS_Store") {
                            testingFaceImagePath = testingPersonFolder + "/" + fileName;
                            std::cout << "final face image path: " << testingFaceImagePath << std::endl;
                            cv::Mat srcImage = cv::imread(testingFaceImagePath);
                            std::vector<cv::Rect> localizedRectangles;
                            faceAnalytics.localize(srcImage, localizedRectangles);
                            if (localizedRectangles.size() == 1) {
                                // Write cropped image
                                std::string destPath = caSettings->getTestingDataPath()
                                        + "/" + testingPersonName
                                        + "/" + fileName;
                                cv::imwrite(destPath, srcImage(localizedRectangles[0]));
                            }

                        } else {
                            std::cout << "Ignoring " << fileName << std::endl;
                        }
                    }
                }
            }
        }
    }

    caSettings->setIsTrained(false);
    caSettings->setIsTested(false);
    aiSaac::CustomAnalytics *ca1 = new aiSaac::CustomAnalytics(caSettings);
    std::cout << "Training model " << std::endl;
    ca1->train();
    std::cout << "Testing model " << std::endl;
    ca1->test();
}
