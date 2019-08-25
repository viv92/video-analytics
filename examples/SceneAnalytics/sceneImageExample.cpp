/*
    Copyright 2016 AITOE
*/

#include <cv.h>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <string>
#include "../../tools/logging.h"
#include "../../src/aiSaac.h"

int main(int argc, char **argv) {
    ::google::InitGoogleLogging(argv[0]);
    aiSaac::AiSaacSettings aiSaacSettings = aiSaac::AiSaacSettings(storagePath, "config");
    aiSaac::SceneAnalytics *sceneAnalytics = NULL;
    sceneAnalytics = new aiSaac::SceneAnalytics(aiSaacSettings);

    if (argc != 3) {
        std::cout << "Usage: sceneImageExample imageToLoad imageTosave" << endl;
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
    std::vector<std::string> labels = sceneAnalytics->detect(frame);
    sceneAnalytics->annotate(annotatedFrame, labels);
    if (annotatedFrame.data != NULL) {
        cv::imshow("Display window", annotatedFrame);
        cv::waitKey(0);
        cv::imwrite(argv[2], annotatedFrame);
    }
}
