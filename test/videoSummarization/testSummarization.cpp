/*
    Copyright 2016 AITOE
*/


#include <cv.h>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <thread>
#include <mutex>
#include "../../src/aiSaac.h"
#include "../../tools/logging.h"

#include <dirent.h>
#include <fstream>
#include <iostream>
#include <string>




int main(int argc, char *argv[]) {

    std::string folderPath;
    std::string outputPath;
    std::string algo;
    std::string type;
    std::string percent;


    if(argc == 5) {
        folderPath = argv[1];
        outputPath = argv[2];
        algo = argv[3];
        type = argv[4];
        std::cout << folderPath << std::endl;
        std::cout << outputPath << std::endl;
    }
    else if(argc==6) {
        folderPath = argv[1];
        outputPath = argv[2];
        algo = argv[3];
        type = argv[4];
        percent = argv[5];
        std::cout << folderPath << std::endl;
        std::cout << outputPath << std::endl;
    }
    else {
        std::cout<< "Usage:" <<std::endl <<" Usage: sceneVideoExample videotoLoad annotatedVideoPath";
    }


    aiSaac::VideoSummarization* videoSummarization = NULL;
    aiSaac::AiSaacSettings* aiSaacSettings = new aiSaac::AiSaacSettings(storagePath, "config1");

    std::string srcFilePathSet=folderPath;
    videoSummarization = new aiSaac::VideoSummarization(aiSaacSettings,srcFilePathSet);


    if (algo=="stream") {
        aiSaacSettings->setSummarizationStreamAlgo(type);
        videoSummarization->summarizeStream();
        videoSummarization->writeSummaryFile(outputPath+"-"+algo+"-"+type+".avi");
    }
    else if(algo=="greedy") {
        aiSaacSettings->setSummarizationGreedyAlgo(type);
        int num_percent = std::stoi(percent);
        videoSummarization->summarizeGreedy(num_percent);
        videoSummarization->writeSummaryFile(outputPath+"-"+algo+"-"+type+".avi");
    }
    else if(algo=="greedyO") {
        aiSaacSettings->setSummarizationGreedyAlgo(type);
        videoSummarization->summarizeGreedyOrdering();
        videoSummarization->writeSummaryFile(outputPath+"-"+algo+"-"+type+".avi");
    }
}
