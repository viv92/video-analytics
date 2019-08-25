/*
    Copyright 2016 AITOE
*/

/*
    TODO:
    1. Error handling if settingsFilePath/instandID passed is invalid (to wrong folder etc.)
    2. In constructor accept only one value: caSettingsFilePath
*/

#include "aiSaacSettings.h"

aiSaac::AiSaacSettings::AiSaacSettings(std::string settingsFilePath) {
    this->settingsFileName = settingsFilePath;
    std::ifstream f(this->settingsFileName);
    if (f.good()) {
        std::cout << "Loading aiSaacSettings ettings" << std::endl;
        this->loadSettings();
        std::cout << "Loaded aiSaacSettings settings" << std::endl;
    } else {
        std::cout << "Cannot find settings file. Creating." << std::endl;
        this->resetDefaultValues();
        // todo advanceSettings->resetDefaultValues();
    }
}

aiSaac::AiSaacSettings::AiSaacSettings(std::string settingsFilePath,
    std::string instanceId) {
    std::cout << "AiSaacSettings(std::string, std::string) is deprecated, " <<
            "Use AiSaacSettings(std::string) instead" << std::endl;
    this->instanceId = instanceId;
    std::string fileName = "/aiSaacSettings" + this->instanceId + ".json";

    // std::cout << "Creating aiSaac settings for " << this->instanceId
            // << std::endl;

    // std::array<char, 4> wildChars {{'.', ':', '/', '\\'}};
    // for (int i = 0; i < wildChars.size(); i++) {
    //     std::replace(fileName.begin(), fileName.end(), wildChars[i], '_');
    // }
    this->settingsFilePath = settingsFilePath;
    this->settingsFileName = settingsFilePath + fileName;

    std::ifstream f(this->settingsFileName);
    if (f.good()) {
        std::cout << "Loading settings" << std::endl;
        this->loadSettings();
    } else {
        std::cout << "Cannot find settings file. Creating." << std::endl;
        this->resetDefaultValues();
        // todo advanceSettings->resetDefaultValues();
    }
}

aiSaac::AiSaacSettings::~AiSaacSettings() {
    std::cout << "deleting aisaac settings" << std::endl;
}

void aiSaac::AiSaacSettings::setSummarizationThreshold(double value) {
    this->settings["aiSaacSettings"]["VideoSummarization"]["summarizationThreshold"] = value;
    this->saveSettings();
}

double aiSaac::AiSaacSettings::getSummarizationThreshold() {
    return this->settings["aiSaacSettings"]["VideoSummarization"]["summarizationThreshold"];
}

void aiSaac::AiSaacSettings::setSummarizationSNPSize(int size) {
  this->settings["aiSaacSettings"]["VideoSummarization"]["summarizationSNPSize"] = size;
  this->saveSettings();
}

int aiSaac::AiSaacSettings::getSummarizationSNPSize() {
  return this->settings["aiSaacSettings"]["VideoSummarization"]["summarizationSNPSize"];
}

void aiSaac::AiSaacSettings::setSummarizationStreamAlgo(std::string algo) {
  this->settings["aiSaacSettings"]["VideoSummarization"]["summarizationStreamAlgo"] = algo;
  this->saveSettings();
}

std::string aiSaac::AiSaacSettings::getSummarizationStreamAlgo() {
  return this->settings["aiSaacSettings"]["VideoSummarization"]["summarizationStreamAlgo"];
}

void aiSaac::AiSaacSettings::setSummarizationGreedyAlgo(std::string algo) {
  this->settings["aiSaacSettings"]["VideoSummarization"]["summarizationGreedyAlgo"] = algo;
  this->saveSettings();
}

std::string aiSaac::AiSaacSettings::getSummarizationGreedyAlgo() {
  return this->settings["aiSaacSettings"]["VideoSummarization"]["summarizationGreedyAlgo"];
}

//
void aiSaac::AiSaacSettings::setSummarizationCollectionDir(std::string filePath) {
    this->settings["aiSaacSettings"]["VideoSummarization"]["summarizationFeaturesDir"] = filePath;
}

std::string aiSaac::AiSaacSettings::getSummarizationCollectionDir() {
    return this->settings["aiSaacSettings"]["VideoSummarization"]["summarizationFeaturesDir"];
}

void aiSaac::AiSaacSettings::setSummarizationScenePrototxtPath(std::string filePath) {
    this->settings["aiSaacSettings"]["VideoSummarization"]["summarizationScenePrototxtPath"]
        = filePath;
}

std::string aiSaac::AiSaacSettings::getSummarizationScenePrototxtPath() {
    return
        this->settings["aiSaacSettings"]["VideoSummarization"]["summarizationScenePrototxtPath"];
}

void aiSaac::AiSaacSettings::setSummarizationSceneCaffemodelPath(std::string filePath) {
    this->settings["aiSaacSettings"]["VideoSummarization"]["summarizationSceneCaffemodelPath"]
        = filePath;
}
std::string aiSaac::AiSaacSettings::getSummarizationSceneCaffemodelPath() {
    return
        this->settings["aiSaacSettings"]["VideoSummarization"]["summarizationSceneCaffemodelPath"];
}

void aiSaac::AiSaacSettings::setSummarizationSceneMeanFilePath(std::string filePath) {
    this->settings["aiSaacSettings"]["VideoSummarization"]["summarizationSceneMeanFilePath"]
        = filePath;
}
std::string aiSaac::AiSaacSettings::getSummarizationSceneMeanFilePath() {
    return this->settings["aiSaacSettings"]["VideoSummarization"]["summarizationSceneMeanFilePath"];
}

void aiSaac::AiSaacSettings::setSummarizationSceneLabelFilePath(std::string filePath) {
    this->settings["aiSaacSettings"]["VideoSummarization"]["summarizationSceneLabelFilePath"]
        = filePath;
}
std::string aiSaac::AiSaacSettings::getSummarizationSceneLabelFilePath() {
    return this->settings["aiSaacSettings"]["VideoSummarization"]["summarizationSceneLabelFilePath"];
}

void aiSaac::AiSaacSettings::setSummarizationSceneFeatureExtractionLayer(std::string filePath) {
    this->settings["aiSaacSettings"]["VideoSummarization"]["summarizationSceneFeatureExtractionLayer"]
        = filePath;
}
std::string aiSaac::AiSaacSettings::getSummarizationSceneFeatureExtractionLayer() {
    return this->settings["aiSaacSettings"]["VideoSummarization"]["summarizationSceneFeatureExtractionLayer"];
}

void aiSaac::AiSaacSettings::setSummarizationFfObjectPrototxtPath(std::string filePath) {
    this->settings["aiSaacSettings"]["VideoSummarization"]["summarizationFfObjectPrototxtPath"]
        = filePath;
}
std::string aiSaac::AiSaacSettings::getSummarizationFfObjectPrototxtPath() {
    return this->settings["aiSaacSettings"]["VideoSummarization"]["summarizationFfObjectPrototxtPath"];
}

void aiSaac::AiSaacSettings::setSummarizationFfObjectCaffemodelPath(std::string filePath) {
    this->settings["aiSaacSettings"]["VideoSummarization"]["summarizationFfObjectCaffemodelPath"]
        = filePath;
}
std::string aiSaac::AiSaacSettings::getSummarizationFfObjectCaffemodelPath() {
    return this->settings["aiSaacSettings"]["VideoSummarization"]["summarizationFfObjectCaffemodelPath"];
}

void aiSaac::AiSaacSettings::setSummarizationFfObjectMeanFilePath(std::string filePath) {
    this->settings["aiSaacSettings"]["VideoSummarization"]["summarizationFfObjectMeanFilePath"]
        = filePath;
}
std::string aiSaac::AiSaacSettings::getSummarizationFfObjectMeanFilePath() {
    return this->settings["aiSaacSettings"]["VideoSummarization"]["summarizationFfObjectMeanFilePath"];
}

void aiSaac::AiSaacSettings::setSummarizationFfObjectLabelFilePath(std::string filePath) {
    this->settings["aiSaacSettings"]["VideoSummarization"]["summarizationFfObjectLabelFilePath"]
        = filePath;
}
std::string aiSaac::AiSaacSettings::getSummarizationFfObjectLabelFilePath() {
    return this->settings["aiSaacSettings"]["VideoSummarization"]["summarizationFfObjectLabelFilePath"];
}

void aiSaac::AiSaacSettings::setSummarizationFfObjectFeatureExtractionLayer(std::string filePath) {
    this->settings["aiSaacSettings"]
        ["VideoSummarization"]
        ["summarizationFfObjectFeatureExtractionLayer"] = filePath;
}
std::string aiSaac::AiSaacSettings::getSummarizationFfObjectFeatureExtractionLayer() {
    return this->settings["aiSaacSettings"]
        ["VideoSummarization"]
        ["summarizationFfObjectFeatureExtractionLayer"];
}

void aiSaac::AiSaacSettings::setSummarizationDnObjectPrototxtPath(std::string filePath) {
    this->settings["aiSaacSettings"]["VideoSummarization"]["summarizationDnObjectPrototxtPath"]
        = filePath;
}
std::string aiSaac::AiSaacSettings::getSummarizationDnObjectPrototxtPath() {
    return this->settings["aiSaacSettings"]["VideoSummarization"]["summarizationDnObjectPrototxtPath"];
}

