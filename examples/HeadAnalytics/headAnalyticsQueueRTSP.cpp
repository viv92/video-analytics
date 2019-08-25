/*
    Copyright 2017 AITOE
*/

#include <cv.h>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <thread>
#include "../../tools/videoSources/libvlcStreamer.h"
#include "../../tools/videoSources/aiVideoStreamer.h"
#include "../../tools/logging.h"
#include "../../src/aiSaac.h"

int drag;
cv::Point point1, point2; /* vertical points of the bounding box */
cv::Rect markedRect;
std::vector<cv::Rect> markedRectVector; /* Vector of bounding boxes */
int select_flag = 0;
cv::Mat img;

void mouseHandler(int event, int x, int y, int flags, void* param) {

    cv::Mat img1 = img.clone();
    if (event == CV_EVENT_LBUTTONDOWN && !drag) {
        point1 = cv::Point(x, y);
        drag = 1;
    }
    if (event == CV_EVENT_MOUSEMOVE && drag) {
        point2 = cv::Point(x, y);
        cv::rectangle(img1, point1, point2, CV_RGB(255, 0, 0), 3, 8, 0);
        cv::imshow("Mark Area", img1);
        //debugMsg("Mark Area\n");
    }
    if (event == CV_EVENT_LBUTTONUP && drag) {
        point2 = cv::Point(x, y);
        markedRect = cv::Rect(point1.x, point1.y, x-point1.x, y-point1.y);
        markedRectVector.push_back(markedRect);
        if (markedRect.width > 0 && markedRect.height > 0)
          drag = 0;
    }
    if (event == CV_EVENT_LBUTTONUP) {
       /* ROI selected */
        select_flag = 1;
        drag = 0;
        std::cout << "MARKED RECTANGLE" << std::endl << std::flush;
        debugMsg("Marked Rectangle");
        // std::cout << "MARKED RECTANGLE2" << std::endl << std::flush;
        // codeReview(Pratik): Currently the bounding boxes disappear when a new box is being drawn. This shouldn't happen.
        for (int i = 0; i < markedRectVector.size(); i++) {
            cv::rectangle(img1, markedRectVector[i],CV_RGB(255, 0, 0), 3, 8, 0);
            cv::imshow("Mark Area", img1);
        }
    }
}

void getSelection(cv::Mat &rawFrame) {
    drag = 0;
    select_flag = 0;
    img = rawFrame.clone();
    cv::namedWindow("Mark Area", CV_WINDOW_AUTOSIZE);
    cv::setMouseCallback("Mark Area", mouseHandler, NULL);
    cv::imshow("Mark Area", img);
    cv::waitKey(10);
    while (!select_flag) {
        cv::waitKey(0);
    }  // todo blocks on main thread
    cv::destroyWindow("Mark Area");
}

int main(int argc, char *argv[]) {

    if ( argc < 4 ) {
        std::cout << "Usage: ./headAnalyticsQueueRTSP [source] [destination] [aiSaacSettingsPath]" << std::endl;
        return -1;
    }

    std::string source = argv[1];
    std::string outputFilePath = argv[2];
    std::string aiSaacSettingsPath = argv[3];

    source = "rtsp://admin:aitoe2016@192.168.0.210:554";

    aiSaac::AiSaacSettings *aiSaacSettings = new aiSaac::AiSaacSettings(aiSaacSettingsPath);
    aiSaac::HeadAnalytics headAnalytics = aiSaac::HeadAnalytics(aiSaacSettings);

    cv::Mat currentFrame, annotatedFrame;
    cv::VideoWriter annotatedVideo;
    cv::Point textOrg;
    int frameNumber = 0;
    int FPS = 25;
    cv::Size S;

    int fontFace = cv::FONT_HERSHEY_SIMPLEX;
    double fontScale = 0.7;
    int thickness = 2;

    std::vector<std::vector<int>> headCount;
    int slidingWindow = 100; // Used for limiting the frames used for average calculation.
    std::vector<int> headCountSum;

    LibvlcStreamer *rtspStreamer = new LibvlcStreamer(source);
    std::thread streamer(&LibvlcStreamer::startStream, rtspStreamer);
    // rtspStreamer->LibvlcStreamer::startStream();

    while (!rtspStreamer->isStreaming()) {}

    std::cout << "Streaming now" << std::endl;
    S = rtspStreamer->getFrame().size();
    annotatedVideo.open(outputFilePath, CV_FOURCC('M','J','P','G'), FPS, S, true);
    std::cout << "Opened AnnotatedVideo" << std::endl;

    currentFrame = rtspStreamer->getFrame();
    std::cout << "Frame Format: " << currentFrame.type() << std::endl;
    getSelection(currentFrame);
    std::vector<std::vector<cv::Rect>> localizedRectangles(markedRectVector.size());
    std::cout << "Number of marked rectangles: " << markedRectVector.size() << std::endl;

    for (int i = 0; i < markedRectVector.size(); i++) {
        headCountSum.push_back(0);
        // To initialise headCount, headCountSum has been pushed back into it. This has been done to avoid the usage of a temp variable just for the sake of initialisaiton. MemorySavingFTW.
        headCount.push_back(headCountSum);
    }

    cv::namedWindow("Display window", CV_WINDOW_AUTOSIZE);

    while (true) {
        currentFrame = rtspStreamer->getFrame();

        if( currentFrame.data == NULL || currentFrame.empty() || !rtspStreamer->isStreaming()) {
            break;
        }

        for (int i = 0; i < markedRectVector.size(); i++) {
            cv::rectangle(currentFrame, markedRectVector[i], CV_RGB(255, 0, 0), 3, 8, 0);
        }

        for (int i = 0; i < markedRectVector.size(); i++) {
            headAnalytics.localize(currentFrame, frameNumber, localizedRectangles[i], markedRectVector[i]);
        }

        for (int i = 0; i < localizedRectangles.size(); i++) {
            for (int j = 0; j < localizedRectangles[i].size(); j++) {
                if (headCount[i].size() < slidingWindow) {
                    headCount[i].push_back(localizedRectangles[i].size());
                } else {
                    headCount[i][(frameNumber % slidingWindow)] = localizedRectangles[i].size();
                }
                cv::rectangle(currentFrame, localizedRectangles[i][j], CV_RGB(255, 255, 255), 3, 8, 0);
            }
        }

        for (int i = 0; i < markedRectVector.size(); i++) {
            textOrg.x = markedRectVector[i].x;
            textOrg.y = markedRectVector[i].y;
            cv::putText(currentFrame, "Current Count: " + std::to_string(localizedRectangles[i].size()), textOrg, fontFace, fontScale, cv::Scalar(127, 255, 0), thickness);
            textOrg.x = markedRectVector[i].x;
            textOrg.y = markedRectVector[i].y - 20;
            headCountSum[i] = 0;
            for (int j = 0; j < headCount[i].size(); j++ ) {
                headCountSum[i] += headCount[i][j];
            }
            cv::putText(currentFrame, "Average Count: " + std::to_string((headCountSum[i] / (float) headCount[i].size())), textOrg, fontFace, fontScale, cv::Scalar(127, 255, 0), thickness);
        }

        cv::imshow("Display window", currentFrame);
        cv::waitKey(10);

        annotatedFrame = currentFrame;
        if (annotatedFrame.data != NULL) {
            // annotatedVideo.write(annotatedFrame);
        }
        frameNumber++;
    }
    annotatedVideo.release();
    return 0;
}
