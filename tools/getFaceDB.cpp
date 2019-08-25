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


void analyticsThread(aiSaac::AiSaacSettings *aiSaacSettings) {
    faceAnalytics = new aiSaac::FaceAnalytics(aiSaacSettings);
    faceAnalytics->faceRecognition->addStoredFacesToDB();
}

int main(int argc, char *argv[]) {
    setDeveloperVariables(developerName);
    std::cout << storagePath << std::endl;
    debugMsg("Camera Initialized\n");
    initDebug(isDebug, storagePath);
    
    aiSaac::AiSaacSettings *aiSaacSettings =
        new aiSaac::AiSaacSettings(storagePath, "config1");
    analyticsThread(aiSaacSettings);
}
