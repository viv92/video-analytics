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
    caSettingsAge->setTrainingDataPath(storagePath + "/data/GenderData1/Train");
    caSettingsAge->setTestingDataPath(storagePath + "/data/GenderData1/Test");
    caSettingsAge->setLabelFilePath(
            storagePath + "/caffeFiles/genderCustomAnalyticslabels.txt");
    caSettingsAge->setCaffeNetworkFilePath(
            storagePath + "/caffeFiles/age/deploy_age.prototxt");
    caSettingsAge->setCaffeTrainedFilePath(
            storagePath + "/caffeFiles/age/age_net.caffemodel");
    caSettingsAge->setTrainingJsonDataPath(
            storagePath + "/data/GenderJSON/trainingData_GenderData1_ageModel.json");
    caSettingsAge->setTestingJsonDataPath(
            storagePath + "/data/GenderJSON/testingData_GenderData1_ageModel.json");
    caSettingsAge->setIsTrained(false);
    caSettingsAge->setIsTested(false);
    caSettingsAge->setDATKSaveModelPath(
            storagePath + "/data/GenderData1/GenderData1_ageModel.datk");
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
    caSettingsAgeVGG->setTrainingDataPath(storagePath + "/data/GenderData1/Train");
    caSettingsAgeVGG->setTestingDataPath(storagePath + "/data/GenderData1/Test");
    caSettingsAgeVGG->setLabelFilePath(
            storagePath + "/caffeFiles/genderCustomAnalyticslabels.txt");
    caSettingsAgeVGG->setCaffeNetworkFilePath(
            storagePath + "/caffeFiles/ageVGG/age.prototxt");
    caSettingsAgeVGG->setCaffeTrainedFilePath(
            storagePath + "/caffeFiles/ageVGG/dex_imdb_wiki.caffemodel");
    caSettingsAgeVGG->setTrainingJsonDataPath(
            storagePath + "/data/GenderJSON/trainingData_GenderData1_ageVGGModel.json");
    caSettingsAgeVGG->setTestingJsonDataPath(
            storagePath + "/data/GenderJSON/testingData_GenderData1_ageVGGModel.json");
    caSettingsAgeVGG->setIsTrained(false);
    caSettingsAgeVGG->setIsTested(false);
    caSettingsAgeVGG->setDATKSaveModelPath(
            storagePath + "/data/GenderData1/GenderData1_ageVGGModel.datk");
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
    caSettingsGender->setTrainingDataPath(storagePath + "/data/GenderData1/Train");
    caSettingsGender->setTestingDataPath(storagePath + "/data/GenderData1/Test");
    caSettingsGender->setLabelFilePath(
            storagePath + "/caffeFiles/genderCustomAnalyticslabels.txt");
    caSettingsGender->setCaffeNetworkFilePath(
            storagePath + "/caffeFiles/gender/deploy_gender.prototxt");
    caSettingsGender->setCaffeTrainedFilePath(
            storagePath + "/caffeFiles/gender/gender_net.caffemodel");
    caSettingsGender->setTrainingJsonDataPath(
            storagePath + "/data/GenderJSON/trainingData_GenderData1_genderModel.json");
    caSettingsGender->setTestingJsonDataPath(
            storagePath + "/data/GenderJSON/testingData_GenderData1_genderModel.json");
    caSettingsGender->setIsTrained(false);
    caSettingsGender->setIsTested(false);
    caSettingsGender->setDATKSaveModelPath(
            storagePath + "/data/GenderData1/GenderData1_genderModel.datk");
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
    caSettingsGenderVGG->setTrainingDataPath(storagePath + "/data/GenderData1/Train");
    caSettingsGenderVGG->setTestingDataPath(storagePath + "/data/GenderData1/Test");
    caSettingsGenderVGG->setLabelFilePath(
            storagePath + "/caffeFiles/genderCustomAnalyticslabels.txt");
    caSettingsGenderVGG->setCaffeNetworkFilePath(
            storagePath + "/caffeFiles/gender_VGG/gender.prototxt");
    caSettingsGenderVGG->setCaffeTrainedFilePath(
            storagePath + "/caffeFiles/gender_VGG/gender.caffemodel");
    caSettingsGenderVGG->setTrainingJsonDataPath(
            storagePath + "/data/GenderJSON/trainingData_GenderData1_genderVGGModel.json");
    caSettingsGenderVGG->setTestingJsonDataPath(
            storagePath + "/data/GenderJSON/testingData_GenderData1_genderVGGModel.json");
    caSettingsGenderVGG->setIsTrained(false);
    caSettingsGenderVGG->setIsTested(false);
    caSettingsGenderVGG->setDATKSaveModelPath(
            storagePath + "/data/GenderData1/GenderData1_genderVGGModel.datk");
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
    caSettingsGenderGooglenet->setTrainingDataPath(storagePath + "/data/GenderData1/Train");
    caSettingsGenderGooglenet->setTestingDataPath(storagePath + "/data/GenderData1/Test");
    caSettingsGenderGooglenet->setLabelFilePath(
            storagePath + "/caffeFiles/genderCustomAnalyticslabels.txt");
    caSettingsGenderGooglenet->setCaffeNetworkFilePath(
            storagePath + "/caffeFiles/gender_GoogleNet/gender.prototxt");
    caSettingsGenderGooglenet->setCaffeTrainedFilePath(
            storagePath + "/caffeFiles/gender_GoogleNet/gender.caffemodel");
    caSettingsGenderGooglenet->setTrainingJsonDataPath(
            storagePath + "/data/GenderJSON/trainingData_GenderData1_genderGoogleNetModel.json");
    caSettingsGenderGooglenet->setTestingJsonDataPath(
            storagePath + "/data/GenderJSON/testingData_GenderData1_genderGoogleNetModel.json");
    caSettingsGenderGooglenet->setIsTrained(false);
    caSettingsGenderGooglenet->setIsTested(false);
    caSettingsGenderGooglenet->setDATKSaveModelPath(
            storagePath + "/data/GenderData1/GenderData1_genderGoogleNetModel.datk");
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
    caSettingsFace->setTrainingDataPath(storagePath + "/data/GenderData1/Train");
    caSettingsFace->setTestingDataPath(storagePath + "/data/GenderData1/Test");
    caSettingsFace->setLabelFilePath(
            storagePath + "/caffeFiles/genderCustomAnalyticslabels.txt");
    caSettingsFace->setCaffeNetworkFilePath(
            storagePath + "/caffeFiles/face/VGG_FACE_deploy.prototxt");
    caSettingsFace->setCaffeTrainedFilePath(
            storagePath + "/caffeFiles/face/VGG_FACE.caffemodel");
    caSettingsFace->setTrainingJsonDataPath(
            storagePath + "/data/GenderJSON/trainingData_GenderData1_faceModel.json");
    caSettingsFace->setTestingJsonDataPath(
            storagePath + "/data/GenderJSON/testingData_GenderData1_faceModel.json");
    caSettingsFace->setIsTrained(false);
    caSettingsFace->setIsTested(false);
    caSettingsFace->setDATKSaveModelPath(
            storagePath + "/data/GenderData1/GenderData1_faceModel.datk");
    caSettingsFace->setMLAlgo("LRL2");
    caSettingsFace->saveSettings();
    aiSaac::CustomAnalytics *caFace =
            new aiSaac::CustomAnalytics(caSettingsFace);
    std::cout << "Training " << "face " << "model" << std::endl;
    caFace->train();
    std::cout << "Testing " << "face " << "model" << std::endl;
    caFace->test();

    aiSaac::AiSaacSettings *aiSaacSettingsGenderModel =
            new aiSaac::AiSaacSettings(storagePath, "configGenderModel");
    aiSaacSettingsGenderModel->setGenderPrototxtPath(
            storagePath + "/caffeFiles/gender/deploy_gender.prototxt");
    aiSaacSettingsGenderModel->setGenderCaffemodelPath(
            storagePath + "/caffeFiles/gender/gender_net.caffemodel");
    aiSaacSettingsGenderModel->setGenderMeanFilePath(
            storagePath + "/caffeFiles/gender/mean.binaryproto");
    aiSaacSettingsGenderModel->setGenderLabelFilePath(
            storagePath + "/caffeFiles/gender/gender_synset_words.txt");
    aiSaacSettingsGenderModel->saveSettings();

    aiSaac::AiSaacSettings *aiSaacSettingsGenderVGGModel =
            new aiSaac::AiSaacSettings(storagePath, "configGenderVGGModel");
    aiSaacSettingsGenderVGGModel->setGenderPrototxtPath(
            storagePath + "/caffeFiles/gender_VGG/gender.prototxt");
    aiSaacSettingsGenderVGGModel->setGenderCaffemodelPath(
            storagePath + "/caffeFiles/gender_VGG/gender.caffemodel");
    aiSaacSettingsGenderVGGModel->setGenderMeanFilePath(
            storagePath + "/caffeFiles/gender_VGG/mean.binaryproto");
    aiSaacSettingsGenderVGGModel->setGenderLabelFilePath(
            storagePath + "/caffeFiles/gender_VGG/gender_synset_words.txt");
    aiSaacSettingsGenderVGGModel->saveSettings();

    aiSaac::AiSaacSettings *aiSaacSettingsGenderGooglenetModel =
            new aiSaac::AiSaacSettings(storagePath, "configGenderGooglenetModel");
    aiSaacSettingsGenderGooglenetModel->setGenderPrototxtPath(
            storagePath + "/caffeFiles/gender_GoogleNet/gender.prototxt");
    aiSaacSettingsGenderGooglenetModel->setGenderCaffemodelPath(
            storagePath + "/caffeFiles/gender_GoogleNet/gender.caffemodel");
    aiSaacSettingsGenderGooglenetModel->setGenderMeanFilePath(
            storagePath + "/caffeFiles/gender_GoogleNet/mean.binaryproto");
    aiSaacSettingsGenderGooglenetModel->setGenderLabelFilePath(
            storagePath + "/caffeFiles/gender_GoogleNet/gender_synset_words.txt");
    aiSaacSettingsGenderGooglenetModel->saveSettings();



    std::cout << "CALLING pretrained models" << std::endl;


    aiSaac::GenderRecognition *genderAnalyticsGenderModel =
            new aiSaac::GenderRecognition(aiSaacSettingsGenderModel);
    genderAnalyticsGenderModel->initialize();
    aiSaac::GenderRecognition *genderAnalyticsGenderVGGModel =
            new aiSaac::GenderRecognition(aiSaacSettingsGenderVGGModel);
    genderAnalyticsGenderVGGModel->initialize();
    aiSaac::GenderRecognition *genderAnalyticsGenderGooglenetModel =
            new aiSaac::GenderRecognition(aiSaacSettingsGenderGooglenetModel);
    genderAnalyticsGenderGooglenetModel->initialize();

    int totalTestedFaces = 0;
    int totalCorrectFacesGenderModel = 0;
    int totalIncorrectFacesGenderModel = 0;
    int totalCorrectFacesGenderVGGModel = 0;
    int totalIncorrectFacesGenderVGGModel = 0;
    int totalCorrectFacesGenderGooglenetModel = 0;
    int totalIncorrectFacesGenderGooglenetModel = 0;

    std::string folderPath = storagePath + "/data/GenderData1/Test";
    DIR *faceStorageDir;
    struct dirent *direntFaceStorageDir;
    DIR *genderDir;
    struct dirent *direntGenderDir;
    std::string genderFolder;
    std::string genderName;

    std::string faceImagePath;
    std::string delimiter = ",";
    faceStorageDir = opendir(folderPath.c_str());
    while((direntFaceStorageDir = readdir(faceStorageDir)) != NULL) {
        if (direntFaceStorageDir->d_type == DT_DIR
            && strcmp(direntFaceStorageDir->d_name, ".")
            && strcmp(direntFaceStorageDir->d_name, "..")) {

            genderName = direntFaceStorageDir->d_name;
            genderFolder = folderPath + "/" + genderName;
            std::cout<< "Gender Folder" << genderFolder << std::endl;
            genderDir = opendir(genderFolder.c_str());
            while((direntGenderDir = readdir(genderDir)) != NULL) {
                if (direntGenderDir->d_type == DT_REG) {
                    std::string fileName(direntGenderDir->d_name);
                    faceImagePath = genderFolder + "/" + fileName;
                    std::cout << "final face image path: " << faceImagePath << std::endl;
                    cv::Mat faceImage = cv::imread(faceImagePath);
                    totalTestedFaces++;

                    std::string genderModelPrediction = genderAnalyticsGenderModel->runAlgo(faceImage);
                    std::cout << "PREDICTION gender model = " << genderModelPrediction << std::endl;
                    if (genderModelPrediction == genderName)
                        totalCorrectFacesGenderModel++;
                    else
                        totalIncorrectFacesGenderModel++;

                    std::string genderVGGModelPrediction = genderAnalyticsGenderVGGModel->runAlgo(faceImage);
                    std::cout << "PREDICTION genderVGG model = " << genderVGGModelPrediction << std::endl;
                    if (genderVGGModelPrediction == genderName)
                        totalCorrectFacesGenderVGGModel++;
                    else
                        totalIncorrectFacesGenderVGGModel++;

                    std::string genderGooglenetModelPrediction = genderAnalyticsGenderGooglenetModel->runAlgo(faceImage);
                    std::cout << "PREDICTION genderGooglenet model = " << genderGooglenetModelPrediction << std::endl;
                    if (genderGooglenetModelPrediction == genderName)
                        totalCorrectFacesGenderGooglenetModel++;
                    else
                        totalIncorrectFacesGenderGooglenetModel++;
                }

            }
        }
    }
    std::cout << "Total faces " << totalTestedFaces << std::endl;
    std::cout << "Gender Model" << std::endl;
    std::cout << "\tCorrect = " << totalCorrectFacesGenderModel
            << " Incorrect =" << totalIncorrectFacesGenderModel << std::endl;
    std::cout << "\tAccuract = " <<
            ((float)totalCorrectFacesGenderModel)/((float)totalTestedFaces) << std::endl;
    std::cout << "gender VGG Model" << std::endl;
    std::cout << "\tCorrect = " << totalCorrectFacesGenderVGGModel
            << " Incorrect =" << totalIncorrectFacesGenderVGGModel << std::endl;
    std::cout << "\tAccuract = " <<
            ((float)totalCorrectFacesGenderVGGModel)/((float)totalTestedFaces) << std::endl;

    std::cout << "gender Googlenet Model" << std::endl;
    std::cout << "\tCorrect = " << totalCorrectFacesGenderGooglenetModel
            << " Incorrect =" << totalIncorrectFacesGenderGooglenetModel << std::endl;
    std::cout << "\tAccuract = " <<
            ((float)totalCorrectFacesGenderGooglenetModel)/((float)totalTestedFaces) << std::endl;

    return 0;
}
