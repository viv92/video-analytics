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

#include <dirent.h>
#include <fstream>
#include <iostream>
#include <string>

// usage: ./summarizationExample videoFile summarizationJsonFilePath aiSaacSettingsPath

int main(int argc, char *argv[]) {

    std::string videoFilePath;
    std::string summarizationJsonFilePath;
    std::string aiSaacSettingsPath;

    if(argc == 4) {
        videoFilePath = argv[1];
        summarizationJsonFilePath = argv[2];
        aiSaacSettingsPath = argv[3];
        std::cout << "Summarizing: " << videoFilePath << std::endl;
    } else {
        std::cout << " Usage: ./summarizationExample videoFile summarizationJsonFilePath "
            << "aiSaacSettingsPath" << std::endl;
        return -1;
    }

    aiSaac::VideoSummarization* videoSummarization = NULL;
    std::string aiSaacSettingsFolderPath;
    std::string aiSaacSettingsFileName;
    for (int i = aiSaacSettingsPath.length() - 1; i >= 0; i--) {
        if (aiSaacSettingsPath[i] == '/') {
            aiSaacSettingsFolderPath = aiSaacSettingsPath.substr(0, i + 1);
            aiSaacSettingsFileName = aiSaacSettingsPath.substr(i + 1,
                aiSaacSettingsPath.length() - i);
            break;
        }
    }

    std::cout << "aiSaacSettingsFolderPath: " << aiSaacSettingsFolderPath << std::endl;
    std::cout << "aiSaacSettingsFileName: " << aiSaacSettingsFileName << std::endl;

    aiSaac::AiSaacSettings* aiSaacSettings = new aiSaac::AiSaacSettings(
            aiSaacSettingsFolderPath,
            aiSaacSettingsFileName);

    videoSummarization = new aiSaac::VideoSummarization(aiSaacSettings, videoFilePath);

    // codeReview(Anurag): shouldn't algo be in aiSaacSettings?
    videoSummarization->summarizeGreedyOrdering();
    // videoSummarization->writeSummaryFile(outputPath + "-" + algo + "-" + type + ".avi");
    std::vector<int> orderedKeyPoints = videoSummarization->getKeyPointsOrdered();
    nlohmann::json summarizationJson;
    std::vector<nlohmann::json> startEndTimes;
    std::cout << "orderedKeyPoints.size()" << orderedKeyPoints.size() << std::endl;
    for (int i = 0; i < orderedKeyPoints.size(); i++) {
        nlohmann::json startEndTime;
        int summaryTime = orderedKeyPoints[i] * aiSaacSettings->getSummarizationSNPSize();
        startEndTime["startTime"] = summaryTime;
        startEndTime["endTime"] = summaryTime + aiSaacSettings->getSummarizationSNPSize();
        startEndTimes.push_back(startEndTime);
    }
    std::cout << "startEndTimes.size(): " << startEndTimes.size() << std::endl;
    summarizationJson["summary"] = startEndTimes;
    // codeReview(Anurag): to be dumped to MongoDB instead of file!!
    std::ofstream summarizationJsonFile;
    summarizationJsonFile.open(summarizationJsonFilePath);
    summarizationJsonFile << summarizationJson.dump(4);
    summarizationJsonFile.close();
}
