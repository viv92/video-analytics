#include <iostream>
#include <fstream>
#include <cv.h>
#include <opencv2/highgui.hpp>
#include <time.h>
#include "../../src/aiSaac.h"
#include "../../tools/videoSources/fileStreamer.h"
#include "../../tools/logging.h"
#include "../../src/utils/json.hpp"
#include "../../src/utils/caffeClassifier.h"
#include "datk.h"

int main(int argc, char *argv[]) {
    aiSaac::AiSaacSettings globalAiSaacSettings(storagePath + "/aiSaacSettingsconfig1.json");
    aiSaac::CaffeClassifier sceneClassifier(
        globalAiSaacSettings.getSummarizationScenePrototxtPath(),
        globalAiSaacSettings.getSummarizationSceneCaffemodelPath(),
        globalAiSaacSettings.getSummarizationSceneMeanFilePath(),
        globalAiSaacSettings.getSummarizationSceneLabelFilePath());
    aiSaac::CaffeClassifier ffObjectClassifier(
        globalAiSaacSettings.getSummarizationFfObjectPrototxtPath(),
        globalAiSaacSettings.getSummarizationFfObjectCaffemodelPath(),
        globalAiSaacSettings.getSummarizationFfObjectMeanFilePath(),
        globalAiSaacSettings.getSummarizationFfObjectLabelFilePath());
    aiSaac::CaffeClassifier dnObjectClassifier(
        globalAiSaacSettings.getSummarizationDnObjectPrototxtPath(),
        globalAiSaacSettings.getSummarizationDnObjectCaffemodelPath(),
        globalAiSaacSettings.getSummarizationDnObjectMeanFilePath(),
        globalAiSaacSettings.getSummarizationDnObjectLabelFilePath());
    aiSaac::CaffeClassifier faceGenderClassifier(
        globalAiSaacSettings.getSummarizationFaceGenderPrototxtPath(),
        globalAiSaacSettings.getSummarizationFaceGenderCaffemodelPath(),
        globalAiSaacSettings.getSummarizationFaceGenderMeanFilePath(),
        globalAiSaacSettings.getSummarizationFaceGenderLabelFilePath());
    aiSaac::CaffeClassifier faceAgeClassifier(
        globalAiSaacSettings.getSummarizationFaceAgePrototxtPath(),
        globalAiSaacSettings.getSummarizationFaceAgeCaffemodelPath(),
        globalAiSaacSettings.getSummarizationFaceAgeMeanFilePath(),
        globalAiSaacSettings.getSummarizationFaceAgeLabelFilePath());
    aiSaac::CaffeClassifier faceRecClassifier(
        globalAiSaacSettings.getSummarizationFaceRecPrototxtPath(),
        globalAiSaacSettings.getSummarizationFaceRecCaffemodelPath(),
        globalAiSaacSettings.getSummarizationFaceRecMeanFilePath(),
        globalAiSaacSettings.getSummarizationFaceRecLabelFilePath());

    aiSaac::ObjectAnalytics objectAnalytics(globalAiSaacSettings);
    aiSaac::FaceAnalytics faceAnalytics(globalAiSaacSettings);

    aiSaac::AiSaacSettings aiSaacSettings(storagePath + argv[1]);
    aiSaac::Summarization summarization(aiSaacSettings,
        argv[2],
        sceneClassifier,
        ffObjectClassifier,
        dnObjectClassifier,
        faceGenderClassifier,
        faceAgeClassifier,
        faceRecClassifier,
        objectAnalytics,
        faceAnalytics);

    summarization.featurizeVideo(std::atoi(argv[3]));
}