void aiSaac::AiSaacSettings::setSummarizationDnObjectCaffemodelPath(std::string filePath) {
    this->settings["aiSaacSettings"]["VideoSummarization"]["summarizationDnObjectCaffemodelPath"]
        = filePath;
}
std::string aiSaac::AiSaacSettings::getSummarizationDnObjectCaffemodelPath() {
    return this->settings["aiSaacSettings"]["VideoSummarization"]["summarizationDnObjectCaffemodelPath"];
}

void aiSaac::AiSaacSettings::setSummarizationDnObjectMeanFilePath(std::string filePath) {
    this->settings["aiSaacSettings"]["VideoSummarization"]["summarizationDnObjectMeanFilePath"]
        = filePath;
}
std::string aiSaac::AiSaacSettings::getSummarizationDnObjectMeanFilePath() {
    return this->settings["aiSaacSettings"]["VideoSummarization"]["summarizationDnObjectMeanFilePath"];
}

void aiSaac::AiSaacSettings::setSummarizationDnObjectLabelFilePath(std::string filePath) {
    this->settings["aiSaacSettings"]["VideoSummarization"]["summarizationDnObjectLabelFilePath"]
        = filePath;
}
std::string aiSaac::AiSaacSettings::getSummarizationDnObjectLabelFilePath() {
    return this->settings["aiSaacSettings"]["VideoSummarization"]["summarizationDnObjectLabelFilePath"];
}

void aiSaac::AiSaacSettings::setSummarizationDnObjectFeatureExtractionLayer(std::string filePath) {
    this->settings["aiSaacSettings"]
        ["VideoSummarization"]
        ["summarizationDnObjectFeatureExtractionLayer"] = filePath;
}
std::string aiSaac::AiSaacSettings::getSummarizationDnObjectFeatureExtractionLayer() {
    return this->settings["aiSaacSettings"]
        ["VideoSummarization"]
        ["summarizationDnObjectFeatureExtractionLayer"];
}
//
void aiSaac::AiSaacSettings::setSummarizationFaceGenderPrototxtPath(std::string filePath) {
    this->settings["aiSaacSettings"]["VideoSummarization"]["summarizationDnObjectLabelFilePath"]
        = filePath;
}
std::string aiSaac::AiSaacSettings::getSummarizationFaceGenderPrototxtPath() {
    return this->settings["aiSaacSettings"]["VideoSummarization"]["summarizationFaceGenderPrototxtPath"];
}

void aiSaac::AiSaacSettings::setSummarizationFaceGenderCaffemodelPath(std::string filePath) {
    this->settings["aiSaacSettings"]["VideoSummarization"]["summarizationFaceGenderPrototxtPath"]
        = filePath;
}
std::string aiSaac::AiSaacSettings::getSummarizationFaceGenderCaffemodelPath() {
    return this->settings["aiSaacSettings"]["VideoSummarization"]["summarizationFaceGenderCaffemodelPath"];
}

void aiSaac::AiSaacSettings::setSummarizationFaceGenderMeanFilePath(std::string filePath) {
    this->settings["aiSaacSettings"]["VideoSummarization"]["summarizationFaceGenderMeanFilePath"]
        = filePath;
}
std::string aiSaac::AiSaacSettings::getSummarizationFaceGenderMeanFilePath() {
    return this->settings["aiSaacSettings"]["VideoSummarization"]["summarizationFaceGenderMeanFilePath"];
}

void aiSaac::AiSaacSettings::setSummarizationFaceGenderLabelFilePath(std::string filePath) {
    this->settings["aiSaacSettings"]["VideoSummarization"]["summarizationFaceGenderLabelFilePath"]
        = filePath;
}
std::string aiSaac::AiSaacSettings::getSummarizationFaceGenderLabelFilePath() {
    return this->settings["aiSaacSettings"]["VideoSummarization"]["summarizationFaceGenderLabelFilePath"];
}

void aiSaac::AiSaacSettings::setSummarizationFaceGenderFeatureExtractionLayer(std::string filePath) {
    this->settings["aiSaacSettings"]
        ["VideoSummarization"]
        ["summarizationFaceGenderFeatureExtractionLayer"] = filePath;
}
std::string aiSaac::AiSaacSettings::getSummarizationFaceGenderFeatureExtractionLayer() {
    return this->settings["aiSaacSettings"]
        ["VideoSummarization"]
        ["summarizationFaceGenderFeatureExtractionLayer"];
}

void aiSaac::AiSaacSettings::setSummarizationFaceAgePrototxtPath(std::string filePath) {
    this->settings["aiSaacSettings"]["VideoSummarization"]["summarizationFaceAgePrototxtPath"]
        = filePath;
}
std::string aiSaac::AiSaacSettings::getSummarizationFaceAgePrototxtPath() {
    return this->settings["aiSaacSettings"]["VideoSummarization"]["summarizationFaceAgePrototxtPath"];
}

void aiSaac::AiSaacSettings::setSummarizationFaceAgeCaffemodelPath(std::string filePath) {
    this->settings["aiSaacSettings"]["VideoSummarization"]["summarizationFaceAgeCaffemodelPath"]
        = filePath;
}
std::string aiSaac::AiSaacSettings::getSummarizationFaceAgeCaffemodelPath() {
    return this->settings["aiSaacSettings"]["VideoSummarization"]["summarizationFaceAgeCaffemodelPath"];
}

void aiSaac::AiSaacSettings::setSummarizationFaceAgeMeanFilePath(std::string filePath) {
    this->settings["aiSaacSettings"]["VideoSummarization"]["summarizationFaceAgeMeanFilePath"]
        = filePath;
}
std::string aiSaac::AiSaacSettings::getSummarizationFaceAgeMeanFilePath() {
    return this->settings["aiSaacSettings"]["VideoSummarization"]["summarizationFaceAgeMeanFilePath"];
}

void aiSaac::AiSaacSettings::setSummarizationFaceAgeLabelFilePath(std::string filePath) {
    this->settings["aiSaacSettings"]["VideoSummarization"]["summarizationFaceAgeLabelFilePath"]
        = filePath;
}
std::string aiSaac::AiSaacSettings::getSummarizationFaceAgeLabelFilePath() {
    return this->settings["aiSaacSettings"]["VideoSummarization"]["summarizationFaceAgeLabelFilePath"];
}

void aiSaac::AiSaacSettings::setSummarizationFaceAgeFeatureExtractionLayer(std::string filePath) {
    this->settings["aiSaacSettings"]
        ["VideoSummarization"]
        ["summarizationFaceAgeFeatureExtractionLayer"] = filePath;
}
std::string aiSaac::AiSaacSettings::getSummarizationFaceAgeFeatureExtractionLayer() {
    return this->settings["aiSaacSettings"]
        ["VideoSummarization"]
        ["summarizationFaceAgeFeatureExtractionLayer"];
}

void aiSaac::AiSaacSettings::setSummarizationFaceRecPrototxtPath(std::string filePath) {
    this->settings["aiSaacSettings"]["VideoSummarization"]["summarizationFaceRecPrototxtPath"]
        = filePath;
}
std::string aiSaac::AiSaacSettings::getSummarizationFaceRecPrototxtPath() {
    return this->settings["aiSaacSettings"]["VideoSummarization"]["summarizationFaceRecPrototxtPath"];
}

void aiSaac::AiSaacSettings::setSummarizationFaceRecCaffemodelPath(std::string filePath) {
    this->settings["aiSaacSettings"]["VideoSummarization"]["summarizationFaceRecCaffemodelPath"]
        = filePath;
}
std::string aiSaac::AiSaacSettings::getSummarizationFaceRecCaffemodelPath() {
    return this->settings["aiSaacSettings"]["VideoSummarization"]["summarizationFaceRecCaffemodelPath"];
}

void aiSaac::AiSaacSettings::setSummarizationFaceRecMeanFilePath(std::string filePath) {
    this->settings["aiSaacSettings"]["VideoSummarization"]["summarizationFaceRecMeanFilePath"]
        = filePath;
}
std::string aiSaac::AiSaacSettings::getSummarizationFaceRecMeanFilePath() {
    return this->settings["aiSaacSettings"]["VideoSummarization"]["summarizationFaceRecMeanFilePath"];
}

void aiSaac::AiSaacSettings::setSummarizationFaceRecLabelFilePath(std::string filePath) {
    this->settings["aiSaacSettings"]["VideoSummarization"]["summarizationFaceRecLabelFilePath"]
        = filePath;
}
std::string aiSaac::AiSaacSettings::getSummarizationFaceRecLabelFilePath() {
    return this->settings["aiSaacSettings"]["VideoSummarization"]["summarizationFaceRecLabelFilePath"];
}

void aiSaac::AiSaacSettings::setSummarizationFaceRecFeatureExtractionLayer(std::string filePath) {
    this->settings["aiSaacSettings"]
        ["VideoSummarization"]
        ["summarizationFaceRecFeatureExtractionLayer"] = filePath;
}
std::string aiSaac::AiSaacSettings::getSummarizationFaceRecFeatureExtractionLayer() {
    return this->settings["aiSaacSettings"]
        ["VideoSummarization"]
        ["summarizationFaceRecFeatureExtractionLayer"];
}

