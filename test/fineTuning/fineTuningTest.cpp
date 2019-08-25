#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <dirent.h>
#include <vector>
#include <fstream>
#include "../../src/aiSaac.h"
#include "datk.h"
#include "../../src/utils/caffeTrainer.h"

// ./aiSaacFineTuning /home/shetty4l/Downloads/AITOEVA/aisaac-fork/build/storageDir/data/FaceRecData1/Train /home/shetty4l/Downloads/AITOEVA/aisaac-fork/build/storageDir/caffeFiles/train_val.prototxt /home/shetty4l/Downloads/AITOEVA/aisaac-fork/build/storageDir/caffeFiles/solver.prototxt /home/shetty4l/Downloads/AITOEVA/aisaac-fork/build/storageDir/caffeFiles/bvlc_reference_caffenet.caffemodel /home/shetty4l/Downloads/AITOEVA/aisaac-fork/build/storageDir/fineTuning

int main(){
	aiSaac::CASettings *caSettings = new aiSaac::CASettings(
		storagePath, "FaceRecognition");
	caSettings->setMode(1);
	caSettings->setTrainingDataPath(storagePath + "/data/minData/Train");
    // caSettings->setTestingDataPath(storagePath + "/data/FaceRecData1/Test");
    caSettings->setCaffeTemplateArchitecturePath(storagePath + "/caffeFiles/age/train_val_age.prototxt");
    caSettings->setCaffeTemplateSolverPath(storagePath + "/caffeFiles/age/solver_age.prototxt");
    caSettings->setOriginalCaffeModelPath(storagePath + "/caffeFiles/age/age_net.caffemodel");
    caSettings->setWorkingDir(storagePath + "/fineTuning");
    caSettings->setIsTrained(false);
    caSettings->setIsTested(false);

    std::string workingDir = caSettings->getWorkingDir();
    caSettings->setCaffeModelFilePath(workingDir + "/model/CustomNet.prototxt");
    caSettings->setNewCaffeTrainedFilePath(storagePath + "/caffeFiles/age/age_net.caffemodel");
    caSettings->setMeanFilePath(workingDir + "/model/meanbinary.proto");
    caSettings->setLabelFilePath(workingDir + "/model/labels.txt");

    aiSaac::CustomAnalytics ca1(caSettings);
    ca1.train();
}
