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
#include "videoSources/libvlcStreamer.h"
#include "videoSources/webcamStreamer.h"
#include "videoSources/fileStreamer.h"
#include "videoSources/ffmpegStreamer.h"
#include "videoSources/aiVideoStreamer.h"
#include "developerConfig.h"
#include "logging.h"
#include "../src/aiSaac.h"
aiSaac::HumanAnalytics *humanAnalytics = NULL;
aiSaac::BgSubAnalytics *bgSubAnalytics = NULL;

bool isIntersect(cv::Rect rect1,cv::Rect rect2) {
    cv::Rect intersect = rect1 & rect2;
    if (intersect.area() == 0 )
        return false;
    return true;
}

void analyticsThread(FileStreamer *videoStreamer, aiSaac::AiSaacSettings *aiSaacSettings) {
    humanAnalytics = new aiSaac::HumanAnalytics(aiSaacSettings);
    bgSubAnalytics = new aiSaac::BgSubAnalytics(aiSaacSettings);
    cv::Mat currentFrame;
    int frameNumber = 0;
    cv::namedWindow("Display window", CV_WINDOW_AUTOSIZE);
    int count = 0;
    std::string personDirectory = aiSaacSettings->getFaceRecognitionTrainingDataPath();
    while (true) {

        if(!videoStreamer->isStreaming()) {
            std::cout << "Seems to be done" <<std::endl;
            break;
        }
        currentFrame = videoStreamer->getFrame();
        if(currentFrame.data == NULL) {
            std::cout << "Seems to be done" <<std::endl;
            break;
        }
        if (currentFrame.cols < 1 && currentFrame.rows <1)
            continue;
        bgSubAnalytics->track(currentFrame, frameNumber);
        humanAnalytics->track(currentFrame, frameNumber);
        for (int i = 0; i < humanAnalytics->blobContainer.size(); i++) {
            int j =0;
            bool isSave = false;
            if (humanAnalytics->blobContainer[i].lastFrameNumber == frameNumber) {
                if (humanAnalytics->blobContainer[i].lastRectangle.x < 0)
                    humanAnalytics->blobContainer[i].lastRectangle.x = 0;
                if (humanAnalytics->blobContainer[i].lastRectangle.y < 0)
                    humanAnalytics->blobContainer[i].lastRectangle.y = 0;
                if ((humanAnalytics->blobContainer[i].lastRectangle.x + humanAnalytics->blobContainer[i].lastRectangle.width) > currentFrame.cols)
                    humanAnalytics->blobContainer[i].lastRectangle.width = currentFrame.cols - humanAnalytics->blobContainer[i].lastRectangle.x;
                if ((humanAnalytics->blobContainer[i].lastRectangle.y + humanAnalytics->blobContainer[i].lastRectangle.height) > currentFrame.rows)
                    humanAnalytics->blobContainer[i].lastRectangle.height = currentFrame.rows - humanAnalytics->blobContainer[i].lastRectangle.y;
                for (j = 0; j < bgSubAnalytics->blobContainer.size(); j++) {
                    if (bgSubAnalytics->blobContainer[j].lastFrameNumber == frameNumber) {
                        if (bgSubAnalytics->blobContainer[j].lastRectangle.x < 0)
                            bgSubAnalytics->blobContainer[j].lastRectangle.x = 0;
                        if (bgSubAnalytics->blobContainer[j].lastRectangle.y < 0)
                            bgSubAnalytics->blobContainer[j].lastRectangle.y = 0;
                        if ((bgSubAnalytics->blobContainer[j].lastRectangle.x + bgSubAnalytics->blobContainer[j].lastRectangle.width) > currentFrame.cols)
                            bgSubAnalytics->blobContainer[j].lastRectangle.width = currentFrame.cols - bgSubAnalytics->blobContainer[j].lastRectangle.x;
                        if ((bgSubAnalytics->blobContainer[j].lastRectangle.y + bgSubAnalytics->blobContainer[j].lastRectangle.height) > currentFrame.rows)
                            bgSubAnalytics->blobContainer[j].lastRectangle.height = currentFrame.rows - bgSubAnalytics->blobContainer[j].lastRectangle.y;

                        if (isIntersect(humanAnalytics->blobContainer[i].lastRectangle,bgSubAnalytics->blobContainer[j].lastRectangle)) {
                            isSave = true;
                            break;
                        }
                    }
                }
                if (isSave) {
                    std::cout << "Getting ready to save " << std::endl;
                    cv::Mat prunedImage = currentFrame(humanAnalytics->blobContainer[i].lastRectangle);
                    if (prunedImage.rows > 0 && prunedImage.cols >0) {
                        std::string imagePath = personDirectory + "/Image" + std::to_string(count) + ".png";
                        std::cout <<"saving "<< imagePath << std::endl;
                        count++;
                        cv::imwrite(imagePath, prunedImage);
                    }
                }
            }
        }
        frameNumber++;
        cv::Mat annotatFrame;
        for (int i = 8; i>0;i--) {
            annotatFrame = videoStreamer->getFrame();
            if (annotatFrame.data != NULL) {
                cv::resize(annotatFrame,annotatFrame,
                    cv::Size((int)(annotatFrame.cols * 0.25), (int)(annotatFrame.rows* 0.25)));
                cv::imshow("Display window", annotatFrame);
                cv::waitKey(2);
            }
        }
    }
}

int main(int argc, char *argv[]) {
    setDeveloperVariables(developerName);
    std::cout << storagePath << std::endl;
    debugMsg("Camera Initialized\n");
    char** filePath;
    if (argc > 1) {
        filePath = argv[1];
        if (argc >2) {
            storagePath = argv [2];
        }
        debugMsg("Developer Name " + developerName);
        debugMsg("RTSP URL "+rtspURL);
    }
    initDebug(isDebug, storagePath);

    // std::cout << "Hello World" << std::endl;

    // Use VLC IP camera feed
    // LibvlcStreamer *cam = new LibvlcStreamer(rtspURL);
    // std::thread cam1(&LibvlcStreamer::startStream, cam);

    // Use webcam Streamer
    // WebcamStreamer *cam = new WebcamStreamer();
    // std::thread cam1(&WebcamStreamer::startStream,cam);

    // Use file Streamer
     FileStreamer *cam = new FileStreamer(filePath);
     if (!cam->isStreaming()) {
        std::cout << "Unable to open file for reading" << std::endl;
        debugMsg("Unable to open file for reading");
        return 0;
     }
     std::cout << "File Frame rate" << cam->getFileFPS() << std::endl;

    // Use FFMPEG Streamer
//     FfmpegStreamer *cam = new FfmpegStreamer(rtspURL);
//     std::thread cam1(&FfmpegStreamer::startStream, cam);

    while (!cam->isStreaming()) {}

    std::cout << "camera running now " << cam->getFrame().size() << std::endl;

    aiSaac::AiSaacSettings *aiSaacSettings =
        new aiSaac::AiSaacSettings(storagePath, "config1");
    analyticsThread(cam, aiSaacSettings);
}
