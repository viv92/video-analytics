#include <iostream>
#include <dirent.h>
#include <vector>
#include <fstream>
#include "datk.h"
#include <memory>
#include "../src/utils/json.hpp"

int unknownCorrect = 0;
int unknownTotal = 0;

void sparsifyFeatures(std::vector<float> &featureVector, datk::SparseFeature &s) {
    s.featureIndex = std::vector<int>();
    s.featureVec = std::vector<double>();
    s.index = 0;
    for (int i = 0; i < featureVector.size(); i++) {
        if (featureVector[i] != 0) {
            s.featureIndex.push_back(i);
            s.featureVec.push_back(featureVector[i]);
        }
    }
    s.numFeatures = featureVector.size();
    s.numUniqueFeatures = s.featureVec.size();
}

double predictAccuracy(
    datk::Classifiers<datk::SparseFeature> *model,
    std::vector<datk::SparseFeature>& testFeatures,
    datk::Vector& ytest,
    float threshold) {
    // assert(testFeatures.size() == ytest.size());
    double accuracy = 0;
    unknownCorrect = 0;
    unknownTotal = 0;
    for (int i = 0; i < testFeatures.size(); i++) {
        datk::Vector predictions;
        model->predictProbability(testFeatures[i], predictions);
        // int prediction = this->model->predict(s);
        // if (prediction == -1)
        //     prediction = 0;
        int predictionMax = datk::argMax(predictions);
        if (predictions.size() == 2) {
            if (predictionMax == 0) {
                predictionMax = -1;
            }
        }
        // std::cout << "Predicted " << ytest[i] << " as " << predictionMax
        //     << " with probability " << predictionMax << std::endl << std::flush;
        if (ytest[i] == -2) {
            unknownTotal++;
        }
        if (predictions[predictionMax] >= threshold) {
            if (predictionMax == ytest[i]) {
                accuracy++;
            }
        }
        else if (ytest[i] == -2) {
            unknownCorrect++;
            accuracy++;
        }
    }
    return accuracy;
}

std::string testGridModel(
    datk::Classifiers<datk::SparseFeature> *model,
    std::vector<datk::SparseFeature> trainingFeatures,
    datk::Vector trainingLabels,
    std::vector<datk::SparseFeature> testingFeatures,
    datk::Vector testingLabels) {

    int testCorrectMax = 0;
    float testCorrectThreshold = 0;
    int testUnknownCorrect = 0;
    int testUnknownTotal = 0;
    for (float threshold = 0; threshold <= 1 ; threshold = threshold + 0.05) {
        int testAccuracy = predictAccuracy(model, testingFeatures, testingLabels, threshold);
        if (testAccuracy >= testCorrectMax) {
            testCorrectMax = testAccuracy;
            testCorrectThreshold = threshold;
            testUnknownCorrect = unknownCorrect;
            testUnknownTotal = unknownTotal;
        }
    }
    int trainCorrect = predictAccuracy(model, trainingFeatures, trainingLabels, testCorrectThreshold);
    std::string result = "=" + std::to_string(trainCorrect) + "/"
        + std::to_string(trainingLabels.size()) + ",="
        + std::to_string(testCorrectMax) + "/"
        + std::to_string(testingLabels.size()) + ","
        + std::to_string(testCorrectThreshold) + ",="
        + std::to_string(testUnknownCorrect) + "/"
        + std::to_string(testUnknownTotal) + ",="
        + std::to_string(testCorrectMax - testUnknownCorrect) + "/"
        + std::to_string(testingLabels.size() - testUnknownTotal);

    std::cout << "Returning " << result << std::endl << std::flush;
    return result;
}

void loadData(std::string filePath, nlohmann::json &jsonData) {
    std::ifstream dataFile;
    dataFile.open(filePath);
    dataFile >> jsonData;
}

