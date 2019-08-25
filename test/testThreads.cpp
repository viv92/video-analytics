#include <cv.h>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include "../tools/videoSources/fileStreamer.h"
#include "../tools/videoSources/aiVideoStreamer.h"
#include "../tools/logging.h"
#include "../src/aiSaac.h"
#include "../src/utils/colorClassifier.h"

#ifdef USE_MONGO
#include "../tools/mongoLink/mongoLink.h"
#else
#include "../tools/mongoLink/serialLink.h"
#endif //USE_MONGO

#include <mutex>
std::mutex out_mutex;

void objectThread(std::string file, std::string aiSaacSettingsPath, std::string resultTableId){

    std::cout << "In objectThread" << std::endl;
    aiSaac::AiSaacSettings *aiSaacSettings = new aiSaac::AiSaacSettings(aiSaacSettingsPath);
    aiSaac::ObjectAnalytics *objectAnalytics = new aiSaac::ObjectAnalytics(*aiSaacSettings);
    aiSaac::ColorClassifier *colorClassifier = new aiSaac::ColorClassifier();

      FileStreamer *fileStreamer = new FileStreamer(file);
      if(!fileStreamer->isStreaming()) {
          debugMsg("Unable to open file for reading");
      } else {
          std::cout << "Found file for reading" << std::endl;
      }

      cv::Mat currentFrame;
      int frameNumber = 0;
      int totalProcessedFrameNumber = 0;
      int FPS = fileStreamer->getFileFPS();
      int procFPS = FPS;
      mongoLink *mongoObject = new mongoLink(FPS);
      cv::Size S;

      std::cout << "here2" << std::endl;

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
      while (true) {
          currentFrame = fileStreamer->getFrame();
          if( currentFrame.data == NULL || currentFrame.empty() || !fileStreamer->isStreaming()) {
              break;
          }
          if (frameNumber % FPS / procFPS == 0) {
              objectAnalytics->track(currentFrame, frameNumber);
              for(int i = 0; i < objectAnalytics->blobContainer.size(); i++) {
                  if (objectAnalytics->blobContainer[i].lastFrameNumber == frameNumber) {
                      std::string color =
                          colorClassifier->patchColor(
                              currentFrame,
                              objectAnalytics->blobContainer[i].lastRectangle
                          );
                      objectAnalytics->blobContainer[i].metaData =
                          color + " " + objectAnalytics->blobContainer[i].label;
                          std::cout << "Color from Blob: " << objectAnalytics->blobContainer[i].metaData << std::endl;
                  }
              }
              out_mutex.lock();
              mongoObject->objectAnalyticsMONGO(objectAnalytics->blobContainer, frameNumber, resultTableId);
              out_mutex.unlock();
          }

          if (frameNumber == floor(totalProcessedFrameNumber*0.25)) {
              std::cout << "AISAAC_LOG: Your video is 25% processed." << std::endl;
          } else if (frameNumber == floor(totalProcessedFrameNumber*0.50)) {
              std::cout << "AISAAC_LOG: Your video is 50% processed." << std::endl;
          } else if (frameNumber == floor(totalProcessedFrameNumber*0.75)) {
              std::cout << "AISAAC_LOG: Your video is 75% processed." << std::endl;
          }
          frameNumber++;
      }
      std::cout << "AISAAC_LOG: Your video is 100% processed." << std::endl;
}

void bgThread(std::string videoFile, std::string aiSaacSettingsPath, std::string resultTableId){
    FileStreamer *fileStreamer = new FileStreamer(videoFile);
    if(!fileStreamer->isStreaming()) {
        debugMsg("Unable to open file for reading");
    }
    std::cout << "In motionThread" << std::endl;
    aiSaac::AiSaacSettings *aiSaacSettings = new aiSaac::AiSaacSettings(aiSaacSettingsPath);
    aiSaac::BgSubAnalytics *bgSubAnalytics = new aiSaac::BgSubAnalytics(*aiSaacSettings);
    mongoLink *mongoObject = new mongoLink(fileStreamer->getFileFPS());
    cv::Mat currentFrame;
    int frameNumber = 0;
    while (true) {
        currentFrame = fileStreamer->getFrame();
        if( currentFrame.data == NULL || currentFrame.empty() || !fileStreamer->isStreaming()) {
            break;
        }
        bgSubAnalytics->track(currentFrame, frameNumber);
        out_mutex.lock();
        mongoObject->motionMONGO(bgSubAnalytics->blobContainer, frameNumber, resultTableId);
        out_mutex.unlock();
        frameNumber++;
    }
}

int main(int argc, char const *argv[]) {
        if ( argc < 4 ) {
            std::cout << "Usage: ./testThreads [file] [aiSaacSettingsPath] [mongoResultTableID]" << std::endl;
            return -1;
        }
        std::string file = argv[1];
        std::string aiSaacSettingsPath = argv[2];
        std::string resultTableId = argv[3];
        std::cout << "Starting thread tests" << std::endl;

        std::thread t[2];

        t[0] = std::thread(objectThread, file, aiSaacSettingsPath, resultTableId);
        t[1] = std::thread(bgThread, file, aiSaacSettingsPath, "MLBGS", resultTableId);
        t[2] = std::thread(bgThread, file, aiSaacSettingsPath, "PBAS", resultTableId);

        std::cout << "Threads Launched" << std::endl;

        t[0].join();
        t[1].join();

    return 0;
}
