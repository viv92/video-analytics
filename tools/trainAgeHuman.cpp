#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <dirent.h>
#include <vector>
#include <fstream>
#include "../src/aiSaac.h"
#include "datk.h"
#include "developerConfig.h"
#include "../src/utils/caffeTrainer.h"

int main(){
    setDeveloperVariables(developerName);
    aiSaac::CASettings *caSettings = new aiSaac::CASettings( storagePath, "AgeClassificationHuman" );
    caSettings->setMode(1);
    caSettings->setSolverMode(1);
    caSettings->setTrainingDataPath(storagePath + "/data/Human Data/Age/Train");
    caSettings->setTestingDataPath(storagePath + "/data/Human Data/Age/Test");
    caSettings->setCaffeTemplateArchitecturePath(storagePath + "/caffeFiles/age/train_val_age.prototxt");
    caSettings->setCaffeTemplateSolverPath(storagePath + "/caffeFiles/age/solver_age.prototxt");
    caSettings->setOriginalCaffeModelPath(storagePath + "/caffeFiles/age/age_net.caffemodel");
    // caSettings->setSolverStatePath(storagePath + "/fineTuning/model/age_snapshot_iter_100.solverstate");
    caSettings->setWorkingDir(storagePath + "/fineTuning");
    caSettings->setNewNetworkName("AgeNetHuman");
    caSettings->setBackpropOptimizationAlgo("Adam");
    caSettings->setMaxIterations(500);
    caSettings->setTestIterationSize(100);
    caSettings->setStepSize(500);
    caSettings->setBaseLR(0.0001);
    caSettings->setIsTrained(false);
    caSettings->setIsTested(false);
    caSettings->saveSettings();
    aiSaac::CustomAnalytics ca1(caSettings);
    ca1.train();
}
