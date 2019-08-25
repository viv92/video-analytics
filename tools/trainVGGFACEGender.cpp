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
    aiSaac::CASettings *caSettings = new aiSaac::CASettings( storagePath, "GenderClassification" );
    caSettings->setMode(1);
    caSettings->setSolverMode(1);
    caSettings->setTrainingDataPath(storagePath + "/data/GenderData1/Train");
    caSettings->setTestingDataPath(storagePath + "/data/GenderData1/Test");
    caSettings->setCaffeTemplateArchitecturePath(storagePath + "/caffeFiles/gender_vgg/VGG_FACE_train_val.prototxt");
    caSettings->setCaffeTemplateSolverPath(storagePath + "/caffeFiles/gender_vgg/solver_gender.prototxt");
    caSettings->setOriginalCaffeModelPath(storagePath + "/caffeFiles/gender_vgg/VGG_FACE.caffemodel");
    // caSettings->setSolverStatePath(storagePath + "/fineTuning/model/age_snapshot_iter_100.solverstate");
    caSettings->setSolverStatePath("");
    caSettings->setWorkingDir(storagePath + "/fineTuning/FACE_VGG_GENDER");
    caSettings->setNewNetworkName("FACEVGGGenderNet");
    caSettings->setBackpropOptimizationAlgo("Adam");
    caSettings->setMaxIterations(500);
    caSettings->setTestIterationSize(50);
    caSettings->setStepSize(500);
    caSettings->setBaseLR(0.0001);
    caSettings->setIsTrained(false);
    caSettings->setIsTested(false);
    caSettings->saveSettings();
    aiSaac::CustomAnalytics ca1(caSettings);
    ca1.train();
}
