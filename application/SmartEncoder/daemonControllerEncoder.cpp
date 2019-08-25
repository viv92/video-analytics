//
// Created by aitoe on 12/09/2017
//

#include <iostream>
#include <cstdlib>
#include <cv.h>
#include <opencv2/highgui.hpp>
#include <thread>
#include <chrono>
#include <time.h>
#include "../../src/aiSaac.h"
#include "../../src/encoderCameraSettings.h"
#include "../../tools/videoSources/libvlcStreamer.h"
#include "../../tools/videoSources/aiVideoStreamer.h"
#include "../../tools/videoGrabber/videoGrabber.h"
#include "../../tools/streamingServer/streamingServer.h"
#include "../../tools/streamingServer/commonStream.h"
#include "../../tools/realTimeAlerts/realTimeAlerts.h"
#include "../../tools/realTimeAlerts/ruleAlert.h"
#include "../../tools/logAlert/logAlert.h"

#ifdef USE_MONGO
#include "../../tools/mongoLink/mongoLink.h"
#else
#include "../../tools/mongoLink/serialLink.h"
#endif //USE_MONGO

std::string generateRandomString() {
    std::string alphanum = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::string randomString;
    srand(time(NULL));
    for (int i = 0; i < 10; ++i) {
        randomString.push_back(alphanum[rand() % (sizeof(alphanum) - 1)]);
    }
    return randomString;
}

