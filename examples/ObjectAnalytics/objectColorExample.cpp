/*
    Copyright 2016 AITOE
*/

#include <cv.h>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include "../../tools/videoSources/fileStreamer.h"
#include "../../tools/videoSources/aiVideoStreamer.h"
#include "../../tools/logging.h"
#include "../../src/aiSaac.h"
#include "../../src/utils/colorClassifier.h"

void annotate(cv::Mat &rawFrame,
    aiSaac::ObjectAnalytics &objectAnalytics,
    aiSaac::AiSaacSettings &aiSaacSettings,
    int frameNumber) {
    int count = 0;
    for (int i = 0; i < objectAnalytics.blobContainer.size(); i++) {
        if (objectAnalytics.blobContainer[i].lastFrameNumber == frameNumber) {
            if (aiSaacSettings.getObjectAnalyticsAlgo() == "CAFFE") {
                /*cv::putText(rawFrame,
                    objectAnalytics.blobContainer[i].metaData,
                    cv::Point(20, 20 * (count + 1)),
                    cv::FONT_HERSHEY_PLAIN,
                    1.0,
                    CV_RGB(124, 252, 0),
                    2.0);*/
                count++;
            } else if (aiSaacSettings.getObjectAnalyticsAlgo() == "DARKNET") {
                cv::rectangle(rawFrame,
                    objectAnalytics.blobContainer[i].lastRectangle,
                    cv::Scalar(255, 255, 255),
                    2);
                int baseline = 0;
                int fontScale = ceil(rawFrame.cols * 0.001);
                int thickness = ceil(rawFrame.cols * 0.002);

                /*cv::Size textSize = cv::getTextSize(objectAnalytics.blobContainer[i].metaData,
                     cv::FONT_HERSHEY_SIMPLEX,
                    fontScale,
                    thickness,
                    &baseline);*/
                baseline += thickness;
                /*cv::Point textOrg(objectAnalytics.blobContainer[i].lastRectangle.x
                    + (objectAnalytics.blobContainer[i].lastRectangle.width / 2)
                    - (textSize.width / 2),
                    objectAnalytics.blobContainer[i].lastRectangle.y - 5);*/
                /*if (textOrg.x < 0) {
                    textOrg.x = 0;
                }
                if (textOrg.y < 0) {
                    textOrg.y = 0;
                }*/
                /*cv::putText(rawFrame,
                    objectAnalytics.blobContainer[i].metaData,
                    textOrg,
                    cv::FONT_HERSHEY_SIMPLEX,
                    fontScale,
                    cv::Scalar(127, 255, 0),
                    thickness);*/
            }
        }
    }
}

int main(int argc, char *argv[]) {
    // ::google::InitGoogleLogging(argv[0]);
    if (argc < 3) {
        std::cout << "Usage: objectColorExample [videotoLoad] [annotatedVideoPath] [aiSaacSettings] [procFPS]" << endl;
        return -1;
    }
    std::string file = argv[1];
    std::string outputFilePath = argv[2];
    aiSaac::AiSaacSettings aiSaacSettings = aiSaac::AiSaacSettings(argv[3]);
    int procFPS = atoi(argv[4]);
    aiSaac::ObjectAnalytics objectAnalytics = aiSaac::ObjectAnalytics(aiSaacSettings);
    aiSaac::ColorClassifier colorClassifier = aiSaac::ColorClassifier();
    cv::VideoWriter annotatedVideo;

    if ( file.find("mp4") < file.length() || file.find("avi") < file.length() ) {
        FileStreamer *cam = new FileStreamer(file);
        cv::Size S = cam->getFrameSize();
        int FPS = cam->getFileFPS();
        if (argc > 2) {
            annotatedVideo.open(outputFilePath,
                CV_FOURCC('M','J','P','G'),
                FPS,
                S,
                true);
        }
        cv::Mat currentFrame, annotatedFrame;
        int frameNumber = 0;
        //int procFrameNumber = 0;
        currentFrame = cam->getFrame();
        //int procFPS = cam->getFileFPS();
        while (true) {
            currentFrame = cam->getFrame();
            if( currentFrame.data == NULL || currentFrame.empty() || !cam->isStreaming()) {
                break;
            }
            annotatedFrame = currentFrame;
            if (frameNumber % (FPS / procFPS) == 0) {
                std::cout << "calling sort tracking" << std::endl;
                objectAnalytics.track(currentFrame, frameNumber);
                for(int i = 0; i < objectAnalytics.blobContainer.size(); i++) {
                    if (objectAnalytics.blobContainer[i].lastFrameNumber == frameNumber) {
                    //    objectAnalytics.blobContainer[i].metaData =
                    //        color + " " + objectAnalytics.blobContainer[i].label;
                    //        std::cout << "Color from Blob: " << objectAnalytics.blobContainer[i].metaData << std::endl;
                    }
                }
                //procFrameNumber++;
            }
            std::cout << "calling sort annotate" << std::endl;
            objectAnalytics.annotate(annotatedFrame);
            if (annotatedFrame.data != NULL) {
                cv::imshow("Annotated Object", annotatedFrame);
                cv::waitKey(10);
                if (argc > 2) {
                    annotatedVideo.write(annotatedFrame);
                }
            }
            frameNumber++;
        }
        annotatedVideo.release();
        std::cout << "AISAAC_LOG: Your video is 100% processed." << std::endl;
    } else {
        cv::Mat image = cv::imread(file, CV_LOAD_IMAGE_COLOR);

        if( !image.data ) {
            debugMsg("Unable to open file for reading");
            return -1;
        }

        objectAnalytics.track(image, 1);

        if ( !objectAnalytics.blobContainer.size() ) {
            std::cout << "AISAAC_LOG: No objects were detected in the input." << std::endl;
        } else {
            cv::Mat annotatedFrame = image;
            objectAnalytics.annotate(annotatedFrame,1);
            std::cout << "outputFilePath" << outputFilePath << std::endl;
            //cv::imshow("window", annotatedFrame);
            //cv::waitKey(0);
            cv::imwrite(outputFilePath, annotatedFrame);
            std::cout << "AISAAC_LOG: Your image has been processed." << std::endl;
        }

    }
}
