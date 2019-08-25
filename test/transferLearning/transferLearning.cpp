#include <iostream>
#include <dirent.h>
#include <vector>
#include <fstream>
#include "../../src/utils/caffeClassifier.h"
#include "../../src/utils/json.hpp"
#include "datk.h"

template <class Feature>
double predictAccuracy(datk::Classifiers<Feature>* c, std::vector<Feature>& testFeatures, datk::Vector& ytest){
	assert(testFeatures.size() == ytest.size());
	double accuracy = 0;
	for (int i = 0; i < testFeatures.size(); i++){
        int prediction = c->predict(testFeatures[i]);
        // std::cout << "truth: " << ytest[i] << " - prediction: " << prediction << std::endl;
		if (prediction == ytest[i])
			accuracy++;
	}
	return accuracy;
}

int main(int argc, char *argv[]) {
    // usage: ./transferLearning [deploy_network.prototxt] [weights.caffemodel] [trainingDataDir] [testingDataDir] [datkModelPath] [trainingFeatureJson] [testingFeatureJson]
    aiSaac::CaffeClassifier caffeClassifier(argv[1], argv[2]);
    // ::google::InitGoogleLogging(argv[0]);
    nlohmann::json trainingData;
    nlohmann::json testingData;
    if (argc == 8) {
        std::ifstream f(argv[6]);
        if (f.good()) {
            std::cout << "Loading training" << std::endl;
            std::ifstream trainingFile;
            trainingFile.open(argv[6]);
            trainingFile >> trainingData;
        } else {
            std::cout << "Sorry the file " << argv[6] << "does not have the training data required" << std::endl;
            return -1;
        }

        std::ifstream f2(argv[7]);
        if (f2.good()) {
            std::cout << "Loading testing" << std::endl;
            std::ifstream testingFile;
            testingFile.open(argv[7]);
            testingFile >> testingData;
        } else {
            std::cout << "Sorry the file " << argv[7] << "does not have the training data required" << std::endl;
            return -1;
        }
    } else {
        std::cout << "No training or testing file found. Creating..." << std::endl;
        std::ofstream trainingFile;
        trainingFile.open("trainingData.json");
        trainingFile << trainingData.dump(4);
        trainingFile.close();

        std::ofstream testingFile;
        testingFile.open("testingData.json");
        testingFile << testingData.dump(4);
        testingFile.close();

        std::cout << "created the files" << std::endl;
    }

    std::vector<std::vector<float>> featureVectors;
    datk::Vector intLabels;
    datk::Vector testIntLabels;
    std::vector<std::pair<int, std::string>> stringToInt;
    std::vector<std::vector<float>> testFeatureVectors;
    std::vector<std::pair<int, std::string>> testStringToInt;


    if (argc == 8) {
        double intLabel = 0;
        for (nlohmann::json::iterator person = trainingData.begin();
            person != trainingData.end();
            person++) {

            std::cout << "person.key: " << person.key() << std::endl;
            stringToInt.push_back(std::make_pair(intLabel, person.key().c_str()));
            for (nlohmann::json::iterator storedFace = person.value().begin();
                storedFace != person.value().end();
                storedFace++) {
                std::vector<float> storedFeature = *storedFace;
                featureVectors.push_back(storedFeature);
                std::cout << "about to store label" << std::endl;
                intLabels.push_back(intLabel);
            }
            intLabel++;
        }

        double testIntLabel = 0;
        for (nlohmann::json::iterator person = testingData.begin();
            person != testingData.end();
            person++) {

            testStringToInt.push_back(std::make_pair(testIntLabel, person.key().c_str()));
            for (nlohmann::json::iterator storedFace = person.value().begin();
                storedFace != person.value().end();
                storedFace++) {
                std::vector<float> storedFeature = *storedFace;
                testFeatureVectors.push_back(storedFeature);
                testIntLabels.push_back(testIntLabel);
            }
            testIntLabel++;
        }
        std::cout << "size of training: " << intLabels.size() << std::endl;
        std::cout << "size of testing: " << testIntLabels.size() << std::endl;
    } else {
        DIR *trainingFaceStorageDir;
        struct dirent *direntTrainingFaceStorageDir;
        DIR *trainingPersonPath;
        struct dirent *direntTrainingPersonPath;
        trainingFaceStorageDir = opendir(argv[3]);
        std::string trainingPersonFolder;
        std::string trainingPersonName;
        std::string trainingFaceImagePath;

        int count = 0;

        while((direntTrainingFaceStorageDir = readdir(trainingFaceStorageDir)) != NULL) {
            // std::cout << direntTrainingFaceStorageDir->d_name;
            trainingPersonFolder = argv[3];
            if (direntTrainingFaceStorageDir->d_type == DT_DIR
                && std::strcmp(direntTrainingFaceStorageDir->d_name, ".")
                && std::strcmp(direntTrainingFaceStorageDir->d_name, "..")) {
                trainingPersonName = direntTrainingFaceStorageDir->d_name;
                trainingPersonFolder = trainingPersonFolder + "/" + direntTrainingFaceStorageDir->d_name;
                trainingPersonPath = opendir(trainingPersonFolder.c_str());
                int currentIntLabel = count;
                stringToInt.push_back(std::make_pair(currentIntLabel, trainingPersonName));
                count++;
                while ((direntTrainingPersonPath = readdir(trainingPersonPath))) {
                    if (direntTrainingPersonPath->d_type == DT_REG) {
                        std::string fileName(direntTrainingPersonPath->d_name);
                        if (fileName.substr(fileName.find_last_of(".") + 1) != "DS_Store") {
                            trainingFaceImagePath = trainingPersonFolder + "/" + fileName;
                            std::cout << "final face image path: " << trainingFaceImagePath << std::endl;
                            std::vector<float> currentFeatureVector =
                                caffeClassifier.Predict(cv::imread(trainingFaceImagePath), "fc7");
                            trainingData[trainingPersonName].push_back(currentFeatureVector);
                            featureVectors.push_back(currentFeatureVector);
                            intLabels.push_back(currentIntLabel);
                        } else {
                            std::cout << "Ignoring " << fileName << std::endl;
                        }
                    }
                }
            }
        }

        std::ofstream trainingFile;
        trainingFile.open("trainingData.json");
        trainingFile << trainingData.dump(4);
        trainingFile.close();

        DIR *testFaceStorageDir;
        struct dirent *direntTestFaceStorageDir;
        DIR *testPersonPath;
        struct dirent *direntTestPersonPath;
        testFaceStorageDir = opendir(argv[4]);
        std::string testPersonFolder;
        std::string testPersonName;
        std::string testFaceImagePath;

        int testCount = 0;

        while((direntTestFaceStorageDir = readdir(testFaceStorageDir)) != NULL) {
            // std::cout << direntTestFaceStorageDir->d_name;
            testPersonFolder = argv[4];
            if (direntTestFaceStorageDir->d_type == DT_DIR
                && std::strcmp(direntTestFaceStorageDir->d_name, ".")
                && std::strcmp(direntTestFaceStorageDir->d_name, "..")) {
                testPersonName = direntTestFaceStorageDir->d_name;
                testPersonFolder = testPersonFolder + "/" + direntTestFaceStorageDir->d_name;
                testPersonPath = opendir(testPersonFolder.c_str());
                int currentTestIntLabel = testCount;
                testStringToInt.push_back(std::make_pair(currentTestIntLabel, testPersonName));
                testCount++;
                while ((direntTestPersonPath = readdir(testPersonPath))) {
                    if (direntTestPersonPath->d_type == DT_REG) {
                        std::string fileName(direntTestPersonPath->d_name);
                        if (fileName.substr(fileName.find_last_of(".") + 1) != "DS_Store") {
                            testFaceImagePath = testPersonFolder + "/" + fileName;
                            std::cout << "final face image path: " << testFaceImagePath << std::endl;
                            std::vector<float> currentTestFeatureVector =
                                caffeClassifier.Predict(cv::imread(testFaceImagePath), "fc7");
                                testingData[testPersonName].push_back(currentTestFeatureVector);
                            testFeatureVectors.push_back(currentTestFeatureVector);
                            testIntLabels.push_back(currentTestIntLabel);
                        } else {
                            std::cout << "Ignoring " << fileName << std::endl;
                        }
                    }
                }
            }
        }

        std::ofstream testingFile;
        testingFile.open("testingData.json");
        testingFile << testingData.dump(4);
        testingFile.close();
    }


    std::vector<datk::SparseFeature> sparseFeatures = std::vector<datk::SparseFeature>();
    for (int i = 0; i < featureVectors.size(); i++) {
        std::cout << "feature: " << featureVectors[i].size() << " - label: " << intLabels[i] << std::endl;
        datk::SparseFeature s;
        s.featureIndex = std::vector<int>();
        s.featureVec = std::vector<double>();
        s.index = i;
        for (int j = 0; j < featureVectors[i].size(); j++) {
            if (featureVectors[i][j] != 0) {
                s.featureIndex.push_back(j);
                s.featureVec.push_back(featureVectors[i][j]);
            }
        }
        s.numFeatures = featureVectors[i].size();
        // mtrain = s.numFeatures;
        s.numUniqueFeatures = s.featureVec.size();
        sparseFeatures.push_back(s);
    }
    // for (int i = 0; i < stringToInt.size(); i++) {
    //     std::cout << "Label: " << stringToInt[i].first << " - labelInt: " << stringToInt[i].second << std::endl;
    // }

    std::vector<datk::SparseFeature> testSparseFeatures = std::vector<datk::SparseFeature>();
    for (int i = 0; i < testFeatureVectors.size(); i++) {
        std::cout << "feature: " << testFeatureVectors[i].size() << " - label: " << testIntLabels[i] << std::endl;
        datk::SparseFeature s;
        s.featureIndex = std::vector<int>();
        s.featureVec = std::vector<double>();
        s.index = i;
        for (int j = 0; j < testFeatureVectors[i].size(); j++) {
            if (testFeatureVectors[i][j] != 0) {
                s.featureIndex.push_back(j);
                s.featureVec.push_back(testFeatureVectors[i][j]);
            }
        }
        s.numFeatures = testFeatureVectors[i].size();
        s.numUniqueFeatures = s.featureVec.size();
        testSparseFeatures.push_back(s);
    }
    if (stringToInt.size() == 2) {
        for (int i = 0; i < intLabels.size(); i++) {
          intLabels[i] = 2*intLabels[i] - 1;
        }
        for (int i = 0; i < testIntLabels.size(); i++) {
          testIntLabels[i] = 2*testIntLabels[i] - 1;
        }
    }
    // int lambda = 20000;
    std::cout << "doing logistic regression" << std::endl;
    double accuracy;
    double accuracy_percentage;
    for (int lambda = 0; lambda < 10; lambda += 1) {
        datk::L2LogisticRegression<datk::SparseFeature> lr =
            datk::L2LogisticRegression<datk::SparseFeature>(sparseFeatures,
                intLabels,
                featureVectors[0].size(),
                featureVectors.size(),
                stringToInt.size(),
                (double)lambda,
                0,
                1000,
                1e-2);
        lr.train();
        accuracy = predictAccuracy(&lr, testSparseFeatures, testIntLabels);
        accuracy_percentage = accuracy/testIntLabels.size();
        cout << "The test acuracy of the classifier with lambda = " << lambda << " is "<< accuracy_percentage << "("<< accuracy << "/"<< testIntLabels.size() << ")" << "\n";
        accuracy = predictAccuracy(&lr, sparseFeatures, intLabels);
        accuracy_percentage = accuracy/intLabels.size();
        cout << "The train acuracy of the classifier with lambda =  "<< lambda << "is " << accuracy_percentage << "("<< accuracy << "/"<< intLabels.size() << ")" << "\n";
    }
    int lambda = 1;
    //std::cout << "Saving the Logistic Regression Model\n";
    const char* modelnamelr;
    string str;
    str = "";
    str += argv[5];
    str += "LR.model";
    modelnamelr = str.c_str();
    std::cout << modelnamelr << "\n" << std::flush;
    //lr.saveModel(modelnamelr);
    std::cout << "Hinge SVM" << std::endl;
    lambda = 1;
    std::cout << stringToInt.size() << "\n";
    // for (double lambda = 0; lambda < 2; lambda+=0.10) {
    datk::L2HingeSVM<datk::SparseFeature> lsvm =
        datk::L2HingeSVM<datk::SparseFeature>(sparseFeatures,
            intLabels,
            featureVectors[0].size(),
            featureVectors.size(),
            stringToInt.size(),
            lambda,
            0,
            1000,
            1e-2);
    lsvm.train();
    accuracy = predictAccuracy(&lsvm, testSparseFeatures, testIntLabels);
    accuracy_percentage = accuracy/testIntLabels.size();
    cout << "The test acuracy of the classifier with lambda = " << lambda << " is "<< accuracy_percentage << "("<< accuracy << "/"<< testIntLabels.size() << ")" << "\n";
    accuracy = predictAccuracy(&lsvm, sparseFeatures, intLabels);
    accuracy_percentage = accuracy/intLabels.size();
    cout << "The train acuracy of the classifier with lambda =  "<< lambda << "is " << accuracy_percentage << "("<< accuracy << "/"<< intLabels.size() << ")" << "\n";
    //}
    /*std::cout << "Saving the Hinge SVM Model\n";
    const char* modelnamehsvm;
    str = "";
    str += argv[5];
    str += "HSVM.model";
    modelnamehsvm = str.c_str();
    std::cout << modelnamehsvm << "\n" << std::flush;
    lsvm.saveModel(modelnamehsvm);*/
    std::cout << "Smooth SVM" << std::endl;
    lambda = 1;
    std::cout << stringToInt.size() << "\n";
    // for (double lambda = 0; lambda < 2; lambda+=0.10) {
    datk::L2SmoothSVM<datk::SparseFeature> lssvm =
        datk::L2SmoothSVM<datk::SparseFeature>(sparseFeatures,
            intLabels,
            featureVectors[0].size(),
            featureVectors.size(),
            stringToInt.size(),
            lambda,
            1,
            1000,
            1e-2);
    lssvm.train();
    accuracy = predictAccuracy(&lssvm, testSparseFeatures, testIntLabels);
    accuracy_percentage = accuracy/testIntLabels.size();
    cout << "The test acuracy of the classifier with lambda = " << lambda << " is "<< accuracy_percentage << "("<< accuracy << "/"<< testIntLabels.size() << ")" << "\n";
    accuracy = predictAccuracy(&lssvm, sparseFeatures, intLabels);
    accuracy_percentage = accuracy/intLabels.size();
    cout << "The train acuracy of the classifier with lambda =  "<< lambda << "is " << accuracy_percentage << "("<< accuracy << "/"<< intLabels.size() << ")" << "\n";
    /*const char* modelnamessvm;
    str = "";
    str += argv[5];
    str += "SSVM.model";
    modelnamessvm = str.c_str();
    std::cout << modelnamessvm << "\n" << std::flush;
    lssvm.saveModel(modelnamessvm);*/
    return 0;
}