int main(int argc, char *argv[]) {
    if (argc < 12) {
        std::cout << "usage: ./testFaceDatasetSizeAccuracies [trainFeatureJsonPath] "
            << "[testFeatureJsonPath] [outputFolder] [initalNumberOfPeople] "
            << "[peopleIncrement] [finalNumberOfPeople] [initalNumberOfImages] "
            << "[imagesIncrement] [finalNumberOfImages] "
            << "[boostUnknown(0-Same no of test per class, 1-Unknown = known, 2- All unknown)/0]] "
            << "[runGridSearch(1/0)] [threshold(if runGridSearch=0)]" << std::endl;
        return -1;
    }
    std::string trainFeatureJsonPath = argv[1];
    std::string testFeatureJsonPath = argv[2];
    std::string outputFolder = argv[3];
    int initPeopleCount = std::atoi(argv[4]);
    int peopleIncrement = std::atoi(argv[5]);
    int finalPeopleCount = std::atoi(argv[6]);
    int initImagesCount = std::atoi(argv[7]);
    int imagesIncrement = std::atoi(argv[8]);
    int finalImagesCount = std::atoi(argv[9]);
    int boostUnknown = std::atoi(argv[10]);

    bool runGridSearch = std::atoi(argv[11]);
    float threshold = 0;
    if (!runGridSearch) {
        if (argc < 13) {
            std::cout << "usage: ./testFaceDatasetSizeAccuracies [trainFeatureJsonPath] "
                << "[testFeatureJsonPath] [outputFolder] [initalNumberOfPeople] "
                << "[peopleIncrement] [finalNumberOfPeople] [initalNumberOfImages] "
                << "[imagesIncrement] [finalNumberOfImages] "
                << "[boostUnknown(0-Same no of test per class, 1-Unknown = known, 2- All unknown)/0]] "
                << "[runGridSearch(1/0)] [threshold(if runGridSearch=0)]" << std::endl;
            return -1;
        }
        threshold = std::atof(argv[12]);
    }

    // std::string classifierList[] = {"Svm", "RadialSvm","DecisionTree", "LinearSvm", "LogisticRegression-L1", "LogisticRegression-L2"};
    // std::vector<std::string> classifierList = {"LogisticRegression-L1", "LogisticRegression-L2"};
    std::vector<std::string> classifierList = {"LogisticRegression-L2"};

    std::vector<shared_ptr<ofstream>> csvFiles;
    for (int i = 0; i < classifierList.size(); i++) {
        std::cout << classifierList[i] << std::endl;
        std::string filename = outputFolder + "/" + classifierList[i] + ".csv";
        // fileStream.open(outputFolder + "/" + classifierList[i] + ".csv");
        // fileStream << "Training Features," << trainFeatureJsonPath << std::endl << std::flush;
        // fileStream << "Testing Features," << testFeatureJsonPath << std::endl << std::flush;
        // fileStream << "Number of people,Number of Images per person,Train Accuracy,Test Accuracy,Threshold"
        //     << std::endl << std::flush;
        csvFiles.push_back(make_shared<ofstream>(filename.c_str()));
        *(csvFiles[i]) << "Training Features," << trainFeatureJsonPath << std::endl << std::flush;
        *(csvFiles[i]) << "Testing Features," << testFeatureJsonPath << std::endl << std::flush;
        *(csvFiles[i]) << "runboostUnknown Mode," << boostUnknown << std::endl << std::flush;
        *(csvFiles[i]) << "Number of people,Number of Images per person,Train Accuracy,"
            << "Test Accuracy,Threshold,Unknown Accuracy, Known Accuracy"
            << std::endl << std::flush;
    }

    datk::Classifiers<datk::SparseFeature> *model;

    nlohmann::json rawTrainData;
    nlohmann::json rawTestData;
    loadData(trainFeatureJsonPath, rawTrainData);
    loadData(testFeatureJsonPath, rawTestData);

    for (int maxPeople = initPeopleCount;
        maxPeople <= finalPeopleCount;
        maxPeople = maxPeople + peopleIncrement) {
        for (int maxImages = initImagesCount;
            maxImages <= finalImagesCount;
            maxImages = maxImages + imagesIncrement) {
            std::cout << "Runing for " << std::to_string(maxPeople) << " people and "
                << std::to_string(maxImages)
                << " images " << std::endl << std::flush;

            std::vector<datk::SparseFeature> trainingFeatures = std::vector<datk::SparseFeature>();
            datk::Vector trainingLabels;
            std::vector<std::pair<int, std::string>> trainingStringToIntLabels;
            int peopleCount = 0;
            for (nlohmann::json::iterator classIterator = rawTrainData.begin();
                classIterator != rawTrainData.end();
                classIterator++) {
                if (peopleCount >= maxPeople) {
                    break;
                }
                trainingStringToIntLabels.push_back(std::make_pair(peopleCount,
                    classIterator.key().c_str()));
                int imagesCount = 0;
                for (nlohmann::json::iterator storedFeature = classIterator.value().begin();
                    storedFeature != classIterator.value().end();
                    storedFeature++) {
                    if (imagesCount >= maxImages) {
                        break;
                    }
                    imagesCount++;
                    datk::SparseFeature s;
                    std::vector<float> v = *storedFeature;
                    sparsifyFeatures(v,s);
                    trainingFeatures.push_back(s);
                    trainingLabels.push_back(peopleCount);
                }
                peopleCount++;
            }
            std::cout << "Training Data ready " << std::to_string(trainingFeatures.size())
                << " = " << std::to_string(trainingLabels.size()) << std::endl << std::flush;

            std::vector<datk::SparseFeature> testingFeatures = std::vector<datk::SparseFeature>();
            datk::Vector testingLabels;
            std::vector<std::pair<int, std::string>> testingStringToIntLabels;
            peopleCount = 0;
            int imagesCount = 0;
            for (nlohmann::json::iterator classIterator = rawTestData.begin();
                classIterator != rawTestData.end();
                classIterator++) {
                if (peopleCount >= maxPeople) {
                    break;
                }
                if (classIterator.key() == "Unknown_Classes") {
                    continue;
                }
                testingStringToIntLabels.push_back(std::make_pair(peopleCount,
                    classIterator.key().c_str()));
                imagesCount = 0;
                for (nlohmann::json::iterator storedFeature = classIterator.value().begin();
                    storedFeature != classIterator.value().end();
                    storedFeature++) {
                    imagesCount++;
                    datk::SparseFeature s;
                    std::vector<float> v = *storedFeature;
                    sparsifyFeatures(v,s);
                    testingFeatures.push_back(s);
                    testingLabels.push_back(peopleCount);
                }
                peopleCount++;
            }
            if (boostUnknown == 1) {
                imagesCount = maxPeople * imagesCount;
            }
            else if (boostUnknown == 2){
                imagesCount = rawTestData["Unknown_Classes"].size();
            }
            testingStringToIntLabels.push_back(std::make_pair(-2,
                "Unknown_Classes"));
            for (nlohmann::json::iterator storedFeature = rawTestData["Unknown_Classes"].begin();
                storedFeature != rawTestData["Unknown_Classes"].end();
                storedFeature++) {
                if (imagesCount <= 0) {
                    break;
                }
                imagesCount--;
                datk::SparseFeature s;
                std::vector<float> v = *storedFeature;
                sparsifyFeatures(v,s);
                testingFeatures.push_back(s);
                testingLabels.push_back(-2);
            }
            std::cout << "Testing Data ready " << std::to_string(testingFeatures.size())
                << " = " << std::to_string(testingLabels.size()) << std::endl << std::flush;

            for (int classifier = 0; classifier < classifierList.size(); classifier++) {
                std::cout << "Classifier is " << classifierList[classifier] << std::endl;

                std::cout << "trainingFeatures.size()" << trainingFeatures.size() << std::endl << std::flush;
                std::cout << "trainingLabels.size()" << trainingLabels.size() << std::endl << std::flush;
                std::cout << "trainingFeatures[0].numFeatures" << trainingFeatures[0].numFeatures << std::endl << std::flush;
                std::cout << "trainingFeatures.size()" << trainingFeatures.size() << std::endl << std::flush;
                std::cout << "trainingStringToIntLabels.size()" << trainingStringToIntLabels.size() << std::endl << std::flush;
                std::cout << "this->caSettings->getLRL2RegulatizationParam()" << 0 << std::endl << std::flush;
                std::cout << "this->caSettings->getLRL2OptimizationAlgo()" << 0 << std::endl << std::flush;
                std::cout << "this->caSettings->getLRL2NumOfIterations()" << 1000 << std::endl << std::flush;
                std::cout << "this->caSettings->getLRL2Tolerance()" << 0.01 << std::endl << std::flush;


                if (classifierList[classifier] == "LogisticRegression-L1") {
                    std::cout << "Creating LrL1" << std::endl << std::flush;
                    model = new datk::L1LogisticRegression<datk::SparseFeature>(trainingFeatures,
                        trainingLabels,
                        trainingFeatures[0].numFeatures,
                        trainingFeatures.size(),
                        trainingStringToIntLabels.size(),
                        0,
                        0,
                        1000,
                        0.01);
                }
                if (classifierList[classifier] == "LogisticRegression-L2") {
                    std::cout << "Creating LrL2" << std::endl << std::flush;
                    model = new datk::L2LogisticRegression<datk::SparseFeature>(trainingFeatures,
                        trainingLabels,
                        trainingFeatures[0].numFeatures,
                        trainingFeatures.size(),
                        trainingStringToIntLabels.size(),
                        0,
                        0,
                        1000,
                        0.01);
                }
                std::cout << "Created model" << std::endl << std::flush;
                model->train();
                std::cout << "Trained Model" << std::endl << std::flush;
                if (runGridSearch) {
                    std::string result = testGridModel(model, trainingFeatures, trainingLabels, testingFeatures, testingLabels);
                    *(csvFiles[classifier]) << maxPeople << "," << maxImages
                    << "," << result << std::endl << std::flush;
                }
                else {
                    std::cout << "Finding train accuracy" << std::endl << std::flush;
                    int trainCorrect = predictAccuracy(model, trainingFeatures, trainingLabels, threshold);
                    std::cout << "Finding Test accuracy" << std::endl << std::flush;
                    int testCorrect = predictAccuracy(model, testingFeatures, testingLabels, threshold);
                    std::cout << "Writing  accuracy" << std::endl << std::flush;
                    *(csvFiles[classifier]) << maxPeople << "," << maxImages
                        << ",=" << std::to_string(trainCorrect) << "/"
                        << std::to_string(trainingLabels.size())
                        << ",=" << std::to_string(testCorrect) << "/"
                        << std::to_string(trainingLabels.size())
                        << "," << threshold
                        << ",=" << std::to_string(unknownCorrect) << "/"
                        << std::to_string(unknownTotal)
                        << ",=" << std::to_string(testCorrect - unknownCorrect) << "/"
                        << std::to_string(testingLabels.size() -unknownTotal)
                        << std::endl << std::flush;
                }
            }



        }
    }
    return 0;
}
