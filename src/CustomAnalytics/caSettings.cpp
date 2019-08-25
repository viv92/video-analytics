/*
    Copyright 2016 AITOE
*/

/*
    TODO:
    1. Error handling if settingsFilePath/instandID passed is invalid (to wrong folder etc.)
    2. In constructor accept only one value: caSettingsFilePath
*/

#include "caSettings.h"

aiSaac::CASettings::CASettings(std::string caSettingsFilePath) {
    this->CAID = caSettingsFilePath;

    // std::cout << "Creating/Loading CA settings for " << this->CAID << std::endl;

    for (int i = caSettingsFilePath.length() - 1; i >= 0; i--) {
        if (caSettingsFilePath[i] == '/') {
            this->caSettingsFilePath = caSettingsFilePath.substr(0, i + 1);
            break;
        }
    }
    this->caSettingsFileName = caSettingsFilePath;
    std::cout << "DOCTOR 1" << this->caSettingsFilePath << std::endl;
    std::cout << "DOCTOR 2" << this->caSettingsFileName << std::endl;
    std::ifstream f(this->caSettingsFileName);
    if (f.good()) {
        std::cout << "Loading settings " + this->CAID << std::endl;
        this->loadSettings();
    } else {
        std::cout << "Cannot find settings file. Creating." << std::endl;
        this->resetDefaultValues();
        // todo advanceSettings->resetDefaultValues();
    }
}

aiSaac::CASettings::CASettings(std::string caSettingsFilePath, std::string CAID) {
    std::cout << "CASettings(std::string, std::string) is deprecated, " <<
            "Use CASettings(std::string) instead" << std::endl;
    this->CAID = CAID;
    std::string fileName = this->CAID;

    std::cout << "Creating CA settings for " << this->CAID << std::endl;

    // std::array<char, 4> wildChars {{'.', ':', '/', '\\'}};
    // for (int i = 0; i < wildChars.size(); i++) {
    //     std::replace(fileName.begin(), fileName.end(), wildChars[i], '_');
    // }
    this->caSettingsFilePath = caSettingsFilePath;
    this->caSettingsFileName = caSettingsFilePath + "/CASettings" + fileName + ".json";

    std::ifstream f(this->caSettingsFileName);
    if (f.good()) {
        std::cout << "Loading settings" << std::endl;
        this->loadSettings();
    } else {
        std::cout << "Cannot find settings file. Creating." << std::endl;
        this->resetDefaultValues();
        // todo advanceSettings->resetDefaultValues();
    }
}

aiSaac::CASettings::~CASettings() {}

void aiSaac::CASettings::setMode(int mode) {
    this->settings["caSettings"]["mode"] = mode;
    this->saveSettings();
}


int aiSaac::CASettings::getMode() {
    return this->settings["caSettings"]["mode"];
    this->saveSettings();
}

void aiSaac::CASettings::setCaffeNetworkFilePath(std::string networkFilePath) {
    this->settings["caSettings"]["caffeNetworkFilePath"] = networkFilePath;
    this->saveSettings();
}

std::string aiSaac::CASettings::getCaffeNetworkFilePath(){
    return this->settings["caSettings"]["caffeNetworkFilePath"];
    this->saveSettings();
}

void aiSaac::CASettings::setCaffeTrainedFilePath(std::string trainedFilePath) {
    this->settings["caSettings"]["caffeTrainedFilePath"] = trainedFilePath;
    this->saveSettings();
}

std::string aiSaac::CASettings::getCaffeTrainedFilePath() {
    return this->settings["caSettings"]["caffeTrainedFilePath"];
    this->saveSettings();
}

void aiSaac::CASettings::setCaffeTemplateArchitecturePath(std::string templateArchitecturePath) {
    this->settings["caSettings"]["templateArchitecturePath"] = templateArchitecturePath;
    this->saveSettings();
}

std::string aiSaac::CASettings::getCaffeTemplateArchitecturePath() {
    return this->settings["caSettings"]["templateArchitecturePath"];
    this->saveSettings();
}

void aiSaac::CASettings::setCaffeTemplateSolverPath(std::string templateSolverPath) {
    this->settings["caSettings"]["templateSolverPath"] = templateSolverPath;
    this->saveSettings();
}

