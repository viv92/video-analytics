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
    if (argc < 4) {
        std::cout << "Usage: objectImageExampleDarknet imageToLoad imageToSave [aiSaacSettings]" << endl;
        return -1;
    }
    aiSaac::AiSaacSettings *aiSaacSettings = new aiSaac::AiSaacSettings(argv[3]);
    aiSaacSettings->setObjectAnalyticsAlgo("DARKNET");
    aiSaac::ObjectAnalytics *objectAnalytics = new aiSaac::ObjectAnalytics(*aiSaacSettings);

    cv::Mat frame;
    frame = cv::imread(argv[1], CV_LOAD_IMAGE_COLOR);  // Read the file
    if (!frame.data) {
        std::cout << "Could not open or find the image" << std::endl;
        return -1;
    }
    cv::namedWindow("Display window", CV_WINDOW_AUTOSIZE);
    cv::Mat annotatedFrame = frame;
    std::vector<std::pair<std::string, cv::Rect>> localizedLabeledRectangles;
    objectAnalytics->localize(frame, localizedLabeledRectangles);
    std::cout << "localizedLabeledRectangles.size(): " << localizedLabeledRectangles.size() << std::endl;
    objectAnalytics->annotate(annotatedFrame, localizedLabeledRectangles);
    if (annotatedFrame.data != NULL) {
        cv::imshow("Display window", annotatedFrame);
        cv::waitKey(0);
    }
    if (argc == 4) {
        cv::imwrite(argv[2], annotatedFrame);
    }
}
