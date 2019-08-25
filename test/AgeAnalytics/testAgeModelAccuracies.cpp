#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <dirent.h>
#include <vector>
#include <fstream>
#include "../../src/aiSaac.h"
#include "datk.h"
#include "../../tools/videoSources/fileStreamer.h"

int main(int argc, char *argv[]) {

    // Transfer learning using Age model
    aiSaac::CASettings *caSettingsAge =
            new aiSaac::CASettings(storagePath, "ageModel");
    caSettingsAge->setTrainingDataPath(storagePath + "/data/AgeData2/Train");
    caSettingsAge->setTestingDataPath(storagePath + "/data/AgeData2/Test");
    caSettingsAge->setLabelFilePath(
            storagePath + "/caffeFiles/ageCustomAnalyticslabels.txt");
    caSettingsAge->setCaffeNetworkFilePath(
            storagePath + "/caffeFiles/age/deploy_age.prototxt");
    caSettingsAge->setCaffeTrainedFilePath(
            storagePath + "/caffeFiles/age/age_net.caffemodel");
    caSettingsAge->setTrainingJsonDataPath(
            storagePath + "/data/AgeJSON/trainingData_AgeData2_ageModel.json");
    caSettingsAge->setTestingJsonDataPath(
            storagePath + "/data/AgeJSON/testingData_AgeData2_ageModel.json");
    caSettingsAge->setIsTrained(false);
    caSettingsAge->setIsTested(false);
    caSettingsAge->setDATKSaveModelPath(
            storagePath + "/data/AgeData2/AgeData2_ageModel.datk");
    caSettingsAge->setMLAlgo("LRL2");
    caSettingsAge->saveSettings();
    aiSaac::CustomAnalytics *caAge = new aiSaac::CustomAnalytics(caSettingsAge);
    std::cout << "Training " << "age " << "model" << std::endl;
    caAge->train();
    std::cout << "Testing " << "age " << "model" << std::endl;
    caAge->test();

    // Transfer learning using Age VGG model
    aiSaac::CASettings *caSettingsAgeVGG =
            new aiSaac::CASettings(storagePath, "ageVGGModel");
    caSettingsAgeVGG->setTrainingDataPath(storagePath + "/data/AgeData2/Train");
    caSettingsAgeVGG->setTestingDataPath(storagePath + "/data/AgeData2/Test");
    caSettingsAgeVGG->setLabelFilePath(
            storagePath + "/caffeFiles/ageCustomAnalyticslabels.txt");
    caSettingsAgeVGG->setCaffeNetworkFilePath(
            storagePath + "/caffeFiles/ageVGG/age.prototxt");
    caSettingsAgeVGG->setCaffeTrainedFilePath(
            storagePath + "/caffeFiles/ageVGG/dex_imdb_wiki.caffemodel");
    caSettingsAgeVGG->setTrainingJsonDataPath(
            storagePath + "/data/AgeJSON/trainingData_AgeData2_ageVGGModel.json");
    caSettingsAgeVGG->setTestingJsonDataPath(
            storagePath + "/data/AgeJSON/testingData_AgeData2_ageVGGModel.json");
    caSettingsAgeVGG->setIsTrained(false);
    caSettingsAgeVGG->setIsTested(false);
    caSettingsAgeVGG->setDATKSaveModelPath(
            storagePath + "/data/AgeData2/AgeData2_ageVGGModel.datk");
    caSettingsAgeVGG->setMLAlgo("LRL2");
    caSettingsAgeVGG->saveSettings();
    aiSaac::CustomAnalytics *caAgeVGG =
            new aiSaac::CustomAnalytics(caSettingsAgeVGG);
    std::cout << "Training " << "ageVGG " << "model" << std::endl;
    caAgeVGG->train();
    std::cout << "Testing " << "ageVGG " << "model" << std::endl;
    caAgeVGG->test();

    // Transfer learning using Gender model
    aiSaac::CASettings *caSettingsGender =
            new aiSaac::CASettings(storagePath, "genderModel");
    caSettingsGender->setTrainingDataPath(storagePath + "/data/AgeData2/Train");
    caSettingsGender->setTestingDataPath(storagePath + "/data/AgeData2/Test");
    caSettingsGender->setLabelFilePath(
            storagePath + "/caffeFiles/ageCustomAnalyticslabels.txt");
    caSettingsGender->setCaffeNetworkFilePath(
            storagePath + "/caffeFiles/gender/deploy_gender.prototxt");
    caSettingsGender->setCaffeTrainedFilePath(
            storagePath + "/caffeFiles/gender/gender_net.caffemodel");
    caSettingsGender->setTrainingJsonDataPath(
            storagePath + "/data/AgeJSON/trainingData_AgeData2_genderModel.json");
    caSettingsGender->setTestingJsonDataPath(
            storagePath + "/data/AgeJSON/testingData_AgeData2_genderModel.json");
    caSettingsGender->setIsTrained(false);
    caSettingsGender->setIsTested(false);
    caSettingsGender->setDATKSaveModelPath(
            storagePath + "/data/AgeData2/AgeData2_genderModel.datk");
    caSettingsGender->setMLAlgo("LRL2");
    caSettingsGender->saveSettings();
    aiSaac::CustomAnalytics *caGender =
            new aiSaac::CustomAnalytics(caSettingsGender);
    std::cout << "Training " << "gender " << "model" << std::endl;
    caGender->train();
    std::cout << "Testing " << "gender " << "model" << std::endl;
    caGender->test();


    // Transfer learning using Gender VGG model
    aiSaac::CASettings *caSettingsGenderVGG =
            new aiSaac::CASettings(storagePath, "genderVGGModel");
    caSettingsGenderVGG->setTrainingDataPath(storagePath + "/data/AgeData2/Train");
    caSettingsGenderVGG->setTestingDataPath(storagePath + "/data/AgeData2/Test");
    caSettingsGenderVGG->setLabelFilePath(
            storagePath + "/caffeFiles/ageCustomAnalyticslabels.txt");
    caSettingsGenderVGG->setCaffeNetworkFilePath(
            storagePath + "/caffeFiles/gender_VGG/gender.prototxt");
    caSettingsGenderVGG->setCaffeTrainedFilePath(
            storagePath + "/caffeFiles/gender_VGG/gender.caffemodel");
    caSettingsGenderVGG->setTrainingJsonDataPath(
            storagePath + "/data/AgeJSON/trainingData_AgeData2_genderVGGModel.json");
    caSettingsGenderVGG->setTestingJsonDataPath(
            storagePath + "/data/AgeJSON/testingData_AgeData2_genderVGGModel.json");
    caSettingsGenderVGG->setIsTrained(false);
    caSettingsGenderVGG->setIsTested(false);
    caSettingsGenderVGG->setDATKSaveModelPath(
            storagePath + "/data/AgeData2/AgeData2_genderVGGModel.datk");
    caSettingsGenderVGG->setMLAlgo("LRL2");
    caSettingsGenderVGG->saveSettings();
    aiSaac::CustomAnalytics *caGenderVGG =
            new aiSaac::CustomAnalytics(caSettingsGenderVGG);
    std::cout << "Training " << "genderVGG " << "model" << std::endl;
    caGenderVGG->train();
    std::cout << "Testing " << "genderVGG " << "model" << std::endl;
    caGenderVGG->test();

    // Transfer learning using Gender Googlenet model
    aiSaac::CASettings *caSettingsGenderGooglenet =
            new aiSaac::CASettings(storagePath, "genderGooglenetModel");
    caSettingsGenderGooglenet->setTrainingDataPath(storagePath + "/data/AgeData2/Train");
    caSettingsGenderGooglenet->setTestingDataPath(storagePath + "/data/AgeData2/Test");
    caSettingsGenderGooglenet->setLabelFilePath(
            storagePath + "/caffeFiles/ageCustomAnalyticslabels.txt");
    caSettingsGenderGooglenet->setCaffeNetworkFilePath(
            storagePath + "/caffeFiles/gender_GoogleNet/gender.prototxt");
    caSettingsGenderGooglenet->setCaffeTrainedFilePath(
            storagePath + "/caffeFiles/gender_GoogleNet/gender.caffemodel");
    caSettingsGenderGooglenet->setTrainingJsonDataPath(
            storagePath + "/data/AgeJSON/trainingData_AgeData2_genderGoogleNetModel.json");
    caSettingsGenderGooglenet->setTestingJsonDataPath(
            storagePath + "/data/AgeJSON/testingData_AgeData2_genderGoogleNetModel.json");
    caSettingsGenderGooglenet->setIsTrained(false);
    caSettingsGenderGooglenet->setIsTested(false);
    caSettingsGenderGooglenet->setDATKSaveModelPath(
            storagePath + "/data/AgeData2/AgeData2_genderGoogleNetModel.datk");
    caSettingsGenderGooglenet->setMLAlgo("LRL2");
    caSettingsGenderGooglenet->setCaffeFeatureExtractionLayer("pool5/7x7_s1");
    caSettingsGenderGooglenet->saveSettings();
    aiSaac::CustomAnalytics *caGenderGooglenet =
            new aiSaac::CustomAnalytics(caSettingsGenderGooglenet);
    std::cout << "Training " << "genderGooglenet " << "model" << std::endl;
    caGenderGooglenet->train();
    std::cout << "Testing " << "genderGooglenet " << "model" << std::endl;
    caGenderGooglenet->test();

    // Transfer learning using Face model
    aiSaac::CASettings *caSettingsFace =
            new aiSaac::CASettings(storagePath, "faceModel");
    caSettingsFace->setTrainingDataPath(storagePath + "/data/AgeData2/Train");
    caSettingsFace->setTestingDataPath(storagePath + "/data/AgeData2/Test");
    caSettingsFace->setLabelFilePath(
            storagePath + "/caffeFiles/ageCustomAnalyticslabels.txt");
    caSettingsFace->setCaffeNetworkFilePath(
            storagePath + "/caffeFiles/face/VGG_FACE_deploy.prototxt");
    caSettingsFace->setCaffeTrainedFilePath(
            storagePath + "/caffeFiles/face/VGG_FACE.caffemodel");
    caSettingsFace->setTrainingJsonDataPath(
            storagePath + "/data/AgeJSON/trainingData_AgeData2_faceModel.json");
    caSettingsFace->setTestingJsonDataPath(
            storagePath + "/data/AgeJSON/testingData_AgeData2_faceModel.json");
    caSettingsFace->setIsTrained(false);
    caSettingsFace->setIsTested(false);
    caSettingsFace->setDATKSaveModelPath(
            storagePath + "/data/AgeData2/AgeData2_faceModel.datk");
    caSettingsFace->setMLAlgo("LRL2");
    caSettingsFace->saveSettings();
    aiSaac::CustomAnalytics *caFace =
            new aiSaac::CustomAnalytics(caSettingsFace);
    std::cout << "Training " << "face " << "model" << std::endl;
    caFace->train();
    std::cout << "Testing " << "face " << "model" << std::endl;
    caFace->test();

    aiSaac::AiSaacSettings *aiSaacSettingsAgeModel =
            new aiSaac::AiSaacSettings(storagePath, "configAgeModel");
    aiSaacSettingsAgeModel->setAgePrototxtPath(
            storagePath + "/caffeFiles/age/deploy_age.prototxt");
    aiSaacSettingsAgeModel->setAgeCaffemodelPath(
            storagePath + "/caffeFiles/age/age_net.caffemodel");
    aiSaacSettingsAgeModel->setAgeMeanFilePath(
            storagePath + "/caffeFiles/age/mean.binaryproto");
    aiSaacSettingsAgeModel->setAgeLabelFilePath(
            storagePath + "/caffeFiles/age/age_synset_words.txt");
    aiSaacSettingsAgeModel->saveSettings();

    aiSaac::AiSaacSettings *aiSaacSettingsAgeVGGModel =
            new aiSaac::AiSaacSettings(storagePath, "configAgeVGGModel");
    aiSaacSettingsAgeVGGModel->setAgePrototxtPath(
            storagePath + "/caffeFiles/ageVGG/age.prototxt");
    aiSaacSettingsAgeVGGModel->setAgeCaffemodelPath(
            storagePath + "/caffeFiles/ageVGG/dex_imdb_wiki.caffemodel");
    aiSaacSettingsAgeVGGModel->setAgeMeanFilePath(
            storagePath + "/caffeFiles/age/mean.binaryproto");
    aiSaacSettingsAgeVGGModel->setAgeLabelFilePath(
            storagePath + "/caffeFiles/ageVGG/age_synset_words.txt");
    aiSaacSettingsAgeVGGModel->saveSettings();

    std::cout << "CALLING pretrained models" << std::endl;

    aiSaac::AgeRecognition *ageAnalyticsAgeModel =
            new aiSaac::AgeRecognition(aiSaacSettingsAgeModel);
    ageAnalyticsAgeModel->initialize();
    aiSaac::AgeRecognition *ageAnalyticsAgeVGGModel =
            new aiSaac::AgeRecognition(aiSaacSettingsAgeVGGModel);
    ageAnalyticsAgeVGGModel->initialize();

    int totalTestedFaces = 0;
    int totalCorrectFacesAgeModel = 0;
    int totalIncorrectFacesAgeModel = 0;
    int totalCorrectFacesAgeVGGModel = 0;
    int totalIncorrectFacesAgeVGGModel = 0;

    std::string folderPath = storagePath + "/data/AgeData2/Test";
    DIR *faceStorageDir;
    struct dirent *direntFaceStorageDir;
    DIR *ageDir;
    struct dirent *direntAgeDir;
    std::string ageFolder;
    std::string ageName;

    std::string faceImagePath;
    std::string delimiter = ",";
    faceStorageDir = opendir(folderPath.c_str());
    while((direntFaceStorageDir = readdir(faceStorageDir)) != NULL) {
        if (direntFaceStorageDir->d_type == DT_DIR
            && strcmp(direntFaceStorageDir->d_name, ".")
            && strcmp(direntFaceStorageDir->d_name, "..")) {

            ageName = direntFaceStorageDir->d_name;
            ageFolder = folderPath + "/" + ageName;
            std::cout<< "Age Folder" << ageFolder << std::endl;
            ageDir = opendir(ageFolder.c_str());
            while((direntAgeDir = readdir(ageDir)) != NULL) {
                if (direntAgeDir->d_type == DT_REG) {
                    std::string fileName(direntAgeDir->d_name);
                    faceImagePath = ageFolder + "/" + fileName;
                    std::cout << "final face image path: " << faceImagePath << std::endl;
                    cv::Mat faceImage = cv::imread(faceImagePath);
                    totalTestedFaces++;
                    std::string ageModelPrediction = ageAnalyticsAgeModel->runAlgo(faceImage);
                    std::cout << "PREDICTION age model = " << ageModelPrediction << std::endl;
                    if (ageModelPrediction == "(0, 2)" ||
                            ageModelPrediction == "(4, 6)" ||
                            ageModelPrediction == "(8, 12)" ) {
                        if (ageName == "(0, 15)")
                            totalCorrectFacesAgeModel++;
                        else
                            totalIncorrectFacesAgeModel++;
                    }

                    else if (ageModelPrediction == "(15, 20)" ||
                            ageModelPrediction == "(25, 32)" ) {
                        if (ageName == "(15, 30)")
                            totalCorrectFacesAgeModel++;
                        else
                            totalIncorrectFacesAgeModel++;
                    }
                    else if (ageModelPrediction == "(38, 43)" ||
                            ageModelPrediction == "(48, 53)" ) {
                        if (ageName == "(30, 50)")
                            totalCorrectFacesAgeModel++;
                        else
                            totalIncorrectFacesAgeModel++;
                    }

                    else if (ageModelPrediction == "(60, 100)") {
                        if (ageName == "(50, 100)")
                            totalCorrectFacesAgeModel++;
                        else
                            totalIncorrectFacesAgeModel++;
                    }
                    double vggPredictedAge =
                        ageAnalyticsAgeVGGModel->detectExpected(faceImage);
                    std::cout << "PREDICTION ageVGG model = " << vggPredictedAge << std::endl;
                    if (0 <= vggPredictedAge <= 15) {
                        if (ageName == "(0, 15)")
                            totalCorrectFacesAgeVGGModel++;
                        else
                            totalIncorrectFacesAgeVGGModel++;
                    }

                    else if (16 <= vggPredictedAge <= 30) {
                        if (ageName == "(15, 30)")
                            totalCorrectFacesAgeVGGModel++;
                        else
                            totalIncorrectFacesAgeVGGModel++;
                    }

                    else if (31 <= vggPredictedAge <= 50) {
                        if (ageName == "(30, 50)")
                            totalCorrectFacesAgeVGGModel++;
                        else
                            totalIncorrectFacesAgeVGGModel++;
                    }

                    else if (51 <= vggPredictedAge) {
                        if (ageName == "(50, 100)")
                            totalCorrectFacesAgeVGGModel++;
                        else
                            totalIncorrectFacesAgeVGGModel++;
                    }

                }

            }
        }
    }
    std::cout << "Total faces " << totalTestedFaces << std::endl;
    std::cout << "age Model" << std::endl;
    std::cout << "\tCorrect = " << totalCorrectFacesAgeModel
            << " Incorrect =" << totalIncorrectFacesAgeModel << std::endl;
    std::cout << "\tAccuract = " <<
            ((float)totalCorrectFacesAgeModel)/((float)totalTestedFaces) << std::endl;
    std::cout << "age VGG Model" << std::endl;
    std::cout << "\tCorrect = " << totalCorrectFacesAgeVGGModel
            << " Incorrect =" << totalIncorrectFacesAgeVGGModel << std::endl;
    std::cout << "\tAccuract = " <<
            ((float)totalCorrectFacesAgeVGGModel)/((float)totalTestedFaces) << std::endl;

    return 0;
}