std::string aiSaac::CASettings::getCaffeTemplateSolverPath() {
    return this->settings["caSettings"]["templateSolverPath"];
    this->saveSettings();
}

void aiSaac::CASettings::setOriginalCaffeModelPath(std::string originalCaffemodelPath) {
    this->settings["caSettings"]["originalCaffemodelPath"] = originalCaffemodelPath;
    this->saveSettings();
}

std::string aiSaac::CASettings::getOriginalCaffeModelPath() {
    return this->settings["caSettings"]["originalCaffemodelPath"];
    this->saveSettings();
}

void aiSaac::CASettings::setSolverStatePath(std::string solverStatePath) {
    this->settings["caSettings"]["solverStatePath"] = solverStatePath;
    this->saveSettings();
}

std::string aiSaac::CASettings::getSolverStatePath() {
    return this->settings["caSettings"]["solverStatePath"];
    this->saveSettings();
}

void aiSaac::CASettings::setWorkingDir(std::string workingDir) {
    this->settings["caSettings"]["workingDir"] = workingDir;
    this->saveSettings();
}

std::string aiSaac::CASettings::getWorkingDir() {
    return this->settings["caSettings"]["workingDir"];
    this->saveSettings();
}

void aiSaac::CASettings::setCaffeModelFilePath(std::string caffeModelFilePath) {
    this->settings["caSettings"]["caffeModelFilePath"] = caffeModelFilePath;
    this->saveSettings();
}

std::string aiSaac::CASettings::getCaffeModelFilePath() {
    return this->settings["caSettings"]["caffeModelFilePath"];
    this->saveSettings();
}

void aiSaac::CASettings::setNewCaffeTrainedFilePath(std::string newCaffeTrainedFilePath) {
    this->settings["caSettings"]["newCaffeTrainedFilePath"] = newCaffeTrainedFilePath;
    this->saveSettings();
}

std::string aiSaac::CASettings::getNewCaffeTrainedFilePath() {
    return this->settings["caSettings"]["newCaffeTrainedFilePath"];
    this->saveSettings();
}

void aiSaac::CASettings::setMeanFilePath(std::string meanFilePath) {
    this->settings["caSettings"]["meanFilePath"] = meanFilePath;
    this->saveSettings();
}

std::string aiSaac::CASettings::getMeanFilePath() {
    return this->settings["caSettings"]["meanFilePath"];
    this->saveSettings();
}

void aiSaac::CASettings::setCaffeFeatureExtractionLayer(std::string layer) {
    this->settings["caSettings"]["caffeFeatureExtractionLayer"] = layer;
    this->saveSettings();
}

std::string aiSaac::CASettings::getCaffeFeatureExtractionLayer() {
    return this->settings["caSettings"]["caffeFeatureExtractionLayer"];
    this->saveSettings();
}

void aiSaac::CASettings::setLabelFilePath(std::string filePath) {
    this->settings["caSettings"]["labelFilePath"] = filePath;
    this->saveSettings();
}

std::string aiSaac::CASettings::getLabelFilePath() {
    return this->settings["caSettings"]["labelFilePath"];
    this->saveSettings();
}

void aiSaac::CASettings::setTrainingDataPath(std::string trainingDataPath) {
    this->settings["caSettings"]["trainingDataPath"] = trainingDataPath;
    this->saveSettings();
}

std::string aiSaac::CASettings::getTrainingDataPath() {
    return this->settings["caSettings"]["trainingDataPath"];
    this->saveSettings();
}

void aiSaac::CASettings::setTestingDataPath(std::string testingDataPath) {
    this->settings["caSettings"]["testingDataPath"] = testingDataPath;
    this->saveSettings();
}

std::string aiSaac::CASettings::getTestingDataPath() {
    return this->settings["caSettings"]["testingDataPath"];
    this->saveSettings();
}

void aiSaac::CASettings::setTrainingJsonDataPath(std::string trainingJsonDataPath) {
    this->settings["caSettings"]["trainingJsonDataPath"] = trainingJsonDataPath;
    this->saveSettings();
}

std::string aiSaac::CASettings::getTrainingJsonDataPath() {
    return this->settings["caSettings"]["trainingJsonDataPath"];
    this->saveSettings();
}