//

void aiSaac::AiSaacSettings::setBackgroundSubtractionAlgo(std::string algo) {
    this->settings["aiSaacSettings"]["BgSubAnalytics"]["backgroundSubtractionAlgo"] = algo;
    this->saveSettings();
}

std::string aiSaac::AiSaacSettings::getBackgroundSubtractionAlgo() {
    return this->settings["aiSaacSettings"]["BgSubAnalytics"]["backgroundSubtractionAlgo"];
}

void aiSaac::AiSaacSettings::setBackgroundUpdateTime(int frames) {
    this->settings["aiSaacSettings"]["BgSubAnalytics"]["backgroundUpdateTime"] = frames;
    this->saveSettings();
}

int aiSaac::AiSaacSettings::getBackgroundUpdateTime() {
    return this->settings["aiSaacSettings"]["BgSubAnalytics"]["backgroundUpdateTime"];
}

void aiSaac::AiSaacSettings::setNoMotionBackgroundUpdateTime(int frames) {
    this->settings["aiSaacSettings"]["BgSubAnalytics"]["noMotionBackgroundUpdateTime"] = frames;
    this->saveSettings();
}

int aiSaac::AiSaacSettings::getNoMotionBackgroundUpdateTime() {
    return this->settings["aiSaacSettings"]["BgSubAnalytics"]["noMotionBackgroundUpdateTime"];
}

void aiSaac::AiSaacSettings::setBlobDeleteTime(int frames) {
    this->settings["aiSaacSettings"]["BgSubAnalytics"]["blobDeleteTime"] = frames;
    this->saveSettings();
}

int aiSaac::AiSaacSettings::getBlobDeleteTime() {
    return this->settings["aiSaacSettings"]["BgSubAnalytics"]["blobDeleteTime"];
}

void aiSaac::AiSaacSettings::setMinimumBlobFrames(int frames) {
    this->settings["aiSaacSettings"]["BgSubAnalytics"]["minimumBlobFrames"] = frames;
    saveSettings();
}

int aiSaac::AiSaacSettings::getMinimumBlobFrames() {
    return this->settings["aiSaacSettings"]["BgSubAnalytics"]["minimumBlobFrames"];
}

void aiSaac::AiSaacSettings::setMinimumWindowFraction(double value) {
    this->settings["aiSaacSettings"]["BgSubAnalytics"]["minimumWindowFraction"] = value;
    saveSettings();
}

double aiSaac::AiSaacSettings::getMinimumWindowFraction() {
    return this->settings["aiSaacSettings"]["BgSubAnalytics"]["minimumWindowFraction"];
}

void aiSaac::AiSaacSettings::setMaximumWindowFraction(double value) {
    this->settings["aiSaacSettings"]["BgSubAnalytics"]["maximumWindowFraction"] = value;
    saveSettings();
}

double aiSaac::AiSaacSettings::getMaximumWindowFraction() {
    return this->settings["aiSaacSettings"]["BgSubAnalytics"]["maximumWindowFraction"];
}

void aiSaac::AiSaacSettings::setHeadAnalyticsAlgo(std::string algo) {
    this->settings["aiSaacSettings"]["HeadAnalytics"]["headAnalyticsAlgo"] = algo;
    saveSettings();
}

std::string aiSaac::AiSaacSettings::getHeadAnalyticsAlgo() {
    return this->settings["aiSaacSettings"]["HeadAnalytics"]["headAnalyticsAlgo"];
}

void aiSaac::AiSaacSettings::setMinimumHeadSize(double x, double y) {
    this->settings["aiSaacSettings"]["HeadAnalytics"]["minimumHeadSize"] = {x, y};
    saveSettings();
}

std::vector<double> aiSaac::AiSaacSettings::getMinimumHeadSize() {
    return this->settings["aiSaacSettings"]["HeadAnalytics"]["minimumHeadSize"];
}

void aiSaac::AiSaacSettings::setMaximumHeadSize(double x, double y) {
    this->settings["aiSaacSettings"]["HeadAnalytics"]["maximumHeadSize"] = {x, y};
    saveSettings();
}

std::vector<double> aiSaac::AiSaacSettings::getMaximumHeadSize() {
    return this->settings["aiSaacSettings"]["HeadAnalytics"]["maximumHeadSize"];
}

double aiSaac::AiSaacSettings::getHeadScaleFactor() {
    return this->settings["aiSaacSettings"]["HeadAnalytics"]["headScaleFactor"];
}

int aiSaac::AiSaacSettings::getHeadMinNeighbours() {
    return this->settings["aiSaacSettings"]["HeadAnalytics"]["headMinNeighbours"];
}

void aiSaac::AiSaacSettings::setHeadScaleFactor(double val) {
    this->settings["aiSaacSettings"]["HeadAnalytics"]["headScaleFactor"] = val;
    saveSettings();
}

void aiSaac::AiSaacSettings::setHeadMinNeighbours(int val) {
    this->settings["aiSaacSettings"]["HeadAnalytics"]["headMinNeighbours"] = val;
    saveSettings();
}


void aiSaac::AiSaacSettings::setHaarCascadeFilePath(std::string filePath) {
    this->settings["aiSaacSettings"]["haarCascadeFilePath"] = filePath;
    saveSettings();
}

std::string aiSaac::AiSaacSettings::getHaarCascadeFilePath() {
    return this->settings["aiSaacSettings"]["haarCascadeFilePath"];
}

void aiSaac::AiSaacSettings::setHeadBlobDeleteTime(int frames) {
    this->settings["aiSaacSettings"]["HeadAnalytics"]["headBlobDeleteTime"] = frames;
    this->saveSettings();
}

int aiSaac::AiSaacSettings::getHeadBlobDeleteTime() {
    return this->settings["aiSaacSettings"]["HeadAnalytics"]["headBlobDeleteTime"];
}

void aiSaac::AiSaacSettings::setMinimumHeadBlobFrames(int frames) {
    this->settings["aiSaacSettings"]["HeadAnalytics"]["minimumHeadBlobFrames"] = frames;
    saveSettings();
}

int aiSaac::AiSaacSettings::getMinimumHeadBlobFrames() {
    return this->settings["aiSaacSettings"]["HeadAnalytics"]["minimumHeadBlobFrames"];
}

void aiSaac::AiSaacSettings::setHumanDetectAnalyticsAlgo(std::string algo) {
    this->settings["aiSaacSettings"]["HumanAnalytics"]["humanDetectAnalyticsAlgo"] = algo;
    saveSettings();
}

std::string aiSaac::AiSaacSettings::getHumanDetectAnalyticsAlgo() {
    return this->settings["aiSaacSettings"]["HumanAnalytics"]["humanDetectAnalyticsAlgo"];
}

int aiSaac::AiSaacSettings::getshirtTopYFactor() {
    return this->settings["aiSaacSettings"]["HumanAnalytics"]["shirtTopYFactor"];
}

int aiSaac::AiSaacSettings::getshirtBottomYFactor() {
    return this->settings["aiSaacSettings"]["HumanAnalytics"]["shirtBottomYFactor"];
}

void aiSaac::AiSaacSettings::setHumanBlobDeleteTime(int frames) {
    this->settings["aiSaacSettings"]["HumanAnalytics"]["humanBlobDeleteTime"] = frames;
    this->saveSettings();
}

int aiSaac::AiSaacSettings::getHumanBlobDeleteTime() {
    return this->settings["aiSaacSettings"]["HumanAnalytics"]["humanBlobDeleteTime"];
}

void aiSaac::AiSaacSettings::setMinimumHumanBlobFrames(int frames) {
    this->settings["aiSaacSettings"]["HumanAnalytics"]["minimumHumanBlobFrames"] = frames;
    saveSettings();
}

int aiSaac::AiSaacSettings::getMinimumHumanBlobFrames() {
    return this->settings["aiSaacSettings"]["HumanAnalytics"]["minimumHumanBlobFrames"];
}

void aiSaac::AiSaacSettings::setDPMPedestrianModelFilePath(std::string filePath) {
    this->settings["aiSaacSettings"]["HumanAnalytics"]["dpmPedestrianModelFilePath"]
        = filePath;
    saveSettings();
}
std::string aiSaac::AiSaacSettings::getDPMPedestrianModelFilePath() {
    return this->settings["aiSaacSettings"]["HumanAnalytics"]["dpmPedestrianModelFilePath"];
}

int aiSaac::AiSaacSettings::getDPM_interval() {
    return this->settings["aiSaacSettings"]["HumanAnalytics"]["dpmInterval"];
}

int aiSaac::AiSaacSettings::getDPM_min_neighbours() {
    return this->settings["aiSaacSettings"]["HumanAnalytics"]["dpmMinNeighbours"];
}

int aiSaac::AiSaacSettings::getDPM_flags() {
    return this->settings["aiSaacSettings"]["HumanAnalytics"]["dpmFlags"];
}

