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
    aiSaac::CASettings *caSettings = new aiSaac::CASettings( storagePath, "AgeClassification" );
    caSettings->setMode(1);
    caSettings->setSolverMode(1);
    caSettings->setTrainingDataPath(storagePath + "/data/AgeData2/Train");
    caSettings->setTestingDataPath(storagePath + "/data/AgeData2/Test");
    caSettings->setCaffeTemplateArchitecturePath(storagePath + "/caffeFiles/age_vgg/VGG_FACE_train_val.prototxt");
    caSettings->setCaffeTemplateSolverPath(storagePath + "/caffeFiles/age_vgg/solver_age.prototxt");
    caSettings->setOriginalCaffeModelPath(storagePath + "/caffeFiles/age_vgg/VGG_FACE.caffemodel");
    // caSettings->setSolverStatePath(storagePath + "/fineTuning/FACE_VGG_AGE/model/age_snapshot_iter_100.solverstate");
    caSettings->setSolverStatePath("");
    caSettings->setWorkingDir(storagePath + "/fineTuning/FACE_VGG_AGE");
    caSettings->setNewNetworkName("FACEVGGAgeNet");
    caSettings->setBackpropOptimizationAlgo("SGD");
    caSettings->setMaxIterations(500);
    caSettings->setTestIterationSize(50);
    caSettings->setStepSize(500);
    caSettings->setBaseLR(0.001);
    caSettings->setIsTrained(false);
    caSettings->setIsTested(false);
    caSettings->saveSettings();
    aiSaac::CustomAnalytics ca1(caSettings);
    ca1.train();
}