void aiSaac::CASettings::setTrainingFileNamesJsonDataPath(std::string trainingFileNamesJsonDataPath) {
    this->settings["caSettings"]["trainingFileNamesJsonDataPath"] = trainingFileNamesJsonDataPath;
    this->saveSettings();
}

std::string aiSaac::CASettings::getTrainingFileNamesJsonDataPath() {
    return this->settings["caSettings"]["trainingFileNamesJsonDataPath"];
    this->saveSettings();
}

void aiSaac::CASettings::setTestingJsonDataPath(std::string testingJsonDataPath) {
    this->settings["caSettings"]["testingJsonDataPath"] = testingJsonDataPath;
    this->saveSettings();
}

std::string aiSaac::CASettings::getTestingJsonDataPath() {
    return this->settings["caSettings"]["testingJsonDataPath"];
    this->saveSettings();
}

void aiSaac::CASettings::setIsTrained(bool isTrained) {
    this->settings["caSettings"]["isTrained"] = isTrained;
    this->saveSettings();
}

bool aiSaac::CASettings::getIsTrained() {
    return this->settings["caSettings"]["isTrained"];
    this->saveSettings();
}

void aiSaac::CASettings::setIsTested(bool isTested) {
    this->settings["caSettings"]["isTested"] = isTested;
    this->saveSettings();
}

bool aiSaac::CASettings::getIsTested() {
    return this->settings["caSettings"]["isTested"];
    this->saveSettings();
}

void aiSaac::CASettings::setMLAlgo(std::string algo) {
    this->settings["caSettings"]["MLAlgo"] = algo;
    this->saveSettings();
}

std::string aiSaac::CASettings::getMLAlgo() {
    return this->settings["caSettings"]["MLAlgo"];
    this->saveSettings();
}

void aiSaac::CASettings::setDATKPredictionProbThresh(double algo) {
    this->settings["caSettings"]["DATKPredictionProbThresh"] = algo;
    this->saveSettings();
}

double aiSaac::CASettings::getDATKPredictionProbThresh() {
    return this->settings["caSettings"]["DATKPredictionProbThresh"];
    this->saveSettings();
}

// Linear Regression L1
void aiSaac::CASettings::setLRL1RegularizationParam(int lambda) {
    this->settings["caSettings"]["LRL1RegularizationParam"] = lambda;
    this->saveSettings();
}

int aiSaac::CASettings::getLRL1RegulatizationParam() {
    return this->settings["caSettings"]["LRL1RegularizationParam"];
    this->saveSettings();
}

void aiSaac::CASettings::setLRL1OptimizationAlgo(int optAlgo) {
    this->settings["caSettings"]["LRL1OptimizationAlgo"] = optAlgo;
    this->saveSettings();
}

int aiSaac::CASettings::getLRL1OptimizationAlgo() {
    return this->settings["caSettings"]["LRL1OptimizationAlgo"];
    this->saveSettings();
}

void aiSaac::CASettings::setLRL1NumOfIterations(int iterations) {
    this->settings["caSettings"]["LRL1NumOfIterations"] = iterations;
    this->saveSettings();
}

int aiSaac::CASettings::getLRL1NumOfIterations() {
    return this->settings["caSettings"]["LRL1NumOfIterations"];
    this->saveSettings();
}

void aiSaac::CASettings::setLRL1Tolerance(double tolerance) {
    this->settings["caSettings"]["LRL1Tolerance"] = tolerance;
    this->saveSettings();
}

double aiSaac::CASettings::getLRL1Tolerance() {
    return this->settings["caSettings"]["LRL1Tolerance"];
    this->saveSettings();
}

// Linear Regression L2
void aiSaac::CASettings::setLRL2RegularizationParam(int lambda) {
    this->settings["caSettings"]["LRL2RegularizationParam"] = lambda;
    this->saveSettings();
}

int aiSaac::CASettings::getLRL2RegulatizationParam() {
    return this->settings["caSettings"]["LRL2RegularizationParam"];
    this->saveSettings();
}

void aiSaac::CASettings::setLRL2OptimizationAlgo(int optAlgo) {
    this->settings["caSettings"]["LRL2OptimizationAlgo"] = optAlgo;
    this->saveSettings();
}

int aiSaac::CASettings::getLRL2OptimizationAlgo() {
    return this->settings["caSettings"]["LRL2OptimizationAlgo"];
    this->saveSettings();
}