float aiSaac::AiSaacSettings::getDPM_threshold() {
    return this->settings["aiSaacSettings"]["HumanAnalytics"]["dpmThreshold"];
}

void aiSaac::AiSaacSettings::setDPM_interval(int val) {
    this->settings["aiSaacSettings"]["HumanAnalytics"]["dpmInterval"] = val;
    saveSettings();
}

void aiSaac::AiSaacSettings::setDPM_min_neighbours(int val) {
    this->settings["aiSaacSettings"]["HumanAnalytics"]["dpmMinNeighbours"] = val;
    saveSettings();
}

void aiSaac::AiSaacSettings::setDPM_flags(int val) {
    this->settings["aiSaacSettings"]["HumanAnalytics"]["dpmFlags"] = val;
    saveSettings();
}

void aiSaac::AiSaacSettings::setDPM_threshold(float val) {
    this->settings["aiSaacSettings"]["HumanAnalytics"]["dpmThreshold"] = val;
    saveSettings();
}

void aiSaac::AiSaacSettings::setICFPedestrianModelFilePath(std::string filePath) {
    this->settings["aiSaacSettings"]["HumanAnalytics"]["icfPedestrianModelFilePath"]
        = filePath;
    saveSettings();
}

std::string aiSaac::AiSaacSettings::getICFPedestrianModelFilePath() {
    return this->settings["aiSaacSettings"]["HumanAnalytics"]["icfPedestrianModelFilePath"];
}

int aiSaac::AiSaacSettings::getICF_interval() {
    return this->settings["aiSaacSettings"]["HumanAnalytics"]["icfInterval"];
}

int aiSaac::AiSaacSettings::getICF_min_neighbours() {
    return this->settings["aiSaacSettings"]["HumanAnalytics"]["icfMinNeighbours"];
}

int aiSaac::AiSaacSettings::getICF_flags() {
    return this->settings["aiSaacSettings"]["HumanAnalytics"]["icfFlags"];
}

float aiSaac::AiSaacSettings::getICF_threshold() {
    return this->settings["aiSaacSettings"]["HumanAnalytics"]["icfThreshold"];
}

int aiSaac::AiSaacSettings::getICF_step_through() {
    return this->settings["aiSaacSettings"]["HumanAnalytics"]["icfStepThrough"];
}

void aiSaac::AiSaacSettings::setICF_interval(int val) {
    this->settings["aiSaacSettings"]["HumanAnalytics"]["icfInterval"] = val;
    saveSettings();
}

void aiSaac::AiSaacSettings::setICF_min_neighbours(int val) {
    this->settings["aiSaacSettings"]["HumanAnalytics"]["icfMinNeighbours"] = val;
    saveSettings();
}

void aiSaac::AiSaacSettings::setICF_flags(int val) {
    this->settings["aiSaacSettings"]["HumanAnalytics"]["icfFlags"] = val;
    saveSettings();
}

void aiSaac::AiSaacSettings::setICF_step_through(int val) {
    this->settings["aiSaacSettings"]["HumanAnalytics"]["icfStepThrough"] = val;
    saveSettings();
}

void aiSaac::AiSaacSettings::setICF_threshold(float val) {
    this->settings["aiSaacSettings"]["HumanAnalytics"]["icfThreshold"] = val;
    saveSettings();
}



void aiSaac::AiSaacSettings::setFaceDetectAnalyticsAlgo(std::string algo) {
    this->settings["aiSaacSettings"]["FaceAnalytics"]["faceDetectAnalyticsAlgo"] = algo;
    saveSettings();
}

std::string aiSaac::AiSaacSettings::getFaceDetectAnalyticsAlgo() {
    return this->settings["aiSaacSettings"]["FaceAnalytics"]["faceDetectAnalyticsAlgo"];
}

void aiSaac::AiSaacSettings::setFaceBlobDeleteTime(int frames) {
    this->settings["aiSaacSettings"]["FaceAnalytics"]["faceBlobDeleteTime"] = frames;
    this->saveSettings();
}

int aiSaac::AiSaacSettings::getFaceBlobDeleteTime() {
    return this->settings["aiSaacSettings"]["FaceAnalytics"]["faceBlobDeleteTime"];
}

void aiSaac::AiSaacSettings::setMinimumFaceBlobFrames(int frames) {
    this->settings["aiSaacSettings"]["FaceAnalytics"]["minimumFaceBlobFrames"] = frames;
    saveSettings();
}

int aiSaac::AiSaacSettings::getMinimumFaceBlobFrames() {
    return this->settings["aiSaacSettings"]["FaceAnalytics"]["minimumFaceBlobFrames"];
}

void aiSaac::AiSaacSettings::setMinimumFaceSize(double x, double y) {
    this->settings["aiSaacSettings"]["FaceAnalytics"]["minimumFaceSize"] = {x, y};
    saveSettings();
}

std::vector<double> aiSaac::AiSaacSettings::getMinimumFaceSize() {
    return this->settings["aiSaacSettings"]["FaceAnalytics"]["minimumFaceSize"];
}

void aiSaac::AiSaacSettings::setMaximumFaceSize(double x, double y) {
    this->settings["aiSaacSettings"]["FaceAnalytics"]["maximumFaceSize"] = {x, y};
    saveSettings();
}

std::vector<double> aiSaac::AiSaacSettings::getMaximumFaceSize() {
    return this->settings["aiSaacSettings"]["FaceAnalytics"]["maximumFaceSize"];
}

void aiSaac::AiSaacSettings::setPositionDistanceFactor(int factor) {
    this->settings["aiSaacSettings"]["FaceAnalytics"]["positionDistanceFactor"] = factor;
    saveSettings();
}

int aiSaac::AiSaacSettings::getPositionDistanceFactor() {
    return this->settings["aiSaacSettings"]["FaceAnalytics"]["positionDistanceFactor"];
}

void aiSaac::AiSaacSettings::setFeatureExtractionLayer(std::string layer) {
    this->settings["aiSaacSettings"]["FaceAnalytics"]["featureExtractionLayer"] = layer;
    saveSettings();
}

std::string aiSaac::AiSaacSettings::getFeatureExtractionLayer() {
    return this->settings["aiSaacSettings"]["FaceAnalytics"]["featureExtractionLayer"];
}

void aiSaac::AiSaacSettings::setNPDFaceDetectFilePath(std::string filePath) {
    this->settings["aiSaacSettings"]["FaceAnalytics"]["NPDFaceDetectFilePath"] = filePath;
    saveSettings();
}

std::string aiSaac::AiSaacSettings::getNPDFaceDetectFilePath() {
    return this->settings["aiSaacSettings"]["FaceAnalytics"]["NPDFaceDetectFilePath"];
}
//
void aiSaac::AiSaacSettings::setDarknetFaceDetectConfigFilePath(std::string filePath) {
    this->settings["aiSaacSettings"]["FaceAnalytics"]["DarknetFaceDetectConfigFilePath"] = filePath;
    saveSettings();
}

std::string aiSaac::AiSaacSettings::getDarknetFaceDetectConfigFilePath() {
    return this->settings["aiSaacSettings"]["FaceAnalytics"]["DarknetFaceDetectConfigFilePath"];
}

void aiSaac::AiSaacSettings::setDarknetFaceDetectWeightFilePath(std::string filePath) {
    this->settings["aiSaacSettings"]["FaceAnalytics"]["DarknetFaceDetectWeightFilePath"] = filePath;
    saveSettings();
}

std::string aiSaac::AiSaacSettings::getDarknetFaceDetectWeightFilePath() {
    return this->settings["aiSaacSettings"]["FaceAnalytics"]["DarknetFaceDetectWeightFilePath"];
}

void aiSaac::AiSaacSettings::setDarknetFaceDetectLabelFilePath(std::string filePath) {
    this->settings["aiSaacSettings"]["FaceAnalytics"]["DarknetFaceDetectLabelFilePath"] = filePath;
    saveSettings();
}

std::string aiSaac::AiSaacSettings::getDarknetFaceDetectLabelFilePath() {
    return this->settings["aiSaacSettings"]["FaceAnalytics"]["DarknetFaceDetectLabelFilePath"];
}

void aiSaac::AiSaacSettings::setDarknetFaceDetectConfidenceThreshold(double thres) {
    this->settings["aiSaacSettings"]["FaceAnalytics"]["DarknetFaceDetectConfidenceThreshold"] = thres;
    saveSettings();
}

double aiSaac::AiSaacSettings::getDarknetFaceDetectConfidenceThreshold() {
    return this->settings["aiSaacSettings"]["FaceAnalytics"]["DarknetFaceDetectConfidenceThreshold"];
}
//
void aiSaac::AiSaacSettings::setFaceStorageMode(bool mode) {
    this->settings["aiSaacSettings"]["FaceAnalytics"]["faceStorageMode"] = mode;
    saveSettings();
}

bool aiSaac::AiSaacSettings::getFaceStorageMode() {
    return this->settings["aiSaacSettings"]["FaceAnalytics"]["faceStorageMode"];
}

void aiSaac::AiSaacSettings::setFaceRecognitionTrainingDataPath(std::string folderPath) {
    this->settings["aiSaacSettings"]["FaceAnalytics"]["faceRecognitionTrainingDataPath"] = folderPath;
    saveSettings();
}

