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
    aiSaac::AiSaacSettings *aiSaacSettings = new aiSaac::AiSaacSettings(storagePath, "config");
    aiSaac::TextAnalytics *textAnalytics = new aiSaac::TextAnalytics(aiSaacSettings);

    if (argc < 2) {
        std::cout << "Usage: ./textImageExample [imageToLoad] [saveImage]" << endl;
        return -1;
    }
    cv::Mat frame;
    frame = cv::imread(argv[1], CV_LOAD_IMAGE_COLOR);   // Read the file
    if (!frame.data) {
        std::cout << "Could not open or find the image" << std::endl;
        return -1;
    }
    cv::namedWindow("Display window", CV_WINDOW_AUTOSIZE);
    std::vector<cv::Rect> localizedRectangles;
    textAnalytics->localize(frame, localizedRectangles);
    std::vector<std::pair<std::string, cv::Rect>> localizedLabeledRectangles;
    for (int i = 0; i < localizedRectangles.size(); i++) {
        std::pair<std::string, cv::Rect> localizedLabeledRectangle;
        localizedLabeledRectangle.first = textAnalytics->recognize(frame(localizedRectangles[i]));
        localizedLabeledRectangle.second = localizedRectangles[i];
        localizedLabeledRectangles.push_back(localizedLabeledRectangle);
    }

    cv::Mat originalFrame = frame;
    textAnalytics->annotate(originalFrame, localizedRectangles);
    cv::Mat annotatedFrame(originalFrame.rows, originalFrame.cols, CV_8UC3, cv::Scalar(0, 0, 0));
    textAnalytics->annotate(annotatedFrame, localizedLabeledRectangles);
    if (originalFrame.data != NULL && annotatedFrame.data != NULL) {
        cv::imshow("Display original", originalFrame);
        cv::imshow("Display Annotation", annotatedFrame);
        cv::waitKey(0);
    }
    if (argc == 3) {
        cv::imwrite(argv[2], annotatedFrame);
    }
}