void aiSaac::CASettings::setLRL2NumOfIterations(int iterations) {
    this->settings["caSettings"]["LRL2NumOfIterations"] = iterations;
    this->saveSettings();
}

int aiSaac::CASettings::getLRL2NumOfIterations() {
    return this->settings["caSettings"]["LRL2NumOfIterations"];
    this->saveSettings();
}

void aiSaac::CASettings::setLRL2Tolerance(double tolerance) {
    this->settings["caSettings"]["LRL2Tolerance"] = tolerance;
    this->saveSettings();
}

double aiSaac::CASettings::getLRL2Tolerance() {
    return this->settings["caSettings"]["LRL2Tolerance"];
    this->saveSettings();
}

// Hinge-SVM L2
void aiSaac::CASettings::setHSVML2RegularizationParam(int lambda) {
    this->settings["caSettings"]["HSVML2RegularizationParam"] = lambda;
    this->saveSettings();
}

int aiSaac::CASettings::getHSVML2RegulatizationParam() {
    return this->settings["caSettings"]["HSVML2RegularizationParam"];
    this->saveSettings();
}

void aiSaac::CASettings::setHSVML2OptimizationAlgo(int optAlgo) {
    this->settings["caSettings"]["HSVML2OptimizationAlgo"] = optAlgo;
    this->saveSettings();
}

int aiSaac::CASettings::getHSVML2OptimizationAlgo() {
    return this->settings["caSettings"]["HSVML2OptimizationAlgo"];
    this->saveSettings();
}

void aiSaac::CASettings::setHSVML2NumOfIterations(int iterations) {
    this->settings["caSettings"]["HSVML2NumOfIterations"] = iterations;
    this->saveSettings();
}

int aiSaac::CASettings::getHSVML2NumOfIterations() {
    return this->settings["caSettings"]["HSVML2NumOfIterations"];
    this->saveSettings();
}

void aiSaac::CASettings::setHSVML2Tolerance(double tolerance) {
    this->settings["caSettings"]["HSVML2Tolerance"] = tolerance;
    this->saveSettings();
}
double aiSaac::CASettings::getHSVML2Tolerance() {
    return this->settings["caSettings"]["HSVML2Tolerance"];
    this->saveSettings();
}

// Smooth-SVM L1
void aiSaac::CASettings::setSSVML1RegularizationParam(int lambda) {
    this->settings["caSettings"]["SSVML1RegularizationParam"] = lambda;
    this->saveSettings();
}

int aiSaac::CASettings::getSSVML1RegulatizationParam() {
    return this->settings["caSettings"]["SSVML1RegularizationParam"];
    this->saveSettings();
}

void aiSaac::CASettings::setSSVML1OptimizationAlgo(int optAlgo) {
    this->settings["caSettings"]["SSVML1OptimizationAlgo"] = optAlgo;
    this->saveSettings();
}

int aiSaac::CASettings::getSSVML1OptimizationAlgo() {
    return this->settings["caSettings"]["SSVML1OptimizationAlgo"];
    this->saveSettings();
}

void aiSaac::CASettings::setSSVML1NumOfIterations(int iterations) {
    this->settings["caSettings"]["SSVML1NumOfIterations"] = iterations;
    this->saveSettings();
}

int aiSaac::CASettings::getSSVML1NumOfIterations() {
    return this->settings["caSettings"]["SSVML1NumOfIterations"];
    this->saveSettings();
}

void aiSaac::CASettings::setSSVML1Tolerance(double tolerance) {
    this->settings["caSettings"]["SSVML1Tolerance"] = tolerance;
    this->saveSettings();
}

double aiSaac::CASettings::getSSVML1Tolerance() {
    return this->settings["caSettings"]["SSVML1Tolerance"];
    this->saveSettings();
}

// Smooth-SVM L2
void aiSaac::CASettings::setSSVML2RegularizationParam(int lambda) {
    this->settings["caSettings"]["SSVML2RegularizationParam"] = lambda;
    this->saveSettings();
}

int aiSaac::CASettings::getSSVML2RegulatizationParam() {
    return this->settings["caSettings"]["SSVML2RegularizationParam"];
    this->saveSettings();
}