void initiateDaemonController(std::string storageFolderPath, std::string aiSaacSettingsPath, std::string encoderCameraSettingsPath) {

    aiSaac::AiSaacSettings* aiSaacSettings = new aiSaac::AiSaacSettings(aiSaacSettingsPath);
    aiSaac::BgSubAnalytics* bgSubAnalytics = new aiSaac::BgSubAnalytics(*aiSaacSettings);
    aiSaac::ObjectAnalytics* objectAnalytics = new aiSaac::ObjectAnalytics(*aiSaacSettings);
    aiSaac::FaceAnalytics* faceAnalytics = new aiSaac::FaceAnalytics(*aiSaacSettings);
    encoderCameraSettings* camSettings = new encoderCameraSettings(encoderCameraSettingsPath);

    logAlert logger = logAlert();

    logger.sendLogAlert("Initialised all settings objects", "Trivial", "initiateDaemonController");

    cv::Mat currentFrame, motionDetectFrame, annotatedMotionFrame, annotatedObjectFrame;
    std::string outputFolderPath, debugFolderPath, folderCreationCommand, videoChunkParentId, debugVideoChunkParentId;
    struct tm* currentTimeStruct;
    struct tm* lastMotionEventBeginTimeStruct;
    bool serialLinkOutputReady = false;

    bool viewMotionAnnotateFlag = camSettings->getViewMotionAnnotateFlag();
    bool writeMotionAnnotateFlag = camSettings->getWriteMotionAnnotateFlag();
    bool objectAnalyticsEnabledFlag = camSettings->getObjectAnalyticsEnabledFlag();
    bool faceAnalyticsEnabledFlag = camSettings->getFaceAnalyticsEnabledFlag();
    bool viewObjectAnnotateFlag = camSettings->getViewObjectAnnotateFlag();
    bool writeObjectAnnotateFlag = camSettings->getWriteObjectAnnotateFlag();
    bool loiteringAlertEnabled = camSettings->getLoiteringAlertEnabledFlag();
    bool objectAlertEnabled = camSettings->getObjectAlertEnabledFlag();
    bool motionAlertEnabled = camSettings->getMotionAlertEnabledFlag();
    double scaleFactor = camSettings->getScaleFactor();
    int FPS = camSettings->getVlcFPS();
    int vidGrabFPS = camSettings->getVideoGrabberFPS();
    int inactivityPeriod = camSettings->getInactivityPeriod();
    int minimumChunkSize = camSettings->getMinimumChunkSize();
    int maximumChunkSize = camSettings->getMaximumChunkSize();
    int procFPS = aiSaacSettings->getProcFPS();
    int ruleCount = camSettings->getRuleCount();
    std::string faceImageStoragePath = camSettings->getFaceImageStoragePath();
    std::string alertImageStoragePath = camSettings->getAlertImageStoragePath();

    logger.sendLogAlert("Extracted settings from encoderCameraSettings", "Trivial", "initiateDaemonController");

    realTimeAlerts* rtAlerts = new realTimeAlerts(camSettings, procFPS);
    ruleAlert rtRuleAlert[10];
    if (ruleCount > 0) {
        for (int  i = 0; i < ruleCount; i++) {
            rtRuleAlert[i] = ruleAlert(camSettings, i);
        }
    }
    // Sanity check to make sure object annotations are only enabled when objectAnalytics is enabled
    if (!objectAnalyticsEnabledFlag) {
        viewObjectAnnotateFlag = false;
        writeObjectAnnotateFlag = false;
    }

    int frameNumber = 0;
    int procFrameNumber = 0;
    int chunkStartFrameNumber = 0;
    bool isMotionEvent = false;
    int lastMotionEventHour = -1;
    time_t currentTime = time(0);
    time_t lastMotionTime = NULL;

    std::string cameraRTSP = camSettings->getCameraRTSP();
    std::string cameraIP = cameraRTSP.substr(cameraRTSP.find("@") + 1, (cameraRTSP.find(":", cameraRTSP.find("@")) - cameraRTSP.find("@")) - 1);

    LibvlcStreamer* rtspStreamer = new LibvlcStreamer(cameraRTSP, FPS);
    std::thread vlcStreamer(&LibvlcStreamer::startStream, rtspStreamer);
    mongoLink* mongoObject = new mongoLink(procFPS);

    while (!rtspStreamer->isStreaming()) {
        // Wait till stream is started.
    }

    logger.sendLogAlert("RTSP Streaming Started", "Trivial", "initiateDaemonController");

    cv::Size S = rtspStreamer->getFrame().size();
    currentFrame = rtspStreamer->getFrame();

    if ( procFPS < 1 ) {
        procFPS = 1;
    } else if ( procFPS > FPS ) {
        procFPS = FPS;
    }

    outputFolderPath = storageFolderPath +  cameraIP;
    debugFolderPath = outputFolderPath + "/debugFiles";
    folderCreationCommand = "mkdir -p " + outputFolderPath + " " + debugFolderPath + " " + faceImageStoragePath + " " + alertImageStoragePath;
    system(folderCreationCommand.c_str());

    logger.sendLogAlert("Created outputFolder at " + outputFolderPath, "Important", "initiateDaemonController");

    videoGrabber vidGrab = videoGrabber(camSettings, cv::Size(320, 180), outputFolderPath, "MAIN");
    videoGrabber debugMotionVidGrab = videoGrabber(camSettings, cv::Size(320, 180), debugFolderPath, "MOTION");
    videoGrabber debugObjectVidGrab = videoGrabber(camSettings, cv::Size(320, 180), debugFolderPath, "OBJECT");

    logger.sendLogAlert("Starting analytics loop", "Important", "initiateDaemonController");
    while (true) {
        currentFrame = rtspStreamer->getFrame();
        frameNumber++;

        if (currentFrame.data == NULL || currentFrame.empty() || !rtspStreamer->isStreaming()) {
            logger.sendLogAlert("Corrupt frame", "Minor Error", "initiateDaemonController");
            break;
        }

        if (frameNumber % (FPS / procFPS) == 0) {
            cv::resize(currentFrame, motionDetectFrame, cv::Size(320, 180));
            if (viewMotionAnnotateFlag || writeMotionAnnotateFlag) {
                annotatedMotionFrame = motionDetectFrame.clone();
            }
            if (viewObjectAnnotateFlag || writeObjectAnnotateFlag) {
                annotatedObjectFrame = motionDetectFrame.clone();
            }
            if (isMotionEvent) {
                if (((difftime(time(0), lastMotionTime) > inactivityPeriod) && (vidGrab.videoChunkFrameNumber() > (vidGrabFPS * minimumChunkSize))) || (vidGrab.videoChunkFrameNumber() > (vidGrabFPS * maximumChunkSize))) {
                    isMotionEvent = false;
                    logger.sendLogAlert("Motion event finished", "Important", "initiateDaemonController");
                    if (motionAlertEnabled) {
                        rtAlerts->sendMotionAlertEnd();
                    }
                    vidGrab.releaseVideo();
                    if (writeMotionAnnotateFlag) {
                        debugMotionVidGrab.releaseVideo();
                    }
                    if (writeObjectAnnotateFlag) {
                        debugObjectVidGrab.releaseVideo();
                    }
                } else {
                    currentTime = time(0);
                    currentTimeStruct = localtime(&currentTime);
                    if (currentTimeStruct->tm_hour != lastMotionEventHour) {
                        isMotionEvent = false;
                        vidGrab.releaseVideo();
                        if (writeMotionAnnotateFlag) {
                            debugMotionVidGrab.releaseVideo();
                        }
                        if (writeObjectAnnotateFlag) {
                            debugObjectVidGrab.releaseVideo();
                        }
                        logger.sendLogAlert("Released video at hour mark", "Important", "initiateDaemonController");
                    }
                }
            }
            if (loiteringAlertEnabled) {
                for (int i = 0; i < bgSubAnalytics->blobContainer.size(); i++) {
                    for (int j = 0; j < ruleCount; j++) {
                        if (rtRuleAlert[j].getRuleType() == "loitering") {
                            rtAlerts->sendLoiteringRuleAlertEnd(bgSubAnalytics->blobContainer[i], rtRuleAlert[j]);
                        }
                    }
                }
            }
            if (objectAnalyticsEnabledFlag) {
                if (objectAlertEnabled) {
                    for (int i = 0; i < objectAnalytics->blobContainer.size(); i++) {
                        for (int j = 0; j < ruleCount; j++) {
                            if (rtRuleAlert[j].getRuleType() == "object") {
                                rtAlerts->sendObjectRuleAlertEnd(objectAnalytics->blobContainer[i], rtRuleAlert[j]);
                            }
                        }
                    }
                }
            }
            if (bgSubAnalytics->detect(motionDetectFrame, frameNumber)) {
                if (!isMotionEvent) {
                    logger.sendLogAlert("Motion event started at frameNumber: " + std::to_string(frameNumber), "Important", "initiateDaemonController");
                    lastMotionTime = time(0);
                    lastMotionEventBeginTimeStruct = localtime(&lastMotionTime);
                    lastMotionEventHour = lastMotionEventBeginTimeStruct->tm_hour;
                    chunkStartFrameNumber = frameNumber;
                    if (motionAlertEnabled) {
                        rtAlerts->sendMotionAlertStart(motionDetectFrame);
                    }
                }
                isMotionEvent = true;
                lastMotionTime = time(0);
                serialLinkOutputReady = true;
                procFrameNumber++;
                bgSubAnalytics->track(motionDetectFrame, procFrameNumber);
                if (loiteringAlertEnabled) {
                    for (int i = 0; i < bgSubAnalytics->blobContainer.size(); i++) {
                        if (bgSubAnalytics->blobContainer[i].lastFrameNumber == procFrameNumber) {
                            for (int j = 0; j < ruleCount; j++) {
                                if (rtRuleAlert[j].getRuleType() == "loitering") {
                                    rtAlerts->sendLoiteringRuleAlertStart(bgSubAnalytics->blobContainer[i], motionDetectFrame, rtRuleAlert[j]);
                                }
                            }
                        }
                    }
                }
                if (objectAnalyticsEnabledFlag) {
                    objectAnalytics->track(motionDetectFrame, procFrameNumber);
                    if (objectAlertEnabled) {
                        for (int i = 0; i < objectAnalytics->blobContainer.size(); i++) {
                            if (objectAnalytics->blobContainer[i].lastFrameNumber == procFrameNumber) {
                                for (int j = 0; j < ruleCount; j++) {
                                    if (rtRuleAlert[j].getRuleType() == "object") {
                                        rtAlerts->sendObjectRuleAlertStart(objectAnalytics->blobContainer[i], motionDetectFrame, rtRuleAlert[j]);
                                    }
                                }
                            }
                        }
                    }
                }
                if (faceAnalyticsEnabledFlag) {
                    std::string faceImagePath;
                    faceAnalytics->track(currentFrame, procFrameNumber);
                    for (int i = 0; i < faceAnalytics->blobContainer.size(); i++) {
                        if (faceAnalytics->blobContainer[i].lastFrameNumber == procFrameNumber) {
                            std::string fileName = faceImageStoragePath + '/' + generateRandomString() + ".jpg";
                            cv::imwrite(fileName, currentFrame(faceAnalytics->blobContainer[i].lastRectangle));
                            faceImagePath = "FACE_REC_BLOB_START<<<" + fileName + ">>>FACE_REC_BLOB_END";
                            std::cout << faceImagePath << std::endl;
                        }
                    }
                }
                if (viewMotionAnnotateFlag || writeMotionAnnotateFlag) {
                    bgSubAnalytics->annotate(annotatedMotionFrame, procFrameNumber);
                }
                if (viewObjectAnnotateFlag || writeObjectAnnotateFlag) {
                    objectAnalytics->annotate(annotatedObjectFrame, procFrameNumber);
                }
            }
            if (viewMotionAnnotateFlag) {
                cv::imshow("Motion Annotations Window", annotatedMotionFrame);
            }
            if (viewObjectAnnotateFlag) {
                cv::imshow("Object Annotations Window", annotatedObjectFrame);
            }
            if (isMotionEvent) {
                videoChunkParentId = vidGrab.acceptFrame(motionDetectFrame);
                if (writeMotionAnnotateFlag) {
                    debugVideoChunkParentId = debugMotionVidGrab.acceptFrame(annotatedMotionFrame);
                }
                if (writeObjectAnnotateFlag) {
                    debugVideoChunkParentId = debugObjectVidGrab.acceptFrame(annotatedObjectFrame);
                }
            }
            if (serialLinkOutputReady) {
                mongoObject->motionMONGO(bgSubAnalytics->blobContainer, procFrameNumber, videoChunkParentId, chunkStartFrameNumber);
                if (objectAnalyticsEnabledFlag) {
                    mongoObject->objectMONGO(objectAnalytics->blobContainer, procFrameNumber, videoChunkParentId, chunkStartFrameNumber);
                }
                serialLinkOutputReady = false;
            }
        }
    }
    delete mongoObject;
    delete rtspStreamer;
    delete camSettings;
    delete bgSubAnalytics;
    delete aiSaacSettings;
    logger.sendLogAlert("Deleted allocated objects", "Trivial", "initiateDaemonController");
}

int main(int argc, char const *argv[]) {
    if ( argc < 4 ) {
        std::cout << "Usage: ./daemonControllerEncoder [storageFolderPath] [aiSaacSettingsPath] [encoderCameraSettingsPath]" << std::endl;
        return -1;
    }

    std::string storageFolderPath = argv[1];
    std::string aiSaacSettingsPath = argv[2];
    std::string encoderCameraSettingsPath = argv[3];

    initiateDaemonController(storageFolderPath, aiSaacSettingsPath, encoderCameraSettingsPath);

    return 0;
}