std::string aiSaac::AiSaacSettings::getFaceRecognitionTrainingDataPath() {
    return this->settings["aiSaacSettings"]["FaceAnalytics"]["faceRecognitionTrainingDataPath"];
}

void aiSaac::AiSaacSettings::setVGGFacePrototxtPath(std::string filePath) {
    this->settings["aiSaacSettings"]["FaceAnalytics"]["VGGFacePrototxtPath"] = filePath;
    saveSettings();
}

std::string aiSaac::AiSaacSettings::getVGGFacePrototxtPath() {
    return this->settings["aiSaacSettings"]["FaceAnalytics"]["VGGFacePrototxtPath"];
}

void aiSaac::AiSaacSettings::setVGGFaceCaffemodelPath(std::string filePath) {
    this->settings["aiSaacSettings"]["FaceAnalytics"]["VGGFaceCaffemodelPath"] = filePath;
    saveSettings();
}

std::string aiSaac::AiSaacSettings::getVGGFaceCaffemodelPath() {
    return this->settings["aiSaacSettings"]["FaceAnalytics"]["VGGFaceCaffemodelPath"];
}

void aiSaac::AiSaacSettings::setFaceRecognitionDBPath(std::string filePath) {
    this->settings["aiSaacSettings"]["FaceAnalytics"]["faceRecognitionDBPath"] = filePath;
    saveSettings();
}

std::string aiSaac::AiSaacSettings::getFaceRecognitionDBPath() {
    return this->settings["aiSaacSettings"]["FaceAnalytics"]["faceRecognitionDBPath"];
}

void aiSaac::AiSaacSettings::setFaceRecognitionModelAlgo(std::string algo) {
    this->settings["aiSaacSettings"]["FaceAnalytics"]["faceRecognitionModelAlgo"] = algo;
    saveSettings();
}

std::string aiSaac::AiSaacSettings::getFaceRecognitionModelAlgo() {
    return this->settings["aiSaacSettings"]["FaceAnalytics"]["faceRecognitionModelAlgo"];
}

void aiSaac::AiSaacSettings::setSimilarityThreshold(double k) {
    this->settings["aiSaacSettings"]["FaceAnalytics"]["similarityThreshold"] = k;
    this->saveSettings();
}

double aiSaac::AiSaacSettings::getSimilarityThreshold() {
    return this->settings["aiSaacSettings"]["FaceAnalytics"]["similarityThreshold"];
}

void aiSaac::AiSaacSettings::setKnnThreshold(int k) {
    this->settings["aiSaacSettings"]["FaceAnalytics"]["knnThreshold"] = k;
    this->saveSettings();
}

int aiSaac::AiSaacSettings::getKnnThreshold() {
    return this->settings["aiSaacSettings"]["FaceAnalytics"]["knnThreshold"];
}

void aiSaac::AiSaacSettings::setGenderPrototxtPath(std::string filePath) {
    this->settings["aiSaacSettings"]["FaceAnalytics"]["genderPrototxtPath"] = filePath;
    saveSettings();
}

std::string aiSaac::AiSaacSettings::getGenderPrototxtPath() {
    return this->settings["aiSaacSettings"]["FaceAnalytics"]["genderPrototxtPath"];
}

void aiSaac::AiSaacSettings::setGenderCaffemodelPath(std::string filePath) {
    this->settings["aiSaacSettings"]["FaceAnalytics"]["genderCaffemodelPath"] = filePath;
    saveSettings();
}

std::string aiSaac::AiSaacSettings::getGenderCaffemodelPath() {
    return this->settings["aiSaacSettings"]["FaceAnalytics"]["genderCaffemodelPath"];
}

void aiSaac::AiSaacSettings::setGenderMeanFilePath(std::string filePath) {
    this->settings["aiSaacSettings"]["FaceAnalytics"]["genderMeanFilePath"] = filePath;
    saveSettings();
}

std::string aiSaac::AiSaacSettings::getGenderMeanFilePath() {
    return this->settings["aiSaacSettings"]["FaceAnalytics"]["genderMeanFilePath"];
}

void aiSaac::AiSaacSettings::setGenderLabelFilePath(std::string filePath) {
    this->settings["aiSaacSettings"]["FaceAnalytics"]["genderLabelFilePath"] = filePath;
    saveSettings();
}

std::string aiSaac::AiSaacSettings::getGenderLabelFilePath() {
    return this->settings["aiSaacSettings"]["FaceAnalytics"]["genderLabelFilePath"];
}

void aiSaac::AiSaacSettings::setGenderRecognitionThreshold(double threshold) {
    this->settings["aiSaacSettings"]["FaceAnalytics"]["genderRecognitionThreshold"] = threshold;
    saveSettings();
}

double aiSaac::AiSaacSettings::getGenderRecognitionThreshold() {
    return this->settings["aiSaacSettings"]["FaceAnalytics"]["genderRecognitionThreshold"];
}



void aiSaac::AiSaacSettings::setAgePrototxtPath(std::string filePath) {
    this->settings["aiSaacSettings"]["FaceAnalytics"]["agePrototxtPath"] = filePath;
    saveSettings();
}

std::string aiSaac::AiSaacSettings::getAgePrototxtPath() {
    return this->settings["aiSaacSettings"]["FaceAnalytics"]["agePrototxtPath"];
}

void aiSaac::AiSaacSettings::setAgeCaffemodelPath(std::string filePath) {
    this->settings["aiSaacSettings"]["FaceAnalytics"]["ageCaffemodelPath"] = filePath;
    saveSettings();
}

std::string aiSaac::AiSaacSettings::getAgeCaffemodelPath() {
    return this->settings["aiSaacSettings"]["FaceAnalytics"]["ageCaffemodelPath"];
}

void aiSaac::AiSaacSettings::setAgeMeanFilePath(std::string filePath) {
    this->settings["aiSaacSettings"]["FaceAnalytics"]["ageMeanFilePath"] = filePath;
    saveSettings();
}

std::string aiSaac::AiSaacSettings::getAgeMeanFilePath() {
    return this->settings["aiSaacSettings"]["FaceAnalytics"]["ageMeanFilePath"];
}

void aiSaac::AiSaacSettings::setAgeLabelFilePath(std::string filePath) {
    this->settings["aiSaacSettings"]["FaceAnalytics"]["ageLabelFilePath"] = filePath;
    saveSettings();
}

std::string aiSaac::AiSaacSettings::getAgeLabelFilePath() {
    return this->settings["aiSaacSettings"]["FaceAnalytics"]["ageLabelFilePath"];
}

void aiSaac::AiSaacSettings::setAgeRecognitionThreshold(double threshold) {
    this->settings["aiSaacSettings"]["FaceAnalytics"]["ageRecognitionThreshold"] = threshold;
    saveSettings();
}

double aiSaac::AiSaacSettings::getAgeRecognitionThreshold() {
    return this->settings["aiSaacSettings"]["FaceAnalytics"]["ageRecognitionThreshold"];
}

void aiSaac::AiSaacSettings::setEmotionPrototxtPath(std::string filePath) {
    this->settings["aiSaacSettings"]["FaceAnalytics"]["emotionPrototxtPath"] = filePath;
    saveSettings();
}

std::string aiSaac::AiSaacSettings::getEmotionPrototxtPath() {
    return this->settings["aiSaacSettings"]["FaceAnalytics"]["emotionPrototxtPath"];
}

void aiSaac::AiSaacSettings::setEmotionCaffemodelPath(std::string filePath) {
    this->settings["aiSaacSettings"]["FaceAnalytics"]["emotionCaffemodelPath"] = filePath;
    saveSettings();
}

std::string aiSaac::AiSaacSettings::getEmotionCaffemodelPath() {
    return this->settings["aiSaacSettings"]["FaceAnalytics"]["emotionCaffemodelPath"];
}

void aiSaac::AiSaacSettings::setEmotionMeanFilePath(std::string filePath) {
    this->settings["aiSaacSettings"]["FaceAnalytics"]["emotionMeanFilePath"] = filePath;
    saveSettings();
}

std::string aiSaac::AiSaacSettings::getEmotionMeanFilePath() {
    return this->settings["aiSaacSettings"]["FaceAnalytics"]["emotionMeanFilePath"];
}

void aiSaac::AiSaacSettings::setEmotionLabelFilePath(std::string filePath) {
    this->settings["aiSaacSettings"]["FaceAnalytics"]["emotionLabelFilePath"] = filePath;
    saveSettings();
}

std::string aiSaac::AiSaacSettings::getEmotionLabelFilePath() {
    return this->settings["aiSaacSettings"]["FaceAnalytics"]["emotionLabelFilePath"];
}

void aiSaac::AiSaacSettings::setObjectAnalyticsAlgo(std::string algo) {
    this->settings["aiSaacSettings"]["ObjectAnalytics"]["objectAnalyticsAlgo"] = algo;
    this->saveSettings();
}

std::string aiSaac::AiSaacSettings::getObjectAnalyticsAlgo() {
    return this->settings["aiSaacSettings"]["ObjectAnalytics"]["objectAnalyticsAlgo"];
}