void aiSaac::CASettings::setSSVML2OptimizationAlgo(int optAlgo) {
    this->settings["caSettings"]["SSVML2OptimizationAlgo"] = optAlgo;
    this->saveSettings();
}

int aiSaac::CASettings::getSSVML2OptimizationAlgo() {
    return this->settings["caSettings"]["SSVML2OptimizationAlgo"];
    this->saveSettings();
}

void aiSaac::CASettings::setSSVML2NumOfIterations(int iterations) {
    this->settings["caSettings"]["SSVML2NumOfIterations"] = iterations;
    this->saveSettings();
}

int aiSaac::CASettings::getSSVML2NumOfIterations() {
    return this->settings["caSettings"]["SSVML2NumOfIterations"];
    this->saveSettings();
}

void aiSaac::CASettings::setSSVML2Tolerance(double tolerance) {
    this->settings["caSettings"]["SSVML2Tolerance"] = tolerance;
    this->saveSettings();
}

double aiSaac::CASettings::getSSVML2Tolerance() {
    return this->settings["caSettings"]["SSVML2Tolerance"];
    this->saveSettings();
}

void aiSaac::CASettings::setDATKSaveModelPath(std::string modelPath) {
    this->settings["caSettings"]["DATKSaveModelPath"] = modelPath;
    this->saveSettings();
}

std::string aiSaac::CASettings::getDATKSaveModelPath() {
    return this->settings["caSettings"]["DATKSaveModelPath"];
    this->saveSettings();
}

void aiSaac::CASettings::setNewNetworkName(std::string newNetworkName) {
    this->settings["caSettings"]["newNetworkName"] = newNetworkName;
    this->saveSettings();
}

std::string aiSaac::CASettings::getNewNetworkName() {
    return this->settings["caSettings"]["newNetworkName"];
    this->saveSettings();
}

void aiSaac::CASettings::setBackpropOptimizationAlgo(std::string backpropOptimizationAlgo) {
    this->settings["caSettings"]["backpropOptimizationAlgo"] = backpropOptimizationAlgo;
    this->saveSettings();
}

std::string aiSaac::CASettings::getBackpropOptimizationAlgo() {
    return this->settings["caSettings"]["backpropOptimizationAlgo"];
    this->saveSettings();
}

void aiSaac::CASettings::setTrainingRatio(double trainingRatio) {
    this->settings["caSettings"]["trainingRatio"] = trainingRatio;
    this->saveSettings();
}

double aiSaac::CASettings::getTrainingRatio() {
    return this->settings["caSettings"]["trainingRatio"];
    this->saveSettings();
}

void aiSaac::CASettings::setResizeWidth(int resizeWidth) {
    this->settings["caSettings"]["resizeWidth"] = resizeWidth;
    this->saveSettings();
}

int aiSaac::CASettings::getResizeWidth() {
    return this->settings["caSettings"]["resizeWidth"];
    this->saveSettings();
}

void aiSaac::CASettings::setResizeHeight(int resizeHeight) {
    this->settings["caSettings"]["resizeHeight"] = resizeHeight;
    this->saveSettings();
}

int aiSaac::CASettings::getResizeHeight() {
    return this->settings["caSettings"]["resizeHeight"];
    this->saveSettings();
}

void aiSaac::CASettings::setIsColor(bool isColor) {
    this->settings["caSettings"]["isColor"] = isColor;
    this->saveSettings();
}

bool aiSaac::CASettings::getIsColor() {
    return this->settings["caSettings"]["isColor"];
    this->saveSettings();
}

void aiSaac::CASettings::setMaxIterations(int maxIterations) {
    this->settings["caSettings"]["maxIterations"] = maxIterations;
    this->saveSettings();
}

int aiSaac::CASettings::getMaxIterations() {
    return this->settings["caSettings"]["maxIterations"];
    this->saveSettings();
}

void aiSaac::CASettings::setTestIterationSize(int testIterationSize) {
    this->settings["caSettings"]["testIterationSize"] = testIterationSize;
    this->saveSettings();
}

int aiSaac::CASettings::getTestIterationSize() {
    return this->settings["caSettings"]["testIterationSize"];
    this->saveSettings();
}

void aiSaac::CASettings::setStepSize(int stepSize) {
    this->settings["caSettings"]["stepSize"] = stepSize;
    this->saveSettings();
}

