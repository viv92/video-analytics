/*
    Copyright 2017 AITOE
*/

#include <cv.h>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <vector>
#include <string>
#include "../../tools/videoSources/fileStreamer.h"
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

    if ( argc < 3 ) {
        std::cout << "Usage: ./headAnalyticsQueue [sourcefile] [outputFilePath] [aiSaacSettingsPath]" << std::endl;
        return -1;
    }

    std::string file = argv[1];
    std::string outputFilePath = argv[2];
    std::string aiSaacSettingsPath = argv[3];

    aiSaac::AiSaacSettings *aiSaacSettings = new aiSaac::AiSaacSettings(aiSaacSettingsPath);
    aiSaac::HeadAnalytics headAnalytics = aiSaac::HeadAnalytics(aiSaacSettings);

    if ( file.find("mp4") < file.length() ) {

        FileStreamer *fileStreamer = new FileStreamer(file);
        if(!fileStreamer->isStreaming()) {
            debugMsg("Unable to open file for reading");
            return -1;
        } else {
            std::cout << "Found file for reading" << std::endl;
        }

        cv::Mat currentFrame, annotatedFrame;
        cv::VideoWriter annotatedVideo;
        cv::Point textOrg;
        int frameNumber = 0;
        int totalProcessedFrameNumber = 0;
        int FPS = fileStreamer->getFileFPS();
        int procFPS = FPS;
        cv::Size S;

        int fontFace = cv::FONT_HERSHEY_SIMPLEX;
        double fontScale = 0.7;
        int thickness = 2;

        std::vector<std::vector<int>> headCount;
        int slidingWindow = 100; // Used for limiting the frames used for average calculation.
        std::vector<int> headCountSum;

        S = fileStreamer->getFrameSize();
        annotatedVideo.open(outputFilePath, CV_FOURCC('M','P','4','2'), FPS, S, true);
        if ( procFPS < 1 ) {
            procFPS = 1;
        } else if ( procFPS > FPS ) {
            procFPS = FPS;
        }

        while (true) {
            currentFrame = fileStreamer->getFrame();
            if( currentFrame.data == NULL || currentFrame.empty() || !fileStreamer->isStreaming()) {
                break;
            }
            totalProcessedFrameNumber++;
        }

        fileStreamer = new FileStreamer(file);

        cv::namedWindow("Display window", CV_WINDOW_AUTOSIZE);
        currentFrame = fileStreamer->getFrame();
        getSelection(currentFrame);
        std::vector<std::vector<cv::Rect>> localizedRectangles(markedRectVector.size());
        std::cout << "Number of marked rectangles: " <<markedRectVector.size() << std::endl;

        for (int i = 0; i < markedRectVector.size(); i++) {
            headCountSum.push_back(0);
            // To initialise headCount, headCountSum has been pushed back into it. This has been done to avoid the usage of a temp variable just for the sake of initialisaiton. MemorySavingFTW.
            headCount.push_back(headCountSum);
        }

        while (true) {
            currentFrame = fileStreamer->getFrame();
            if( currentFrame.data == NULL || currentFrame.empty() || !fileStreamer->isStreaming()) {
                break;
            }

            for (int i = 0; i < markedRectVector.size(); i++) {
                cv::rectangle(currentFrame, markedRectVector[i], CV_RGB(255, 0, 0), 3, 8, 0);
            }

            if (frameNumber % FPS / procFPS == 0) {
                for (int i = 0; i < markedRectVector.size(); i++) {
                    headAnalytics.localize(currentFrame, frameNumber, localizedRectangles[i], markedRectVector[i]);
                }
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

            if (frameNumber == floor(totalProcessedFrameNumber*0.25)) {
                std::cout << "AISAAC_LOG: Your video is 25% processed." << std::endl;
            } else if (frameNumber == floor(totalProcessedFrameNumber*0.50)) {
                std::cout << "AISAAC_LOG: Your video is 50% processed." << std::endl;
            } else if (frameNumber == floor(totalProcessedFrameNumber*0.75)) {
                std::cout << "AISAAC_LOG: Your video is 75% processed." << std::endl;
            }

            annotatedFrame = currentFrame;
            if (annotatedFrame.data != NULL) {
                annotatedVideo.write(annotatedFrame);
            }
            frameNumber++;
        }
        annotatedVideo.release();
        std::cout << "AISAAC_LOG: Your video is 100% processed." << std::endl;
    }
    return 0;
}
