#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <dirent.h>
#include <vector>
#include <fstream>
#include "../../src/CustomAnalytics/caSettings.h"
#include "../../src/CustomAnalytics/customAnalytics.h"
#include "datk.h"

int main(int argc, char *argv[]) {

    // Transfer learning using Age model
    aiSaac::CASettings *caSettingsAge =
            new aiSaac::CASettings(storagePath, "ageModel");
    caSettingsAge->setTrainingDataPath(storagePath + "/data/FaceData2/Train");
    caSettingsAge->setTestingDataPath(storagePath + "/data/FaceData2/Test");
    caSettingsAge->setLabelFilePath(
            storagePath + "/caffeFiles/faceCustomAnalyticslabels.txt");
    caSettingsAge->setCaffeNetworkFilePath(
            storagePath + "/caffeFiles/age/deploy_age.prototxt");
    caSettingsAge->setCaffeTrainedFilePath(
            storagePath + "/caffeFiles/age/age_net.caffemodel");
    caSettingsAge->setTrainingJsonDataPath(
            storagePath + "/data/FaceJSON/trainingData_FaceData2_ageModel.json");
    caSettingsAge->setTestingJsonDataPath(
            storagePath + "/data/FaceJSON/testingData_FaceData2_ageModel.json");
    caSettingsAge->setIsTrained(false);
    caSettingsAge->setIsTested(false);
    caSettingsAge->setDATKSaveModelPath(
            storagePath + "/data/FaceData2/FaceData2_ageModel.datk");
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
    caSettingsAgeVGG->setTrainingDataPath(storagePath + "/data/FaceData2/Train");
    caSettingsAgeVGG->setTestingDataPath(storagePath + "/data/FaceData2/Test");
    caSettingsAgeVGG->setLabelFilePath(
            storagePath + "/caffeFiles/faceCustomAnalyticslabels.txt");
    caSettingsAgeVGG->setCaffeNetworkFilePath(
            storagePath + "/caffeFiles/ageVGG/age.prototxt");
    caSettingsAgeVGG->setCaffeTrainedFilePath(
            storagePath + "/caffeFiles/ageVGG/dex_imdb_wiki.caffemodel");
    caSettingsAgeVGG->setTrainingJsonDataPath(
            storagePath + "/data/FaceJSON/trainingData_FaceData2_ageVGGModel.json");
    caSettingsAgeVGG->setTestingJsonDataPath(
            storagePath + "/data/FaceJSON/testingData_FaceData2_ageVGGModel.json");
    caSettingsAgeVGG->setIsTrained(false);
    caSettingsAgeVGG->setIsTested(false);
    caSettingsAgeVGG->setDATKSaveModelPath(
            storagePath + "/data/FaceData2/FaceData2_ageVGGModel.datk");
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
    caSettingsGender->setTrainingDataPath(storagePath + "/data/FaceData2/Train");
    caSettingsGender->setTestingDataPath(storagePath + "/data/FaceData2/Test");
    caSettingsGender->setLabelFilePath(
            storagePath + "/caffeFiles/faceCustomAnalyticslabels.txt");
    caSettingsGender->setCaffeNetworkFilePath(
            storagePath + "/caffeFiles/gender/deploy_gender.prototxt");
    caSettingsGender->setCaffeTrainedFilePath(
            storagePath + "/caffeFiles/gender/gender_net.caffemodel");
    caSettingsGender->setTrainingJsonDataPath(
            storagePath + "/data/FaceJSON/trainingData_FaceData2_genderModel.json");
    caSettingsGender->setTestingJsonDataPath(
            storagePath + "/data/FaceJSON/testingData_FaceData2_genderModel.json");
    caSettingsGender->setIsTrained(false);
    caSettingsGender->setIsTested(false);
    caSettingsGender->setDATKSaveModelPath(
            storagePath + "/data/FaceData2/FaceData2_genderModel.datk");
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
    caSettingsGenderVGG->setTrainingDataPath(storagePath + "/data/FaceData2/Train");
    caSettingsGenderVGG->setTestingDataPath(storagePath + "/data/FaceData2/Test");
    caSettingsGenderVGG->setLabelFilePath(
            storagePath + "/caffeFiles/faceCustomAnalyticslabels.txt");
    caSettingsGenderVGG->setCaffeNetworkFilePath(
            storagePath + "/caffeFiles/gender_VGG/gender.prototxt");
    caSettingsGenderVGG->setCaffeTrainedFilePath(
            storagePath + "/caffeFiles/gender_VGG/gender.caffemodel");
    caSettingsGenderVGG->setTrainingJsonDataPath(
            storagePath + "/data/FaceJSON/trainingData_FaceData2_genderVGGModel.json");
    caSettingsGenderVGG->setTestingJsonDataPath(
            storagePath + "/data/FaceJSON/testingData_FaceData2_genderVGGModel.json");
    caSettingsGenderVGG->setIsTrained(false);
    caSettingsGenderVGG->setIsTested(false);
    caSettingsGenderVGG->setDATKSaveModelPath(
            storagePath + "/data/FaceData2/FaceData2_genderVGGModel.datk");
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
    caSettingsGenderGooglenet->setTrainingDataPath(storagePath + "/data/FaceData2/Train");
    caSettingsGenderGooglenet->setTestingDataPath(storagePath + "/data/FaceData2/Test");
    caSettingsGenderGooglenet->setLabelFilePath(
            storagePath + "/caffeFiles/faceCustomAnalyticslabels.txt");
    caSettingsGenderGooglenet->setCaffeNetworkFilePath(
            storagePath + "/caffeFiles/gender_GoogleNet/gender.prototxt");
    caSettingsGenderGooglenet->setCaffeTrainedFilePath(
            storagePath + "/caffeFiles/gender_GoogleNet/gender.caffemodel");
    caSettingsGenderGooglenet->setTrainingJsonDataPath(
            storagePath + "/data/FaceJSON/trainingData_FaceData2_genderGoogleNetModel.json");
    caSettingsGenderGooglenet->setTestingJsonDataPath(
            storagePath + "/data/FaceJSON/testingData_FaceData2_genderGoogleNetModel.json");
    caSettingsGenderGooglenet->setIsTrained(false);
    caSettingsGenderGooglenet->setIsTested(false);
    caSettingsGenderGooglenet->setDATKSaveModelPath(
            storagePath + "/data/FaceData2/FaceData2_genderGoogleNetModel.datk");
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
    caSettingsFace->setTrainingDataPath(storagePath + "/data/FaceData2/Train");
    caSettingsFace->setTestingDataPath(storagePath + "/data/FaceData2/Test");
    caSettingsFace->setLabelFilePath(
            storagePath + "/caffeFiles/faceCustomAnalyticslabels.txt");
    caSettingsFace->setCaffeNetworkFilePath(
            storagePath + "/caffeFiles/face/VGG_FACE_deploy.prototxt");
    caSettingsFace->setCaffeTrainedFilePath(
            storagePath + "/caffeFiles/face/VGG_FACE.caffemodel");
    caSettingsFace->setTrainingJsonDataPath(
            storagePath + "/data/FaceJSON/trainingData_FaceData2_faceModel.json");
    caSettingsFace->setTestingJsonDataPath(
            storagePath + "/data/FaceJSON/testingData_FaceData2_faceModel.json");
    caSettingsFace->setIsTrained(false);
    caSettingsFace->setIsTested(false);
    caSettingsFace->setDATKSaveModelPath(
            storagePath + "/data/FaceData2/FaceData2_faceModel.datk");
    caSettingsFace->setMLAlgo("LRL2");
    caSettingsFace->saveSettings();
    aiSaac::CustomAnalytics *caFace =
            new aiSaac::CustomAnalytics(caSettingsFace);
    std::cout << "Training " << "face " << "model" << std::endl;
    caFace->train();
    std::cout << "Testing " << "face " << "model" << std::endl;
    caFace->test();

    return 0;
}
