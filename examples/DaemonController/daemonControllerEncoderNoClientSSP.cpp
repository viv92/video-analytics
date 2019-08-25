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
#include "../../tools/videoSources/fileStreamer.h"
#include "../../tools/videoSources/aiVideoStreamer.h"
#include "../../src/aiSaac.h"
#include "../../tools/logging.h"
#include "../../tools/streamingServer/streamingServer.h"
#include "../../tools/streamingServer/commonStream.h"
#include "../../src/encoderCameraSettings.h"
#include "../../tools/logAlert/logAlert.h"

#ifdef USE_MONGO
#include "../../tools/mongoLink/mongoLink.h"
#else
#include "../../tools/mongoLink/serialLink.h"
#endif //USE_MONGO
const int max_length = 1024;

int SSPAnalyze(std::string videoFile, std::string aiSaacSettingsPath, std::string videoFileID, int motionFlag, int objectFlag, int debugFlag) {

  try {

    FileStreamer *fileStreamer;
    FileStreamer *beginnerFileStreamer;
    if ( videoFile.find("mp4") < videoFile.length() || videoFile.find("avi") < videoFile.length()  ) {
        //rewrite video in order to correct bad frames
        // std::string ffmpeg = "ffmpeg -i " + videoFile + " -c:v libx264 -pix_fmt yuv420p -movflags faststart " + videoFile + " -y";
        // system(ffmpeg.c_str());
        fileStreamer = new FileStreamer(videoFile);
        beginnerFileStreamer = new FileStreamer(videoFile);
        if(!(fileStreamer->isStreaming() && beginnerFileStreamer->isStreaming())) {
            debugMsg("Unable to open file for reading");
            return -1;
        } else {
            std::cout << "Found file for reading" << std::endl;
        }
    } else {
        debugMsg("Video not a MP4 or AVI file");
        return -1;
    }
    std::cout << "Initialising AISAAC settings" << std::endl;
    aiSaac::AiSaacSettings* aiSaacSettings = new aiSaac::AiSaacSettings(aiSaacSettingsPath);
    aiSaac::BgSubAnalytics* bgSubAnalytics = new aiSaac::BgSubAnalytics(*aiSaacSettings);
    aiSaac::ObjectAnalytics* objectAnalytics = new aiSaac::ObjectAnalytics(*aiSaacSettings);

    logAlert logger = logAlert();
    logger.sendLogAlert("Initialised all settings objects", "Trivial", "SSPAnalyze");

    cv::Mat currentFrame, scaledDownFrame, annotatedMotionFrame, annotatedObjectFrame;
    int frameNumber = 0;
    int totalFrames = fileStreamer->getNumberOfFrames();
    int FPS = fileStreamer->getFileFPS();
    int procFPS = aiSaacSettings->getProcFPS();
    cv::Size S = fileStreamer->getFrameSize();
    int videoDuration = totalFrames/FPS;
    int height, width;
    height = S.height;
    width = S.width;
    bool serialLinkOutputReady = false;

    std::cout << "VIDEO_INFO_START<<<" <<  "{\"height\": " << height << ", \"width\": "  << width << ", \"videoDuration\": "  << videoDuration << "}>>>VIDEO_INFO_END";

    if ( procFPS < 1 ) {
        procFPS = 1;
    } else if ( procFPS > FPS ) {
        procFPS = FPS;
    }
    std::cout << "!!!!!!!!!!ProcFPS = " << procFPS << std::endl << std::flush;
    mongoLink *mongoObject = new mongoLink(FPS); //since SSP output is original unaltered video
    std::string log_msg;
    char data[max_length];
    memset(data, '\0', max_length);

    logger.sendLogAlert("Starting analytics loop", "Important", "SSPAnalyze");

    std::cout << "-----initializing " << aiSaacSettings->getBackgroundSubtractionAlgo() << " algorithm----" << std::endl;
    bool motionAlgoInitFlag = false;
    while(!motionAlgoInitFlag) {
      currentFrame = beginnerFileStreamer->getFrame();
      if( currentFrame.data == NULL || currentFrame.empty() || !beginnerFileStreamer->isStreaming()) {
          break;
      }
      if (frameNumber % (FPS / procFPS) == 0) {

          cv::resize(currentFrame, scaledDownFrame, cv::Size(320, 180));
          //cv::resize(currentFrame, scaledDownFrame, S);
          if (bgSubAnalytics->detect(scaledDownFrame, frameNumber)) {
            motionAlgoInitFlag = true;
          }
      }
      frameNumber++;
    }
    delete beginnerFileStreamer;

    std::cout << "-----starting analysis loop----" << std::endl;
    frameNumber = 0; //restart from frame 1 after motion algo is initialized
    while (true) {

      currentFrame = fileStreamer->getFrame();
      if( currentFrame.data == NULL || currentFrame.empty() || !fileStreamer->isStreaming()) {
          break;
      }
      height = currentFrame.rows;
      width = currentFrame.cols;

      if (frameNumber % (FPS / procFPS) == 0) {

          cv::resize(currentFrame, scaledDownFrame, cv::Size(320, 180));
          //cv::resize(currentFrame, scaledDownFrame, S);
          if (bgSubAnalytics->detect(scaledDownFrame, frameNumber)) {
            if (motionFlag || objectFlag) {
              serialLinkOutputReady = true;
              if (motionFlag)
                bgSubAnalytics->track(scaledDownFrame, frameNumber);
              if (objectFlag)
                objectAnalytics->track(scaledDownFrame, frameNumber);
            }
          }
      }
      if (debugFlag) {
          if (motionFlag) {
            annotatedMotionFrame = scaledDownFrame.clone();
            bgSubAnalytics->annotate(annotatedMotionFrame, frameNumber);
            cv::imshow("Motion Annotations Window", annotatedMotionFrame);
            cv::waitKey(20);
          }
          if (objectFlag) {
            annotatedObjectFrame = scaledDownFrame.clone();
            objectAnalytics->annotate(annotatedObjectFrame, frameNumber);
            cv::imshow("Object Annotations Window", annotatedObjectFrame);
            cv::waitKey(20);
          }
      }
      if (serialLinkOutputReady) {
          if (motionFlag)
            mongoObject->motionMONGO(bgSubAnalytics->blobContainer, frameNumber, videoFileID);
          if (objectFlag)
            mongoObject->objectMONGO(objectAnalytics->blobContainer, frameNumber, videoFileID);
          serialLinkOutputReady = false;
      }
      frameNumber++;
      int percentProcessed = (frameNumber*100)/totalFrames;
      std::string analysisProgressOutput = "ANALYSIS_PROGRESS_START<<<" + std::to_string(percentProcessed) + ">>>ANALYSIS_PROGRESS_END";
      std::cout << analysisProgressOutput << std::endl;

    }
    delete mongoObject;
    delete fileStreamer;
    delete bgSubAnalytics;
    delete objectAnalytics;
    delete aiSaacSettings;
    logger.sendLogAlert("Deleted allocated objects", "Trivial", "SSPAnalyze");
    return 0;

  } catch(...) {
      std::cout << "There was an exception" << std::endl;
      return -1;
  }

}

int main(int argc, char const *argv[]) {
    if ( argc < 3) {
        std::cout << "Usage: ./daemonControllerEncoderNoClientSSP [videotoAnalyze] [aiSaacSettingsPath] [parentID] [motionFlag] [objectFlag] [debugFlag]" << std::endl;
        return -1;
    }

    std::string videoFile = argv[1];
    std::string aiSaacSettingsPath = argv[2];
    std::string videoFileID = argv[3];
    int motionFlag = atoi(argv[4]);
    int objectFlag = atoi(argv[5]);
    int debugFlag = atoi(argv[6]);
    std::cout << "Calling SSP analyze" << std::endl;
    SSPAnalyze(videoFile, aiSaacSettingsPath, videoFileID, motionFlag, objectFlag, debugFlag);

    return 0;
}
