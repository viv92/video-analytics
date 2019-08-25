/*
    Copyright 2016 AITOE
*/

#include <cv.h>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <mutex>
#include "../../src/Summarization/videoSummarization.h"
#include "../../src/utils/json.hpp"
#include "../../tools/logging.h"
#include "../../tools/videoSources/fileStreamer.h"

#ifdef USE_MONGO
#include "../../tools/mongoLink/mongoLink.h"
#else
#include "../../tools/mongoLink/serialLink.h"
#endif //USE_MONGO


#include <dirent.h>
#include <fstream>
#include <iostream>
#include <string>

// usage: ./summarizationExample videoFile summarizationJsonFilePath aiSaacSettingsPath

int main(int argc, char *argv[]) {

    try {
        if ( argc < 4 ) {
            std::cout << "Usage: ./summarizationJsonifyExec [videoFile] [aiSaacSettingsPath] [mongoResultTableID]" << std::endl;
            return -1;
        }
        std::string videoFile = argv[1];
        std::string aiSaacSettingsPath = argv[2];
        std::string resultTableId = argv[3];

        FileStreamer* fileStreamer = new FileStreamer(videoFile);
        int FPS = fileStreamer->getFileFPS();

        aiSaac::AiSaacSettings *aiSaacSettings = new aiSaac::AiSaacSettings(aiSaacSettingsPath);
        aiSaac::VideoSummarization* videoSummarization = new aiSaac::VideoSummarization(aiSaacSettings, videoFile);
        //mongoLink *mongoObject = new mongoLink(aiSaacSettings->getProcFPS());
        mongoLink *mongoObject = new mongoLink(FPS);
        std::cout << "Calling summarizeGreedyOrdering" << std::endl << std::flush;
        videoSummarization->summarizeGreedyOrdering();
        std::cout << "Called summarizeGreedyOrdering" << std::endl << std::flush;
        std::vector<int> orderedKeyPoints = videoSummarization->getKeyPointsOrdered();
        std::cout << "Got keypoints" << std::endl << std::flush;
        mongoObject->summarizationMONGO(orderedKeyPoints, aiSaacSettings, resultTableId);
        std::cout << "Finished mongo" << std::endl << std::flush;
        return 0;
    } catch(...) {
        std::cout << "Exception" << std::endl << std::flush;
        return -1;
    }
}
