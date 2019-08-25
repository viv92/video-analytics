/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/*
    Copyright 2016 AITOE
*/

#include <cv.h>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <thread>
#include <mutex>
#include "../../src/aiSaac.h"
#include "../../tools/logging.h"

#include <dirent.h>
#include <fstream>
#include <iostream>
#include <string>
int drag;
cv::Point point1, point2; /* vertical points of the bounding box */
cv::Rect markedRect; /* bounding box */
int select_flag = 0;
cv::Mat img;
// void alertEvent(struct aisl::event eventData) {
//     std::cout << "EVENT CALLBACK" << std::endl;
//     //debugMsg("Event Callback\n");
//     std::cout << eventData.eventType << std::endl;
//     std::cout << eventData.time << std::endl;
//     std::cout << eventData.imageLocation << std::endl;
//     std::cout << eventData.videoLocation << std::endl;
//     std::cout << eventData.description << std::endl;
// }
aiSaac::FaceAnalytics *faceAnalytics = NULL;



int main(int argc, char *argv[]) {
    if(argc<3) {
        std::cout<< "Usage:" <<std::endl <<"./aiSaac <path/to/query/images> <path/to/outputcsv>";
        return 0;
    }
    std::string folderPath = argv[1];
    std::string outputPath = argv[2];
    std::cout << folderPath << std::endl;
    std::cout << outputPath << std::endl;

    int totalTestedFaces = 0;
    int totalCorrectFaces = 0;
    int totalIncorrectFaces = 0;

    ofstream dataCSVFile;
    dataCSVFile.open(outputPath + "/GenderData"+".csv");
    dataCSVFile << "Source,Predicted, Actual, Correct(0/1)\n";

    aiSaac::AiSaacSettings *aiSaacSettings =
        new aiSaac::AiSaacSettings(storagePath, "config1");

    faceAnalytics = new aiSaac::FaceAnalytics(aiSaacSettings);
    faceAnalytics->genderRecognition->initialize();
    DIR *faceStorageDir;
    struct dirent *direntFaceStorageDir;
    DIR *genderDir;
    struct dirent *direntGenderDir;
    std::string genderFolder;
    std::string genderName;

    std::string faceImagePath;

    faceStorageDir = opendir(folderPath.c_str());
    while((direntFaceStorageDir = readdir(faceStorageDir)) != NULL) {
        if (direntFaceStorageDir->d_type == DT_DIR
            && strcmp(direntFaceStorageDir->d_name, ".")
            && strcmp(direntFaceStorageDir->d_name, "..")) {

            genderName = direntFaceStorageDir->d_name;
            genderFolder = folderPath + "/" + genderName;
            std::cout<< "gender Folder" << genderFolder << std::endl;
            genderDir = opendir(genderFolder.c_str());
            while((direntGenderDir = readdir(genderDir)) != NULL) {
                if (direntGenderDir->d_type == DT_REG) {
                        std::string fileName(direntGenderDir->d_name);
                        faceImagePath = genderFolder + "/" + fileName;
                        std::cout << "final face image path: " << faceImagePath << std::endl;
                        cv::Mat faceImage = cv::imread(faceImagePath);
                        totalTestedFaces++;
                        std::string prediction = faceAnalytics->genderRecognition->runAlgo(faceImage);
                        std::cout << "image:" << faceImagePath <<"\tprediction" <<
                                prediction << "\tactual" << genderName;
                        dataCSVFile << faceImagePath<<","<<prediction<<","<<genderName<<",";
                        if(prediction == genderName) {
                            dataCSVFile << "1\n";
                            totalCorrectFaces++;
                        }
                        else {
                            dataCSVFile << "0\n";
                            totalIncorrectFaces++;
                        }
                    }
                }
            }
    }
    dataCSVFile << "\n";
    dataCSVFile << "Total = " <<totalTestedFaces << ", Correct = "
            << totalCorrectFaces <<",Incorrect = "<<totalIncorrectFaces
            << ", Accuracy = " << ((float)totalCorrectFaces)/((float)totalTestedFaces) << "\n";
    dataCSVFile.close();
}
