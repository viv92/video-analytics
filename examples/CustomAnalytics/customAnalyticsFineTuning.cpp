#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <dirent.h>
#include <vector>
#include <fstream>
#include "../src/aiSaac.h"
#include "datk.h"
#include "../src/utils/caffeTrainer.h"

int main(int argc, char *argv[]){
    if (argc < 2){
        std::cout << "Usage: ./customAnalyticsFineTuning [caSettingsPath]"
        return;
    }
    std::string caSettingsPath = argv[1];
    aiSaac::CASettings *caSettings = new aiSaac::CASettings(caSettingsPath);
    caSettings->setMode(1);
    caSettings->setSolverMode(1);
    // caSettings->setTrainingDataPath(storagePath + "/data/AgeData2/Train");
    // caSettings->setTestingDataPath(storagePath + "/data/AgeData2/Test");
    // caSettings->setCaffeTemplateArchitecturePath(storagePath + "/fineTuning/AgeNet/model/AgeNet.prototxt");
    // caSettings->setCaffeTemplateSolverPath(storagePath + "/fineTuning/AgeNet/model/solver.prototxt");
    // caSettings->setOriginalCaffeModelPath(storagePath + "/fineTuning/AgeNet/model/age_snapshot_iter_500.caffemodel");
    // // caSettings->setSolverStatePath(storagePath + "/fineTuning/model/age_snapshot_iter_100.solverstate");
    // caSettings->setWorkingDir(storagePath + "/fineTuning/age_test");
    // caSettings->setNewNetworkName("AgeTest");
    // caSettings->setBackpropOptimizationAlgo("Adam");
    // caSettings->setMaxIterations(500);
    // caSettings->setTestIterationSize(50);
    // caSettings->setStepSize(500);
    // caSettings->setBaseLR(0.0001);
    // caSettings->setIsTrained(false);
    // caSettings->setIsTested(false);
    // caSettings->saveSettings();
    aiSaac::CustomAnalytics ca1(caSettings);
    ca1.train();
}
