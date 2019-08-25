/*
    Copyright 2016 AITOE
*/

#include <cv.h>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <mutex>
#include "../../src/aiSaac.h"
#include "../../src/utils/json.hpp"
#include "../../tools/logging.h"

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
        if ( argc < 5 ) {
            std::cout << "Usage: ./summarizationStreamExample [videoFile] [aiSaacSettingsPath] [outputFolder] [summarizationThreshold]" << std::endl;
            return -1;
        }
        std::string videoFile = argv[1];
        std::string aiSaacSettingsPath = argv[2];
        std::string outputFolder = argv[3];
        std::string summarizationThreshold = argv[4];

        aiSaac::AiSaacSettings *aiSaacSettings = new aiSaac::AiSaacSettings(aiSaacSettingsPath);
        aiSaacSettings->setSummarizationThreshold(
            std::atof(summarizationThreshold.c_str()));
        std::cout << "Using Summarization Algorithm: " << aiSaacSettings->getSummarizationStreamAlgo() << std::endl;
        aiSaac::VideoSummarization* videoSummarization = new aiSaac::VideoSummarization(aiSaacSettings, videoFile);
        std::cout << "Calling summarizeStream" << std::endl << std::flush;
        videoSummarization->summarizeStream();
        std::cout << "Called summarizeStream" << std::endl << std::flush;
        std::string folderName = summarizationThreshold;
        std::replace(folderName.begin(), folderName.end(), '.', '_');
        folderName = outputFolder + "/" + folderName;
        std::string command = "mkdir -p \""
            + folderName
            +  "\"";
        std::cout << command << std::endl;
        system(command.c_str());

        videoSummarization->writeSummaryFile(folderName + "/summary.avi");
        std::cout << "Finished mongo" << std::endl << std::flush;
        return 0;
    } catch(...) {
        std::cout << "Exception" << std::endl << std::flush;
        return -1;
    }
}
