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

#ifdef USE_MONGO
#include "../../tools/mongoLink/mongoLink.h"
#else
#include "../../tools/mongoLink/serialLink.h"
#endif //USE_MONGO

int main( int argc, char *argv[]) {
        if ( argc < 4 ) {
            std::cout << "Usage: ./bgSubJsonifyExec [videoFile] [aiSaacSettingsPath] [mongoResultTableID]" << std::endl;
            return -1;
        }
        std::string videoFile = argv[1];
        std::string aiSaacSettingsPath = argv[2];
        std::string resultTableId = argv[3];

        FileStreamer *fileStreamer = new FileStreamer(videoFile);
        if(!fileStreamer->isStreaming()) {
            debugMsg("Unable to open file for reading");
            return -1;
        }

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
            mongoObject->motionMONGO(bgSubAnalytics->blobContainer, frameNumber, resultTableId);
            frameNumber++;
        }
        return 0;
}