void aiSaac::AiSaacSettings::setObjectBlobDeleteTime(int frames) {
    this->settings["aiSaacSettings"]["ObjectAnalytics"]["objectBlobDeleteTime"] = frames;
    this->saveSettings();
}

int aiSaac::AiSaacSettings::getObjectBlobDeleteTime() {
    return this->settings["aiSaacSettings"]["ObjectAnalytics"]["objectBlobDeleteTime"];
}

void aiSaac::AiSaacSettings::setMinimumObjectBlobFrames(int frames) {
    this->settings["aiSaacSettings"]["ObjectAnalytics"]["minimumObjectBlobFrames"] = frames;
    saveSettings();
}

int aiSaac::AiSaacSettings::getMinimumObjectBlobFrames() {
    return this->settings["aiSaacSettings"]["ObjectAnalytics"]["minimumObjectBlobFrames"];
}

void aiSaac::AiSaacSettings::setObjectCaffePrototxtPath(std::string filePath) {
    this->settings["aiSaacSettings"]["ObjectAnalytics"]["objectCaffePrototxtPath"] = filePath;
    this->saveSettings();
}

std::string aiSaac::AiSaacSettings::getObjectCaffePrototxtPath() {
    return this->settings["aiSaacSettings"]["ObjectAnalytics"]["objectCaffePrototxtPath"];
}

void aiSaac::AiSaacSettings::setObjectCaffeCaffemodelPath(std::string filePath) {
    this->settings["aiSaacSettings"]["ObjectAnalytics"]["objectCaffeCaffemodelPath"] = filePath;
    this->saveSettings();
}

std::string aiSaac::AiSaacSettings::getObjectCaffeCaffemodelPath() {
    return this->settings["aiSaacSettings"]["ObjectAnalytics"]["objectCaffeCaffemodelPath"];
}

void aiSaac::AiSaacSettings::setObjectCaffeMeanFilePath(std::string filePath) {
    this->settings["aiSaacSettings"]["ObjectAnalytics"]["objectCaffeMeanFilePath"] = filePath;
    this->saveSettings();
}

std::string aiSaac::AiSaacSettings::getObjectCaffeMeanFilePath() {
    return this->settings["aiSaacSettings"]["ObjectAnalytics"]["objectCaffeMeanFilePath"];
}

void aiSaac::AiSaacSettings::setObjectCaffeLabelFilePath(std::string filePath) {
    this->settings["aiSaacSettings"]["ObjectAnalytics"]["objectCaffeLabelFilePath"] = filePath;
    this->saveSettings();
}

std::string aiSaac::AiSaacSettings::getObjectCaffeLabelFilePath() {
    return this->settings["aiSaacSettings"]["ObjectAnalytics"]["objectCaffeLabelFilePath"];
}

void aiSaac::AiSaacSettings::setObjectDarknetConfigFilePath(std::string filePath) {
    this->settings["aiSaacSettings"]["ObjectAnalytics"]["objectDarknetConfigFilePath"] = filePath;
    this->saveSettings();
}

std::string aiSaac::AiSaacSettings::getObjectDarknetConfigFilePath() {
    return this->settings["aiSaacSettings"]["ObjectAnalytics"]["objectDarknetConfigFilePath"];
}

void aiSaac::AiSaacSettings::setObjectDarknetWeightFilePath(std::string filePath) {
    this->settings["aiSaacSettings"]["ObjectAnalytics"]["objectDarknetWeightFilePath"] = filePath;
    this->saveSettings();
}

std::string aiSaac::AiSaacSettings::getObjectDarknetWeightFilePath() {
    return this->settings["aiSaacSettings"]["ObjectAnalytics"]["objectDarknetWeightFilePath"];
}

void aiSaac::AiSaacSettings::setObjectDarknetLabelFilePath(std::string filePath) {
    this->settings["aiSaacSettings"]["ObjectAnalytics"]["objectDarknetLabelFilePath"] = filePath;
    this->saveSettings();
}

std::string aiSaac::AiSaacSettings::getObjectDarknetLabelFilePath() {
    return this->settings["aiSaacSettings"]["ObjectAnalytics"]["objectDarknetLabelFilePath"];
}

void aiSaac::AiSaacSettings::setObjectDNNPrototxtPath(std::string filePath) {
    this->settings["aiSaacSettings"]["ObjectAnalytics"]["objectDNNPrototxtPath"] = filePath;
    this->saveSettings();
}

std::string aiSaac::AiSaacSettings::getObjectDNNPrototxtPath() {
    return this->settings["aiSaacSettings"]["ObjectAnalytics"]["objectDNNPrototxtPath"];
}

void aiSaac::AiSaacSettings::setObjectDNNCaffemodelPath(std::string filePath) {
    this->settings["aiSaacSettings"]["ObjectAnalytics"]["objectDNNCaffemodelPath"] = filePath;
    this->saveSettings();
}

std::string aiSaac::AiSaacSettings::getObjectDNNCaffemodelPath() {
    return this->settings["aiSaacSettings"]["ObjectAnalytics"]["objectDNNCaffemodelPath"];
}

void aiSaac::AiSaacSettings::setObjectDNNMeanFilePath(std::string filePath) {
    this->settings["aiSaacSettings"]["ObjectAnalytics"]["objectDNNMeanFilePath"] = filePath;
    this->saveSettings();
}

std::string aiSaac::AiSaacSettings::getObjectDNNMeanFilePath() {
    return this->settings["aiSaacSettings"]["ObjectAnalytics"]["objectDNNMeanFilePath"];
}

void aiSaac::AiSaacSettings::setObjectDNNLabelFilePath(std::string filePath) {
    this->settings["aiSaacSettings"]["ObjectAnalytics"]["objectDNNLabelFilePath"] = filePath;
    this->saveSettings();
}

std::string aiSaac::AiSaacSettings::getObjectDNNLabelFilePath() {
    return this->settings["aiSaacSettings"]["ObjectAnalytics"]["objectDNNLabelFilePath"];
}

void aiSaac::AiSaacSettings::setObjectAnalyticsConfidenceThreshold(float confidence) {
    this->settings["aiSaacSettings"]["ObjectAnalytics"]["objectAnalyticsConfidenceThreshold"] =
        confidence;
    this->saveSettings();
}

float aiSaac::AiSaacSettings::getObjectAnalyticsConfidenceThreshold() {
    return
        this->settings["aiSaacSettings"]["ObjectAnalytics"]["objectAnalyticsConfidenceThreshold"];
}

void aiSaac::AiSaacSettings::setSceneAnalyticsAlgo(std::string algo) {
    this->settings["aiSaacSettings"]["SceneAnalytics"]["sceneAnalyticsAlgo"] = algo;
    this->saveSettings();
}

std::string aiSaac::AiSaacSettings::getSceneAnalyticsAlgo() {
    return this->settings["aiSaacSettings"]["SceneAnalytics"]["sceneAnalyticsAlgo"];
}

void aiSaac::AiSaacSettings::setScenePrototxtPath(std::string filePath) {
    this->settings["aiSaacSettings"]["SceneAnalytics"]["scenePrototxtPath"] = filePath;
    this->saveSettings();
}

std::string aiSaac::AiSaacSettings::getScenePrototxtPath() {
    return this->settings["aiSaacSettings"]["SceneAnalytics"]["scenePrototxtPath"];
}

void aiSaac::AiSaacSettings::setSceneCaffemodelPath(std::string filePath) {
    this->settings["aiSaacSettings"]["SceneAnalytics"]["sceneCaffemodelPath"] = filePath;
    this->saveSettings();
}

std::string aiSaac::AiSaacSettings::getSceneCaffemodelPath() {
    return this->settings["aiSaacSettings"]["SceneAnalytics"]["sceneCaffemodelPath"];
}

void aiSaac::AiSaacSettings::setSceneMeanFilePath(std::string filePath) {
    this->settings["aiSaacSettings"]["SceneAnalytics"]["sceneMeanFilePath"] = filePath;
    this->saveSettings();
}

std::string aiSaac::AiSaacSettings::getSceneMeanFilePath() {
    return this->settings["aiSaacSettings"]["SceneAnalytics"]["sceneMeanFilePath"];
}

void aiSaac::AiSaacSettings::setSceneLabelFilePath(std::string filePath) {
    this->settings["aiSaacSettings"]["SceneAnalytics"]["sceneLabelFilePath"] = filePath;
    this->saveSettings();
}

std::string aiSaac::AiSaacSettings::getSceneLabelFilePath() {
    return this->settings["aiSaacSettings"]["SceneAnalytics"]["sceneLabelFilePath"];
}

void aiSaac::AiSaacSettings::setSceneAnalyticsConfidenceThreshold(float confidence) {
    this->settings["aiSaacSettings"]["SceneAnalytics"]["sceneAnalyticsConfidenceThreshold"] =
        confidence;
    this->saveSettings();
}

float aiSaac::AiSaacSettings::getSceneAnalyticsConfidenceThreshold() {
    return this->settings["aiSaacSettings"]["SceneAnalytics"]["sceneAnalyticsConfidenceThreshold"];
}

