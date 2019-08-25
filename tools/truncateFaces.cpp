/*
    Copyright 2016 AITOE
*/

#include <cv.h>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <thread>
#include <mutex>
#include <dirent.h>
#include <fstream>
#include "developerConfig.h"
#include "../src/aiSaac.h"

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cout << "Usage: ./truncateFaces [aiSaacSettingsPath]"
            << " [dataPath] [storeTruncatedFacesPath]" << std::endl;
        return -1;
    }
    setDeveloperVariables(developerName);
    aiSaac::AiSaacSettings aiSaacSettings = aiSaac::AiSaacSettings(argv[1]);
    aiSaac::FaceAnalytics *faceAnalytics = new aiSaac::FaceAnalytics(aiSaacSettings);

    DIR *faceStorageDir;
    struct dirent *direntFaceStorageDir;
    DIR *personPath;
    struct dirent *direntPersonPath;
    faceStorageDir = opendir(argv[2]);
    std::string storeTruncatedFacesPath = argv[3];
    std::string personFolder;
    std::string personName;
    std::string faceImagePath;


    while((direntFaceStorageDir = readdir(faceStorageDir)) != NULL) {
        // std::cout << direntFaceStorageDir->d_name;
        personFolder = argv[2];
        if (direntFaceStorageDir->d_type == DT_DIR
            && std::strcmp(direntFaceStorageDir->d_name, ".")
            && std::strcmp(direntFaceStorageDir->d_name, "..")) {
            personName = direntFaceStorageDir->d_name;
            personFolder = personFolder + "/" + direntFaceStorageDir->d_name;
            personPath = opendir(personFolder.c_str());
            // stringToInt.push_back(std::make_pair(currentIntLabel, personName));
            std::string personDirectory = storeTruncatedFacesPath + "/" + personName;
            std::system(("mkdir " + personDirectory).c_str());
            int count = 0;
            while ((direntPersonPath = readdir(personPath))) {
                if (direntPersonPath->d_type == DT_REG) {
                    std::string fileName(direntPersonPath->d_name);
                    if (fileName.substr(fileName.find_last_of(".") + 1) != "DS_Store") {
                        faceImagePath = personFolder + "/" + fileName;
                        std::cout << "final face image path: " << faceImagePath << std::endl;
                        std::vector<cv::Rect> localizedRectangles;
                        cv::Mat rawImage = cv::imread(faceImagePath);
                        faceAnalytics->localize(rawImage, localizedRectangles);
                        std::cout << "Finished Localizing" << std::endl
                            << std::flush;
                        for(int i = 0; i < localizedRectangles.size(); i++) {
                            int topLeftX = localizedRectangles[i].x
                                - (localizedRectangles[i].width / 4);
                            (topLeftX >= 0) ? topLeftX = topLeftX : topLeftX = 0;
                            int topLeftY = localizedRectangles[i].y
                                - (localizedRectangles[i].height / 4);
                            (topLeftY >= 0) ? topLeftY = topLeftY : topLeftY = 0;
                            cv::Point topLeft(topLeftX, topLeftY);

                            int bottomRightX = localizedRectangles[i].x
                                + localizedRectangles[i].width
                                + (localizedRectangles[i].width / 4);
                            (bottomRightX <= rawImage.cols) ? bottomRightX = bottomRightX :
                                bottomRightX = rawImage.cols;
                            int bottomRightY = localizedRectangles[i].y
                                + localizedRectangles[i].height +
                                (localizedRectangles[i].height / 4);
                            (bottomRightY <= rawImage.rows) ? bottomRightY = bottomRightY :
                                bottomRightY = rawImage.rows;
                            cv::Point bottomRight(bottomRightX, bottomRightY);
                            cv::Rect largerLocalization(topLeft, bottomRight);
                            std::string personImagePath = personDirectory
                                + "/"
                                + std::to_string(count++)
                                + ".png";
                            std::cout << "storing image: " << personImagePath << std::endl;
                            cv::imwrite(personImagePath, rawImage(largerLocalization));                            
                        }

                    } else {
                        std::cout << "Ignoring " << fileName << std::endl;
                    }
                }
            }
        }
    }
}
