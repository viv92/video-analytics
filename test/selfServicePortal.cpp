//
// Created by aitoe on 03/01/2018
//

#include <iostream>
#include <cstdlib>
#include <cv.h>
#include <opencv2/highgui/highgui.hpp>
#include <thread>
#include <chrono>
#include <time.h>
#include "../tools/videoSources/fileStreamer.h"
#include "../tools/videoSources/aiVideoStreamer.h"
#include "../src/aiSaac.h"

#ifdef USE_MONGO
#include "../tools/mongoLink/mongoLink.h"
#else
#include "../tools/mongoLink/serialLink.h"
#endif //USE_MONGO

int SSPAnalyze(std::string videoFile, std::string aiSaacSettingsPath, std::string videoFileID, int motionFlag, int objectFlag, int debugFlag, std::string outputFile) {

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
            return -1;
        } else {
            std::cout << "Found file for reading" << std::endl;
        }
    } else {
        return -1;
    }
    std::cout << "Initialising AISAAC settings" << std::endl;
    aiSaac::AiSaacSettings* aiSaacSettings = new aiSaac::AiSaacSettings(aiSaacSettingsPath);
    std::cout << "Initialising bgSubAnalytics" << std::endl;
    aiSaac::BgSubAnalytics* bgSubAnalytics = new aiSaac::BgSubAnalytics(*aiSaacSettings);
    aiSaac::ObjectAnalytics* objectAnalytics = new aiSaac::ObjectAnalytics(*aiSaacSettings);

    cv::Mat currentFrame, scaledDownFrame, annotatedMotionFrame, annotatedObjectFrame;
    int frameNumber = 0;
    int motionFrameNumber = 0;
    int totalFrames = fileStreamer->getNumberOfFrames();
    int FPS = fileStreamer->getFileFPS();
    int procFPS = aiSaacSettings->getProcFPS();
    cv::Size S = fileStreamer->getFrameSize();
    int videoDuration = totalFrames/FPS;
    int height, width;
    height = S.height;
    width = S.width;
    bool serialLinkOutputReady = false;

    cv::VideoWriter annotatedVideo;
    annotatedVideo.open(outputFile,
        CV_FOURCC('M','J','P','G'),
        FPS,
        cv::Size(320, 180),
        true);

    std::cout << "VIDEO_INFO_START<<<" <<  "{\"height\": " << height << ", \"width\": "  << width << ", \"videoDuration\": "  << videoDuration << "}>>>VIDEO_INFO_END";

    if ( procFPS < 1 ) {
        procFPS = 1;
    } else if ( procFPS > FPS ) {
        procFPS = FPS;
    }
    std::cout << "!!!!!!!!!!ProcFPS = " << procFPS << std::endl << std::flush;
    mongoLink *mongoObject = new mongoLink(FPS); //since SSP output is original unaltered video

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
              if (motionFlag) {
		 auto motStart = std::chrono::high_resolution_clock::now();
                 bgSubAnalytics->track(scaledDownFrame, frameNumber);
		 auto motEnd = std::chrono::high_resolution_clock::now();
		 std::cout << "Time taken for running motion track: " << std::chrono::duration_cast<std::chrono::microseconds>(motEnd - motStart).count() << std::endl;
		}
              if (objectFlag) {
		auto objStart = std::chrono::high_resolution_clock::now();
                objectAnalytics->track(scaledDownFrame, frameNumber, motionFrameNumber);
		auto objEnd = std::chrono::high_resolution_clock::now();
		std::cout << "Time taken for running object track: " << std::chrono::duration_cast<std::chrono::milliseconds>(objEnd - objStart).count() << std::endl;
}
            motionFrameNumber++;
		}
          }
      }
      if (debugFlag) {
          if (motionFlag) {
            annotatedMotionFrame = scaledDownFrame.clone();
            bgSubAnalytics->annotate(annotatedMotionFrame, frameNumber);
            //cv::imshow("Motion Annotations Window", annotatedMotionFrame);
            //cv::waitKey(20);
          }
          if (objectFlag) {
            annotatedObjectFrame = scaledDownFrame.clone();
            objectAnalytics->annotate(annotatedObjectFrame, frameNumber);
            cv::imshow("Object Annotations Window", annotatedObjectFrame);
            cv::waitKey(5);
            annotatedVideo.write(annotatedObjectFrame);
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

    annotatedVideo.release();

    delete mongoObject;
    delete fileStreamer;
    delete bgSubAnalytics;
    delete objectAnalytics;
    delete aiSaacSettings;
    return 0;

  } catch(...) {
      std::cout << "There was an exception" << std::endl;
      return -1;
  }

}

int main(int argc, char const *argv[]) {
    if ( argc < 3) {
        std::cout << "Usage: ./selfServicePortal [videotoAnalyze] [aiSaacSettingsPath] [parentID] [motionFlag] [objectFlag] [debugFlag] [outputFilePath]" << std::endl;
        return -1;
    }

    std::string videoFile = argv[1];
    std::string aiSaacSettingsPath = argv[2];
    std::string videoFileID = argv[3];
    int motionFlag = atoi(argv[4]);
    int objectFlag = atoi(argv[5]);
    int debugFlag = atoi(argv[6]);
    std::string outputFile = argv[7];
    std::cout << "Calling SSP analyze" << std::endl;
    SSPAnalyze(videoFile, aiSaacSettingsPath, videoFileID, motionFlag, objectFlag, debugFlag, outputFile);

    return 0;
}