int aiSaac::CASettings::getStepSize() {
    return this->settings["caSettings"]["stepSize"];
    this->saveSettings();
}

void aiSaac::CASettings::setBaseLR(float base_lr) {
    this->settings["caSettings"]["base_lr"] = base_lr;
    this->saveSettings();
}

float aiSaac::CASettings::getBaseLR() {
    return this->settings["caSettings"]["base_lr"];
    this->saveSettings();
}

void aiSaac::CASettings::setGamma(float gamma) {
    this->settings["caSettings"]["gamma"] = gamma;
    this->saveSettings();
}

float aiSaac::CASettings::getGamma() {
    return this->settings["caSettings"]["gamma"];
    this->saveSettings();
}

void aiSaac::CASettings::setSolverMode(int solverMode) {
    this->settings["caSettings"]["solverMode"] = solverMode;
    this->saveSettings();
}

int aiSaac::CASettings::getSolverMode() {
    return this->settings["caSettings"]["solverMode"];
    this->saveSettings();
}

void aiSaac::CASettings::loadSettings() {
    std::ifstream configFile;
    configFile.open(this->caSettingsFileName);
    configFile >> this->settings;
}

void aiSaac::CASettings::saveSettings() {
    std::cout << "saving file" << this->caSettingsFileName << std::endl;
    std::ofstream configFile;
    configFile.open(this->caSettingsFileName);
    configFile << this->settings.dump(4);
    configFile.close();
}

void aiSaac::CASettings::resetDefaultValues() {
    this->settings["caSettings"]["mode"] = 0;
    this->settings["caSettings"]["caffeNetworkFilePath"] = "";
    this->settings["caSettings"]["caffeTrainedFilePath"] = "";
    this->settings["caSettings"]["caffeFeatureExtractionLayer"] = "";
    this->settings["caSettings"]["labelFilePath"] = "";
    this->settings["caSettings"]["trainingDataPath"] = "";
    this->settings["caSettings"]["testingDataPath"] = "";
    this->settings["caSettings"]["trainingJsonDataPath"] = "";
    this->settings["caSettings"]["testingJsonDataPath"] = "";
    this->settings["caSettings"]["solverStatePath"] = "";
    this->settings["caSettings"]["isTrained"] = false;
    this->settings["caSettings"]["isTested"] = false;
    this->settings["caSettings"]["MLAlgo"] = "LRL2";
    this->settings["caSettings"]["LRL1RegularizationParam"] = 0;
    this->settings["caSettings"]["LRL1OptimizationAlgo"] = 0;
    this->settings["caSettings"]["LRL1NumOfIterations"] = 1000;
    this->settings["caSettings"]["LRL1Tolerance"] = 1e-2;
    this->settings["caSettings"]["LRL2RegularizationParam"] = 0;
    this->settings["caSettings"]["LRL2OptimizationAlgo"] = 0;
    this->settings["caSettings"]["LRL2NumOfIterations"] = 1000;
    this->settings["caSettings"]["LRL2Tolerance"] = 1e-2;
    this->settings["caSettings"]["HSVML2RegularizationParam"] = 0;
    this->settings["caSettings"]["HSVML2OptimizationAlgo"] = 0;
    this->settings["caSettings"]["HSVML2NumOfIterations"] = 1000;
    this->settings["caSettings"]["HSVML2Tolerance"] = 1e-2;
    this->settings["caSettings"]["SSVML1RegularizationParam"] = 0;
    this->settings["caSettings"]["SSVML1OptimizationAlgo"] = 0;
    this->settings["caSettings"]["SSVML1NumOfIterations"] = 1000;
    this->settings["caSettings"]["SSVML1Tolerance"] = 1e-2;
    this->settings["caSettings"]["SSVML2RegularizationParam"] = 0;
    this->settings["caSettings"]["SSVML2OptimizationAlgo"] = 0;
    this->settings["caSettings"]["SSVML2NumOfIterations"] = 1000;
    this->settings["caSettings"]["SSVML2Tolerance"] = 1e-2;
    this->settings["caSettings"]["DATKSaveModelPath"] = "";
    this->settings["caSettings"]["DATKPredictionProbThresh"] = 0.1;

    this->saveSettings();
}