void aiSaac::AiSaacSettings::setSceneBlobDeleteTime(int frames) {
    this->settings["aiSaacSettings"]["SceneAnalytics"]["sceneBlobDeleteTime"] = frames;
    this->saveSettings();
}

int aiSaac::AiSaacSettings::getSceneBlobDeleteTime() {
    return this->settings["aiSaacSettings"]["SceneAnalytics"]["sceneBlobDeleteTime"];
}

void aiSaac::AiSaacSettings::setMinimumSceneBlobFrames(int frames) {
    this->settings["aiSaacSettings"]["SceneAnalytics"]["minimumSceneBlobFrames"] = frames;
    saveSettings();
}

int aiSaac::AiSaacSettings::getMinimumSceneBlobFrames() {
    return this->settings["aiSaacSettings"]["SceneAnalytics"]["minimumSceneBlobFrames"];
}

void aiSaac::AiSaacSettings::setTextAnalyticsAlgo(std::string algo) {
    this->settings["aiSaacSettings"]["TextAnalytics"]["textAnalyticsAlgo"] = algo;
    saveSettings();
}

std::string aiSaac::AiSaacSettings::getTextAnalyticsAlgo() {
    return this->settings["aiSaacSettings"]["TextAnalytics"]["textAnalyticsAlgo"];
}

void aiSaac::AiSaacSettings::setTextBlobDeleteTime(int frames) {
    this->settings["aiSaacSettings"]["TextAnalytics"]["textBlobDeleteTime"] = frames;
    this->saveSettings();
}

int aiSaac::AiSaacSettings::getTextBlobDeleteTime() {
    return this->settings["aiSaacSettings"]["TextAnalytics"]["textBlobDeleteTime"];
}

void aiSaac::AiSaacSettings::setMinimumTextBlobFrames(int frames) {
    this->settings["aiSaacSettings"]["TextAnalytics"]["minimumTextBlobFrames"] = frames;
    saveSettings();
}

int aiSaac::AiSaacSettings::getMinimumTextBlobFrames() {
    return this->settings["aiSaacSettings"]["TextAnalytics"]["minimumTextBlobFrames"];
}

void aiSaac::AiSaacSettings::setTextRectDistanceThresh(int threshold) {
    this->settings["aiSaacSettings"]["TextAnalytics"]["textRectDistanceThresh"];
    saveSettings();
}

int aiSaac::AiSaacSettings::getTextRectDistanceThresh() {
    return this->settings["aiSaacSettings"]["TextAnalytics"]["textRectDistanceThresh"];
}

void aiSaac::AiSaacSettings::setVehicleAnalyticsAlgo(std::string algo) {
    this->settings["aiSaacSettings"]["VehicleAnalytics"]["vehicleAnalyticsAlgo"] = algo;
    saveSettings();
}

std::string aiSaac::AiSaacSettings::getVehicleAnalyticsAlgo() {
    return this->settings["aiSaacSettings"]["VehicleAnalytics"]["vehicleAnalyticsAlgo"];
}
void aiSaac::AiSaacSettings::setVehicleBlobDeleteTime(int frames) {
    this->settings["aiSaacSettings"]["VehicleAnalytics"]["vehicleBlobDeleteTime"] = frames;
    this->saveSettings();
}

int aiSaac::AiSaacSettings::getVehicleBlobDeleteTime() {
    return this->settings["aiSaacSettings"]["VehicleAnalytics"]["vehicleBlobDeleteTime"];
}

void aiSaac::AiSaacSettings::setMinimumVehicleBlobFrames(int frames) {
    this->settings["aiSaacSettings"]["VehicleAnalytics"]["minimumVehicleBlobFrames"] = frames;
    saveSettings();
}

int aiSaac::AiSaacSettings::getMinimumVehicleBlobFrames() {
    return this->settings["aiSaacSettings"]["VehicleAnalytics"]["minimumVehicleBlobFrames"];
}

void aiSaac::AiSaacSettings::setMinimumVehicleSize(int x, int y) {
    this->settings["aiSaacSettings"]["VehicleAnalytics"]["minimumVehicleSize"] = {x, y};
    saveSettings();
}

std::vector<int> aiSaac::AiSaacSettings::getMinimumVehicleSize() {
    return this->settings["aiSaacSettings"]["VehicleAnalytics"]["minimumVehicleSize"];
}

void aiSaac::AiSaacSettings::setMaximumVehicleSize(int x, int y) {
    this->settings["aiSaacSettings"]["VehicleAnalytics"]["maximumVehicleSize"] = {x, y};
    saveSettings();
}

std::vector<int> aiSaac::AiSaacSettings::getMaximumVehicleSize() {
    return this->settings["aiSaacSettings"]["VehicleAnalytics"]["maximumVehicleSize"];
}

void aiSaac::AiSaacSettings::setNSFWAnalyticsAlgo(std::string algo) {
    this->settings["aiSaacSettings"]["NSFWAnalytics"]["NSFWAnalyticsAlgo"] = algo;
    this->saveSettings();
}

std::string aiSaac::AiSaacSettings::getNSFWAnalyticsAlgo() {
    return this->settings["aiSaacSettings"]["NSFWAnalytics"]["NSFWAnalyticsAlgo"];
}

void aiSaac::AiSaacSettings::setNSFWCaffePrototxtPath(std::string filePath) {
    this->settings["aiSaacSettings"]["NSFWAnalytics"]["NSFWCaffePrototxtPath"] = filePath;
    this->saveSettings();
}

std::string aiSaac::AiSaacSettings::getNSFWCaffePrototxtPath() {
    return this->settings["aiSaacSettings"]["NSFWAnalytics"]["NSFWCaffePrototxtPath"];
}

void aiSaac::AiSaacSettings::setNSFWCaffeCaffemodelPath(std::string filePath) {
    this->settings["aiSaacSettings"]["NSFWAnalytics"]["NSFWCaffeCaffemodelPath"] = filePath;
    this->saveSettings();
}

std::string aiSaac::AiSaacSettings::getNSFWCaffeCaffemodelPath() {
    return this->settings["aiSaacSettings"]["NSFWAnalytics"]["NSFWCaffeCaffemodelPath"];
}

void aiSaac::AiSaacSettings::setNSFWCaffeLabelFilePath(std::string filePath) {
    this->settings["aiSaacSettings"]["NSFWAnalytics"]["NSFWCaffeLabelFilePath"] = filePath;
    this->saveSettings();
}

std::string aiSaac::AiSaacSettings::getNSFWCaffeLabelFilePath() {
    return this->settings["aiSaacSettings"]["NSFWAnalytics"]["NSFWCaffeLabelFilePath"];
}

void aiSaac::AiSaacSettings::setNSFWAnalyticsConfidenceThreshold(float confidence) {
    this->settings["aiSaacSettings"]["NSFWAnalytics"]["NSFWAnalyticsConfidenceThreshold"] =
        confidence;
    this->saveSettings();
}

float aiSaac::AiSaacSettings::getNSFWAnalyticsConfidenceThreshold() {
    return
        this->settings["aiSaacSettings"]["NSFWAnalytics"]["NSFWAnalyticsConfidenceThreshold"];
}

void aiSaac::AiSaacSettings::setProcFPS(int procFPS) {
    this->settings["aiSaacSettings"]["procFPS"] = procFPS;
    this->saveSettings();
}

int aiSaac::AiSaacSettings::getProcFPS() {
    return this->settings["aiSaacSettings"]["procFPS"];
}

void aiSaac::AiSaacSettings::loadSettings() {
    std::ifstream configFile;
    configFile.open(this->settingsFileName);
    configFile >> this->settings;
}

void aiSaac::AiSaacSettings::saveSettings() {
    std::cout << "saving file" << this->settingsFileName << std::endl;
    std::ofstream configFile;
    configFile.open(this->settingsFileName);
    configFile << this->settings.dump(4);
    configFile.close();
}

