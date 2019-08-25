//
// Created by aitoe on 12/09/2017
//

#include <iostream>
#include <cstdlib>
#include <cv.h>
#include <opencv2/highgui.hpp>
#include <thread>
#include <time.h>
#include "../../tools/videoSources/libvlcStreamer.h"
#include "../../tools/videoSources/aiVideoStreamer.h"
#include "../../src/aiSaac.h"
#include "../../tools/logging.h"
#include "../../tools/videoGrabber/videoGrabber.h"
#include "../../tools/streamingServer/streamingServer.h"
#include "../../tools/streamingServer/commonStream.h"
#include "../../tools/realTimeAlerts/realTimeAlerts.h"
#include "../../src/encoderCameraSettings.h"

#ifdef USE_MONGO
#include "../../tools/mongoLink/mongoLink.h"
#else
#include "../../tools/mongoLink/serialLink.h"
#endif //USE_MONGO

void initiateDaemonController(std::string outputFolderPath, std::string aiSaacSettingsPath, std::string encoderCameraSettingsPath) {
    std::cout << "Starting DaemonController" << std::endl;

    aiSaac::AiSaacSettings* aiSaacSettings = new aiSaac::AiSaacSettings(aiSaacSettingsPath);
    aiSaac::BgSubAnalytics* bgSubAnalytics = new aiSaac::BgSubAnalytics(*aiSaacSettings);
    encoderCameraSettings* camSettings = new encoderCameraSettings(encoderCameraSettingsPath);

    std::string cameraRTSP = camSettings->getCameraRTSP();
    std::string cameraIP = cameraRTSP.substr(cameraRTSP.find("@") + 1, (cameraRTSP.find(":", cameraRTSP.find("@")) - cameraRTSP.find("@")) - 1);

    LibvlcStreamer *rtspStreamer = new LibvlcStreamer(cameraRTSP);
    std::thread vlcStreamer(&LibvlcStreamer::startStream, rtspStreamer);

    while (!rtspStreamer->isStreaming()) {
        // Wait till stream is started.
    }

    cv::Mat currentFrame, motionDetectFrame;
    std::string motionResultTableId;
    cv::Size S = rtspStreamer->getFrame().size();
    int frameNumber = 0;
    int FPS = camSettings->getVideoGrabberFPS();
    int inactivityPeriod = camSettings->getInactivityPeriod();
    currentFrame = rtspStreamer->getFrame();
    int procFPS = aiSaacSettings->getProcFPS();

    if ( procFPS < 1 ) {
        procFPS = 1;
    } else if ( procFPS > FPS ) {
        procFPS = FPS;
    }

    outputFolderPath = outputFolderPath +  cameraIP + ":" + std::to_string(camSettings->getCameraStreamPort());
    std::string folderCreationCommand = "mkdir -p " + outputFolderPath;
    system(folderCreationCommand.c_str());

    videoGrabber vidGrab = videoGrabber(camSettings, S, outputFolderPath);
    realTimeAlerts alert = realTimeAlerts(camSettings);
    mongoLink* mongoObject = new mongoLink(FPS);

    while (true) {
        currentFrame = rtspStreamer->getFrame();
        frameNumber++;
        if( currentFrame.data == NULL || currentFrame.empty() || !rtspStreamer->isStreaming()) {
            break;
        }
        if (frameNumber % (FPS / procFPS) == 0) {
            vidGrab.forceReleaseVideo();
            cv::resize(currentFrame, motionDetectFrame, cv::Size(320, 180));
            if (bgSubAnalytics->detect(motionDetectFrame, frameNumber)) {
                if (alert.hasMotionEventBegun()) {
                    alert.sendMotionStartAlert(cv::Mat currentFrame);
                }
                std::cout << "Motion detected in frame: " << frameNumber << std::endl;
                motionResultTableId = vidGrab.acceptFrame(currentFrame, frameNumber);
                bgSubAnalytics->track(motionDetectFrame, frameNumber);
                mongoObject->motionMONGO(bgSubAnalytics->blobContainer, frameNumber, motionResultTableId);
            } else {
                if (alert.hasMotionEventFinished()) {
                    alert.sendMotionEndAlert();
                }
            }
        }
    }
    delete mongoObject;
    delete rtspStreamer;
    delete camSettings;
    delete bgSubAnalytics;
    delete aiSaacSettings;
}

int main(int argc, char const *argv[]) {
    if ( argc < 4 ) {
        std::cout << "Usage: ./daemonControllerEncoderNoClient [outputFolderPath] [aiSaacSettingsPath] [encoderCameraSettingsPath]" << std::endl;
        return -1;
    }

    std::string outputFolderPath = argv[1];
    std::string aiSaacSettingsPath = argv[2];
    std::string encoderCameraSettingsPath = argv[3];

    initiateDaemonController(outputFolderPath, aiSaacSettingsPath, encoderCameraSettingsPath);

    return 0;
}
