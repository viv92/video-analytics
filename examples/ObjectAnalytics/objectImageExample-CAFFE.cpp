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
    // ::google::InitGoogleLogging(argv[0]);
    if (argc < 3) {
        std::cout << "Usage: objectImageExampleCaffe [imageToLoad] [aiSaacSettings]" << endl;
        return -1;
    }
    aiSaac::AiSaacSettings aiSaacSettings = aiSaac::AiSaacSettings(argv[2]);
    aiSaac::ObjectAnalytics objectAnalytics = aiSaac::ObjectAnalytics(aiSaacSettings);

    cv::Mat frame;
    frame = cv::imread(argv[1], CV_LOAD_IMAGE_COLOR);  // Read the file
    if (!frame.data) {
        std::cout << "Could not open or find the image" << std::endl;
        return -1;
    }
    cv::namedWindow("Display window", CV_WINDOW_AUTOSIZE);
    cv::Mat annotatedFrame = frame;
    std::vector<std::string> labels = objectAnalytics.detect(frame);
    objectAnalytics.annotate(annotatedFrame, labels);
    if (annotatedFrame.data != NULL) {
        cv::imshow("Display window", annotatedFrame);
        cv::waitKey(0);
    }
    return 0;
}