void aiSaac::AiSaacSettings::resetDefaultValues() {
    // codeReview(Anurag): change this to by under the SummarizationAnalytics object in JSON
    this->settings["aiSaacSettings"]["VideoSummarization"]["summarizationThreshold"] = 0.001;
    this->settings["aiSaacSettings"]["VideoSummarization"]["summarizationSNPSize"] = 3;
    this->settings["aiSaacSettings"]["VideoSummarization"]["summarizationStreamAlgo"] = "MMR";
    this->settings["aiSaacSettings"]["VideoSummarization"]["summarizationGreedyAlgo"] = "DM";

    this->settings["aiSaacSettings"]["BgSubAnalytics"]["backgroundSubtractionAlgo"] = "MLBGS";
    this->settings["aiSaacSettings"]["BgSubAnalytics"]["minimumWindowFraction"] = 0.01;
    this->settings["aiSaacSettings"]["BgSubAnalytics"]["maximumWindowFraction"] = 0.95;
    this->settings["aiSaacSettings"]["BgSubAnalytics"]["blobDeleteTime"] = 60;
    this->settings["aiSaacSettings"]["BgSubAnalytics"]["backgroundUpdateTime"] = 3000;
    this->settings["aiSaacSettings"]["BgSubAnalytics"]["noMotionBackgroundUpdateTime"] = 20;
    this->settings["aiSaacSettings"]["BgSubAnalytics"]["minimumBlobFrames"] = 20;

    this->settings["aiSaacSettings"]["HeadAnalytics"]["headAnalyticsAlgo"] = "HAAR";
    this->settings["aiSaacSettings"]["HeadAnalytics"]["minimumHeadSize"] = {0.3, 0.3};
    this->settings["aiSaacSettings"]["HeadAnalytics"]["maximumHeadSize"] = {0.9, 0.9};
    this->settings["aiSaacSettings"]["HeadAnalytics"]["headBlobDeleteTime"] = 60;
    this->settings["aiSaacSettings"]["HeadAnalytics"]["minimumHeadBlobFrames"] = 20;

    this->settings["aiSaacSettings"]["HumanAnalytics"]["humanDetectAnalyticsAlgo"] = "HOG";
    this->settings["aiSaacSettings"]["HumanAnalytics"]["humanBlobDeleteTime"] = 60;
    this->settings["aiSaacSettings"]["HumanAnalytics"]["minimumHumanBlobFrames"] = 20;
    this->settings["aiSaacSettings"]["HumanAnalytics"]["dpmPedestrianModelFilePath"] = "";
    this->settings["aiSaacSettings"]["HumanAnalytics"]["icfPedestrianModelFilePath"] = "";

    this->settings["aiSaacSettings"]["FaceAnalytics"]["faceDetectAnalyticsAlgo"] = "NPD";
    this->settings["aiSaacSettings"]["FaceAnalytics"]["faceBlobDeleteTime"] = 60;
    this->settings["aiSaacSettings"]["FaceAnalytics"]["minimumFaceBlobFrames"] = 20;
    this->settings["aiSaacSettings"]["FaceAnalytics"]["minimumFaceSize"] = {0.3, 0.3};
    this->settings["aiSaacSettings"]["FaceAnalytics"]["maximumFaceSize"] = {0.9, 0.9};
    this->settings["aiSaacSettings"]["FaceAnalytics"]["NPDFaceDetectFilePath"] = "";
    this->settings["aiSaacSettings"]["FaceAnalytics"]["faceStorageMode"] = false;
    this->settings["aiSaacSettings"]["FaceAnalytics"]["faceRecognitionTrainingDataPath"] ="";
    this->settings["aiSaacSettings"]["FaceAnalytics"]["VGGFacePrototxtPath"] = "";
    this->settings["aiSaacSettings"]["FaceAnalytics"]["VGGFaceCaffemodelPath"] = "";
    this->settings["aiSaacSettings"]["FaceAnalytics"]["faceRecognitionDBPath"] = "";
    this->settings["aiSaacSettings"]["FaceAnalytics"]["faceRecognitionModelAlgo"] = "KNN";
    this->settings["aiSaacSettings"]["FaceAnalytics"]["knnThreshold"] = 10;
    this->settings["aiSaacSettings"]["FaceAnalytics"]["genderPrototxtPath"] = "";
    this->settings["aiSaacSettings"]["FaceAnalytics"]["genderCaffemodelPath"] = "";
    this->settings["aiSaacSettings"]["FaceAnalytics"]["genderMeanFilePath"] = "";
    this->settings["aiSaacSettings"]["FaceAnalytics"]["genderLabelFilePath"] = "";
    this->settings["aiSaacSettings"]["FaceAnalytics"]["genderRecognitionThreshold"] = 0.7;
    this->settings["aiSaacSettings"]["FaceAnalytics"]["agePrototxtPath"] = "";
    this->settings["aiSaacSettings"]["FaceAnalytics"]["ageCaffemodelPath"] = "";
    this->settings["aiSaacSettings"]["FaceAnalytics"]["ageMeanFilePath"] = "";
    this->settings["aiSaacSettings"]["FaceAnalytics"]["ageLabelFilePath"] = "";
    this->settings["aiSaacSettings"]["FaceAnalytics"]["ageRecognitionThreshold"] = 0.7;
    this->settings["aiSaacSettings"]["FaceAnalytics"]["emotionPrototxtPath"] = "";
    this->settings["aiSaacSettings"]["FaceAnalytics"]["emotionCaffemodelPath"] = "";
    this->settings["aiSaacSettings"]["FaceAnalytics"]["emotionMeanFilePath"] = "";
    this->settings["aiSaacSettings"]["FaceAnalytics"]["emotionLabelFilePath"] = "";
    this->settings["aiSaacSettings"]["FaceAnalytics"]["DarknetFaceDetectConfigFilePath"] = "";
    this->settings["aiSaacSettings"]["FaceAnalytics"]["DarknetFaceDetectWeightFilePath"] = "";
    this->settings["aiSaacSettings"]["FaceAnalytics"]["DarknetFaceDetectLabelFilePath"] = "";
    this->settings["aiSaacSettings"]["FaceAnalytics"]["DarknetFaceDetectConfidenceThreshold"] = "0.1";


    this->settings["aiSaacSettings"]["ObjectAnalytics"]["objectAnalyticsAlgo"] = "CAFFE";
    this->settings["aiSaacSettings"]["ObjectAnalytics"]["objectBlobDeleteTime"] = 60;
    this->settings["aiSaacSettings"]["ObjectAnalytics"]["minimumObjectBlobFrames"] = 20;
    this->settings["aiSaacSettings"]["ObjectAnalytics"]["objectCaffePrototxtPath"] = "";
    this->settings["aiSaacSettings"]["ObjectAnalytics"]["objectCaffeCaffemodelPath"] = "";
    this->settings["aiSaacSettings"]["ObjectAnalytics"]["objectCaffeMeanFilePath"] = "";
    this->settings["aiSaacSettings"]["ObjectAnalytics"]["objectCaffeLabelFilePath"] = "";
    this->settings["aiSaacSettings"]["ObjectAnalytics"]["objectDarknetConfigFilePath"] = "";
    this->settings["aiSaacSettings"]["ObjectAnalytics"]["objectDarknetWeightFilePath"] = "";
    this->settings["aiSaacSettings"]["ObjectAnalytics"]["objectDarknetLabelFilePath"] = "";
    this->settings["aiSaacSettings"]["ObjectAnalytics"]["objectAnalyticsConfidenceThreshold"]
        = 0.1;

    this->settings["aiSaacSettings"]["SceneAnalytics"]["sceneAnalyticsAlgo"] = "CAFFE";
    this->settings["aiSaacSettings"]["SceneAnalytics"]["sceneBlobDeleteTime"] = 60;
    this->settings["aiSaacSettings"]["SceneAnalytics"]["minimumSceneBlobFrames"] = 20;
    this->settings["aiSaacSettings"]["SceneAnalytics"]["scenePrototxtPath"] = "";
    this->settings["aiSaacSettings"]["SceneAnalytics"]["sceneCaffemodelPath"] = "";
    this->settings["aiSaacSettings"]["SceneAnalytics"]["sceneMeanFilePath"] = "";
    this->settings["aiSaacSettings"]["SceneAnalytics"]["sceneLabelFilePath"] = "";
    this->settings["aiSaacSettings"]["SceneAnalytics"]["sceneAnalyticsConfidenceThreshold"]
        = 0.1;

    this->settings["aiSaacSettings"]["TextAnalytics"]["textAnalyticsAlgo"] = "SWT+TES";
    this->settings["aiSaacSettings"]["TextAnalytics"]["textBlobDeleteTime"] = 60;
    this->settings["aiSaacSettings"]["TextAnalytics"]["minimumTextBlobFrames"] = 20;
    this->settings["aiSaacSettings"]["TextAnalytics"]["textRectDistanceThresh"] = 10;

    this->settings["aiSaacSettings"]["VehicleAnalytics"]["vehicleAnalyticsAlgo"] = "HAAR";
    this->settings["aiSaacSettings"]["VehicleAnalytics"]["vehicleBlobDeleteTime"] = 60;
    this->settings["aiSaacSettings"]["VehicleAnalytics"]["minimumVehicleBlobFrames"] = 20;
    this->settings["aiSaacSettings"]["VehicleAnalytics"]["minimumVehicleSize"] = {30, 30};
    this->settings["aiSaacSettings"]["VehicleAnalytics"]["maximumVehicleSize"] = {100, 100};

    this->settings["aiSaacSettings"]["NSFWAnalytics"]["NSFWAnalyticsAlgo"] = "CAFFE";
    this->settings["aiSaacSettings"]["NSFWAnalytics"]["NSFWCaffePrototxtPath"] = "";
    this->settings["aiSaacSettings"]["NSFWAnalytics"]["NSFWCaffeCaffemodelPath"] = "";
    this->settings["aiSaacSettings"]["NSFWAnalytics"]["NSFWCaffeLabelFilePath"] = "";
    this->settings["aiSaacSettings"]["NSFWAnalytics"]["NSFWAnalyticsConfidenceThreshold"] = 0.1;


    this->settings["aiSaacSettings"]["haarCascadeFilePath"] = "";
    this->settings["aiSaacSettings"]["procFPS"] = 1;
    // for (nlohmann::json::iterator it = this->settings.begin(); it != this->settings.end(); ++it) {
    //     std::cout << *it << '\n';
    // }
    this->saveSettings();
}
