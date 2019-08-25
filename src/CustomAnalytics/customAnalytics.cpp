/*
    Copyright 2016 AITOE
*/

#include "customAnalytics.h"

aiSaac::CustomAnalytics::CustomAnalytics(
    aiSaac::CASettings *caSettings,
    aiSaac::CaffeClassifier *caffeClassifier) {
    this->caSettings = caSettings;
    if (this->caSettings->getMode() == 0) {
        if (caffeClassifier == NULL) {
            this->caffeClassifier = new aiSaac::CaffeClassifier(
                this->caSettings->getCaffeNetworkFilePath(),
                this->caSettings->getCaffeTrainedFilePath());
        }
        else {
            this->caffeClassifier = caffeClassifier;
        }
    }
}

aiSaac::CustomAnalytics::~CustomAnalytics() {}

void aiSaac::CustomAnalytics::loadDatkModel() {
    std::cout << "hello there 1" << std::endl;
    // std::ifstream labelFile(this->caSettings->getLabelFilePath().c_str());
    // if (labelFile.is_open()) {
    //     std::cout << "hello there 2" << std::endl;
    //     std::string label;
    //     int intLabel = 0;
    //     while (!labelFile.eof()) {
    //         labelFile >> label;
    //         this->trainingStringToIntLabels.push_back(std::make_pair(intLabel, label));
    //         intLabel++;
    //     }
    //     std::cout << "hello there 3" << std::endl;
    // }
    // labelFile.close();
    std::cout << "hello there 4" << std::endl;

    if (!this->caSettings->getTrainingJsonDataPath().empty()) {
        std::ifstream f(this->caSettings->getTrainingJsonDataPath());
        if (f.good() && this->caSettings->getIsTrained()) {
            std::cout << "Loading training" << std::endl;
            this->loadData(this->caSettings->getTrainingJsonDataPath(), this->trainingJsonData);
            //double intLabel = 0;
            std::cout << "training Data size: " << this->trainingJsonData.size() << std::endl;
            /*for (nlohmann::json::iterator theClass = this->trainingJsonData.begin();
                theClass != this->trainingJsonData.end();
                theClass++) {
                std::cout << "theClass.key: " << theClass.key() << std::endl;
                this->trainingStringToIntLabels.push_back(std::make_pair(intLabel,
                    theClass.key().c_str()));
                for (nlohmann::json::iterator storedFeature = theClass.value().begin();
                    storedFeature != theClass.value().end();
                    storedFeature++) {
                    this->trainingFeatureVectors.push_back(*storedFeature);
                    this->trainingIntLabels.push_back(intLabel);
                }
                intLabel++;
            }*/

            //get label mapping from label file
            int intLabel = 0;
            std::ifstream labelFile(this->caSettings->getLabelFilePath());
            std::string line;
            while (std::getline(labelFile, line)) {
                std::istringstream iss(line);
                std::string className;
                if (!(iss >> className)) { break; } // error
                this->trainingStringToIntLabels.push_back(std::make_pair(intLabel,className));
                intLabel++;
            }

            std::cout << "training Data size: " << this->trainingJsonData.size() << std::endl;
            for (nlohmann::json::iterator theClass = this->trainingJsonData.begin();
                theClass != this->trainingJsonData.end();
                theClass++) {
                std::cout << "theClass.key: " << theClass.key() << std::endl;
                for (std::vector< std::pair<int, std::string> >::iterator it = this->trainingStringToIntLabels.begin();
                    it != this->trainingStringToIntLabels.end();) {
                    if(it->second == theClass.key().c_str()) {
                        intLabel = it->first;
                        break;
                    }
                    ++it;
                }
                std::cout << "theClass intLabel: " << intLabel << std::endl;
                for (nlohmann::json::iterator storedFeature = theClass.value().begin();
                    storedFeature != theClass.value().end();
                    storedFeature++) {
                    this->trainingFeatureVectors.push_back(*storedFeature);
                    this->trainingIntLabels.push_back(intLabel);
                }
            }

        } else {
            std::cout << "Sorry the training file "
                << this->caSettings->getTrainingJsonDataPath()
                << " does not exist/not trained and will be created"
                << std::endl;
            this->extractFeatures(this->caSettings->getTrainingDataPath(),
                this->trainingFeatureVectors,
                this->trainingIntLabels,
                this->trainingStringToIntLabels,
                this->trainingJsonData,
                true);
            std::cout << "extracted all features.. going to save to JSON: " << this->trainingJsonData.size() << std::endl << std::flush;
            this->saveData(this->caSettings->getTrainingJsonDataPath(), this->trainingJsonData);
        }
    } else {
        std::cout << "You dont want to store the training data" << std::endl;
        this->extractFeatures(this->caSettings->getTrainingDataPath(),
            this->trainingFeatureVectors,
            this->trainingIntLabels,
            this->trainingStringToIntLabels,
            this->trainingJsonData);
    }
    std::cout << "creating sparse feature vector" << std::endl;
    std::vector<datk::SparseFeature> sparseFeatures = std::vector<datk::SparseFeature>();
    for (int i = 0; i < this->trainingFeatureVectors.size(); i++) {
        std::cout << "feature: " << this->trainingFeatureVectors[i].size() << " - label: "
            << this->trainingIntLabels[i] << std::endl;
        datk::SparseFeature s;
        s.featureIndex = std::vector<int>();
        s.featureVec = std::vector<double>();
        s.index = i;
        for (int j = 0; j < this->trainingFeatureVectors[i].size(); j++) {
            if (this->trainingFeatureVectors[i][j] != 0) {
                s.featureIndex.push_back(j);
                s.featureVec.push_back(this->trainingFeatureVectors[i][j]);
            }
        }
        s.numFeatures = this->trainingFeatureVectors[i].size();
        s.numUniqueFeatures = s.featureVec.size();
        sparseFeatures.push_back(s);
    }
    std::cout << "sparseFeatures.size()" << sparseFeatures.size() << std::endl << std::flush;

    if (this->trainingStringToIntLabels.size() == 2) {
        for (int i = 0; i < this->trainingIntLabels.size(); i++) {
            this->trainingIntLabels[i] = 2 * this->trainingIntLabels[i] - 1;
        }
    }

    if (this->caSettings->getMLAlgo() == "LRL1") {
        this->model = new datk::L1LogisticRegression<datk::SparseFeature>(sparseFeatures,
            this->trainingIntLabels,
            this->trainingFeatureVectors[0].size(),
            this->trainingFeatureVectors.size(),
            this->trainingStringToIntLabels.size(),
            this->caSettings->getLRL1RegulatizationParam(),
            this->caSettings->getLRL1OptimizationAlgo(),
            this->caSettings->getLRL1NumOfIterations(),
            this->caSettings->getLRL1Tolerance());
    } else if (this->caSettings->getMLAlgo() == "LRL2") {
        std::cout << "hello there :)" << std::endl << std::flush;

        std::cout << "trainingIntLabels.size()" << trainingIntLabels.size() << std::endl << std::flush;
        std::cout << "trainingFeatureVectors[0].size()" << trainingFeatureVectors[0].size() << std::endl << std::flush;
        std::cout << "trainingFeatureVectors.size()" << trainingFeatureVectors.size() << std::endl << std::flush;
        std::cout << "trainingStringToIntLabels.size()" << trainingStringToIntLabels.size() << std::endl << std::flush;
        std::cout << "this->caSettings->getLRL2RegulatizationParam()" << this->caSettings->getLRL2RegulatizationParam() << std::endl << std::flush;
        std::cout << "this->caSettings->getLRL2OptimizationAlgo()" << this->caSettings->getLRL2OptimizationAlgo() << std::endl << std::flush;
        std::cout << "this->caSettings->getLRL2NumOfIterations()" << this->caSettings->getLRL2NumOfIterations() << std::endl << std::flush;
        std::cout << "this->caSettings->getLRL2Tolerance()" << this->caSettings->getLRL2Tolerance() << std::endl << std::flush;

        this->model = new datk::L2LogisticRegression<datk::SparseFeature>(sparseFeatures,
            this->trainingIntLabels,
            this->trainingFeatureVectors[0].size(),
            this->trainingFeatureVectors.size(),
            this->trainingStringToIntLabels.size(),
            this->caSettings->getLRL2RegulatizationParam(),
            this->caSettings->getLRL2OptimizationAlgo(),
            this->caSettings->getLRL2NumOfIterations(),
            this->caSettings->getLRL2Tolerance());
    } else if (this->caSettings->getMLAlgo() == "HSVML2") {
        this->model = new datk::L2HingeSVM<datk::SparseFeature>(sparseFeatures,
            this->trainingIntLabels,
            this->trainingFeatureVectors[0].size(),
            this->trainingFeatureVectors.size(),
            this->trainingStringToIntLabels.size(),
            this->caSettings->getHSVML2RegulatizationParam(),
            this->caSettings->getHSVML2OptimizationAlgo(),
            this->caSettings->getHSVML2NumOfIterations(),
            this->caSettings->getHSVML2Tolerance());
    } else if (this->caSettings->getMLAlgo() == "SSVML1") {
        this->model = new datk::L2SmoothSVM<datk::SparseFeature>(sparseFeatures,
            this->trainingIntLabels,
            this->trainingFeatureVectors[0].size(),
            this->trainingFeatureVectors.size(),
            this->trainingStringToIntLabels.size(),
            this->caSettings->getSSVML1RegulatizationParam(),
            this->caSettings->getSSVML1OptimizationAlgo(),
            this->caSettings->getSSVML1NumOfIterations(),
            this->caSettings->getSSVML1Tolerance());
    } else if (this->caSettings->getMLAlgo() == "SSVML2") {
        this->model = new datk::L1SmoothSVM<datk::SparseFeature>(sparseFeatures,
            this->trainingIntLabels,
            this->trainingFeatureVectors[0].size(),
            this->trainingFeatureVectors.size(),
            this->trainingStringToIntLabels.size(),
            this->caSettings->getSSVML2RegulatizationParam(),
            this->caSettings->getSSVML2OptimizationAlgo(),
            this->caSettings->getSSVML2NumOfIterations(),
            this->caSettings->getSSVML2Tolerance());
    }
    std::cout << "hello there 5" << std::endl;

    this->model->loadJsonModel(this->caSettings->getDATKSaveModelPath().c_str());
    std::cout << "hello there 6" << std::endl;
}

std::string aiSaac::CustomAnalytics::detect(const cv::Mat &rawFrame) {
    if (!this->model) {
        std::cerr << "DATK Model has not been loaded" << std::endl;
        return NULL;
    }
    std::vector<float> featureVector = this->caffeClassifier->Predict(rawFrame,
        this->caSettings->getCaffeFeatureExtractionLayer());
    datk::SparseFeature s;
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
    datk::Vector predictions;
    this->model->predictProbability(s, predictions);
    // int prediction = this->model->predict(s);
    // if (prediction == -1)
    //     prediction = 0;
    int predictionMax = datk::argMax(predictions);
    if (predictions[predictionMax] >= this->caSettings->getDATKPredictionProbThresh()) {
        return this->trainingStringToIntLabels[predictionMax].second;
    } else {
        return "Unknown";
    }

}

void aiSaac::CustomAnalytics::train(bool loadData,
    int *numCorrect,
    int *numTotal) {
    if (this->caSettings->getMode() == 0) {
        std::cout << "selected transfer learning" << std::endl;
        if(loadData) {
            if (!this->caSettings->getTrainingJsonDataPath().empty()) {
                std::ifstream f(this->caSettings->getTrainingJsonDataPath());
                if (f.good() && this->caSettings->getIsTrained()) {
                    std::cout << "Loading training" << std::endl;
                    this->loadData(this->caSettings->getTrainingJsonDataPath(), this->trainingJsonData);

                    //get label mapping from label file
                    int intLabel = 0;
                    std::ifstream labelFile(this->caSettings->getLabelFilePath());
                    std::string line;
                    while (std::getline(labelFile, line)) {
                        std::istringstream iss(line);
                        std::string className;
                        if (!(iss >> className)) { break; } // error
                        this->trainingStringToIntLabels.push_back(std::make_pair(intLabel,className));
                        intLabel++;
                    }

                    std::cout << "training Data size: " << this->trainingJsonData.size() << std::endl;
                    for (nlohmann::json::iterator theClass = this->trainingJsonData.begin();
                        theClass != this->trainingJsonData.end();
                        theClass++) {
                        std::cout << "theClass.key: " << theClass.key() << std::endl;
                        for (std::vector< std::pair<int, std::string> >::iterator it = this->trainingStringToIntLabels.begin();
                            it != this->trainingStringToIntLabels.end();) {
                            if(it->second == theClass.key().c_str()) {
                                intLabel = it->first;
                                break;
                            }
                            ++it;
                        }
                        std::cout << "theClass intLabel: " << intLabel << std::endl;
                        for (nlohmann::json::iterator storedFeature = theClass.value().begin();
                            storedFeature != theClass.value().end();
                            storedFeature++) {
                            this->trainingFeatureVectors.push_back(*storedFeature);
                            this->trainingIntLabels.push_back(intLabel);
                        }
                    }
                } else {
                    std::cout << "Sorry the training file "
                        << this->caSettings->getTrainingJsonDataPath()
                        << " does not exist/not trained and will be created"
                        << std::endl;
                    this->extractFeatures(this->caSettings->getTrainingDataPath(),
                        this->trainingFeatureVectors,
                        this->trainingIntLabels,
                        this->trainingStringToIntLabels,
                        this->trainingJsonData,
                        true);
                    std::cout << "extracted all features.. going to save to JSON: " << this->trainingJsonData.size() << std::endl << std::flush;
                    this->saveData(this->caSettings->getTrainingJsonDataPath(), this->trainingJsonData);
                }
            } else {
                std::cout << "You dont want to store the training data" << std::endl;
                this->extractFeatures(this->caSettings->getTrainingDataPath(),
                    this->trainingFeatureVectors,
                    this->trainingIntLabels,
                    this->trainingStringToIntLabels,
                    this->trainingJsonData);
            }
        }
        std::cout << "creating sparse feature vector" << std::endl;
        std::vector<datk::SparseFeature> sparseFeatures = std::vector<datk::SparseFeature>();
        for (int i = 0; i < this->trainingFeatureVectors.size(); i++) {
            std::cout << "feature: " << this->trainingFeatureVectors[i].size() << " - label: "
                << this->trainingIntLabels[i] << std::endl;
            datk::SparseFeature s;
            s.featureIndex = std::vector<int>();
            s.featureVec = std::vector<double>();
            s.index = i;
            for (int j = 0; j < this->trainingFeatureVectors[i].size(); j++) {
                if (this->trainingFeatureVectors[i][j] != 0) {
                    s.featureIndex.push_back(j);
                    s.featureVec.push_back(this->trainingFeatureVectors[i][j]);
                }
            }
            s.numFeatures = this->trainingFeatureVectors[i].size();
            s.numUniqueFeatures = s.featureVec.size();
            sparseFeatures.push_back(s);
        }
        std::cout << "sparseFeatures.size()" << sparseFeatures.size() << std::endl << std::flush;

        if (this->trainingStringToIntLabels.size() == 2) {
            for (int i = 0; i < this->trainingIntLabels.size(); i++) {
                this->trainingIntLabels[i] = 2 * this->trainingIntLabels[i] - 1;
            }
        }
        if (this->caSettings->getMLAlgo() == "LRL1") {
            this->model = new datk::L1LogisticRegression<datk::SparseFeature>(sparseFeatures,
                this->trainingIntLabels,
                this->trainingFeatureVectors[0].size(),
                this->trainingFeatureVectors.size(),
                this->trainingStringToIntLabels.size(),
                this->caSettings->getLRL1RegulatizationParam(),
                this->caSettings->getLRL1OptimizationAlgo(),
                this->caSettings->getLRL1NumOfIterations(),
                this->caSettings->getLRL1Tolerance());
        } else if (this->caSettings->getMLAlgo() == "LRL2") {
            std::cout << "hello there :)" << std::endl << std::flush;

            std::cout << "trainingIntLabels.size()" << trainingIntLabels.size() << std::endl << std::flush;
            std::cout << "trainingFeatureVectors[0].size()" << trainingFeatureVectors[0].size() << std::endl << std::flush;
            std::cout << "trainingFeatureVectors.size()" << trainingFeatureVectors.size() << std::endl << std::flush;
            std::cout << "trainingStringToIntLabels.size()" << trainingStringToIntLabels.size() << std::endl << std::flush;
            std::cout << "this->caSettings->getLRL2RegulatizationParam()" << this->caSettings->getLRL2RegulatizationParam() << std::endl << std::flush;
            std::cout << "this->caSettings->getLRL2OptimizationAlgo()" << this->caSettings->getLRL2OptimizationAlgo() << std::endl << std::flush;
            std::cout << "this->caSettings->getLRL2NumOfIterations()" << this->caSettings->getLRL2NumOfIterations() << std::endl << std::flush;
            std::cout << "this->caSettings->getLRL2Tolerance()" << this->caSettings->getLRL2Tolerance() << std::endl << std::flush;

            this->model = new datk::L2LogisticRegression<datk::SparseFeature>(sparseFeatures,
                this->trainingIntLabels,
                this->trainingFeatureVectors[0].size(),
                this->trainingFeatureVectors.size(),
                this->trainingStringToIntLabels.size(),
                this->caSettings->getLRL2RegulatizationParam(),
                this->caSettings->getLRL2OptimizationAlgo(),
                this->caSettings->getLRL2NumOfIterations(),
                this->caSettings->getLRL2Tolerance());
        } else if (this->caSettings->getMLAlgo() == "HSVML2") {
            this->model = new datk::L2HingeSVM<datk::SparseFeature>(sparseFeatures,
                this->trainingIntLabels,
                this->trainingFeatureVectors[0].size(),
                this->trainingFeatureVectors.size(),
                this->trainingStringToIntLabels.size(),
                this->caSettings->getHSVML2RegulatizationParam(),
                this->caSettings->getHSVML2OptimizationAlgo(),
                this->caSettings->getHSVML2NumOfIterations(),
                this->caSettings->getHSVML2Tolerance());
        } else if (this->caSettings->getMLAlgo() == "SSVML1") {
            this->model = new datk::L2SmoothSVM<datk::SparseFeature>(sparseFeatures,
                this->trainingIntLabels,
                this->trainingFeatureVectors[0].size(),
                this->trainingFeatureVectors.size(),
                this->trainingStringToIntLabels.size(),
                this->caSettings->getSSVML1RegulatizationParam(),
                this->caSettings->getSSVML1OptimizationAlgo(),
                this->caSettings->getSSVML1NumOfIterations(),
                this->caSettings->getSSVML1Tolerance());
        } else if (this->caSettings->getMLAlgo() == "SSVML2") {
            this->model = new datk::L1SmoothSVM<datk::SparseFeature>(sparseFeatures,
                this->trainingIntLabels,
                this->trainingFeatureVectors[0].size(),
                this->trainingFeatureVectors.size(),
                this->trainingStringToIntLabels.size(),
                this->caSettings->getSSVML2RegulatizationParam(),
                this->caSettings->getSSVML2OptimizationAlgo(),
                this->caSettings->getSSVML2NumOfIterations(),
                this->caSettings->getSSVML2Tolerance());
        }
        std::cout << "Custom Analytics: calling train" << std::endl;
        this->model->train();
        std::cout << "Custom Analytics: save" << std::endl;
        this->model->saveJsonModel(this->caSettings->getDATKSaveModelPath().c_str());

        std::cout << "Custom Analytics: writing labels to labels.txt" << std::endl;
        std::ofstream labelFile;
        labelFile.open(this->caSettings->getLabelFilePath(),
            std::ofstream::out | std::ofstream::trunc);

        for (int i = 0; i < this->trainingStringToIntLabels.size(); i++) {
            labelFile << this->trainingStringToIntLabels[i].second << "\n";
        }

        labelFile.close();
        std::cout << "Custom Analytics: Calling predictAccuracy" << std::endl;
        double accuracy = this->predictAccuracy(sparseFeatures, this->trainingIntLabels);
        double accuracy_percentage = accuracy/trainingIntLabels.size();
        if (numCorrect != NULL) {
            *numCorrect = accuracy;
        }
        if (numTotal != NULL) {
            *numTotal = trainingIntLabels.size();
        }
        cout << "The train acuracy of the classifier is " << accuracy_percentage << "("
            << accuracy << "/"<< trainingIntLabels.size() << ")" << std::endl;
    } else if(this->caSettings->getMode() == 1) {
        std::cout << "selected fine tuning" << std::endl;
        if(!this->caSettings->getTrainingDataPath().empty()){
            std::cout << "setting up caffe trainer" << std::endl;
            aiSaac::CaffeTrainer caffeTrainer(this->caSettings->getTrainingDataPath(),
                this->caSettings->getTestingDataPath(),
                this->caSettings->getCaffeTemplateArchitecturePath(),
                this->caSettings->getCaffeTemplateSolverPath(),
                this->caSettings->getOriginalCaffeModelPath(),
                this->caSettings->getSolverStatePath(),
                this->caSettings->getWorkingDir(),
                this->caSettings->getNewNetworkName(),
                this->caSettings->getBackpropOptimizationAlgo(),
                this->caSettings->getMaxIterations(),
                this->caSettings->getTestIterationSize(),
                this->caSettings->getStepSize(),
                this->caSettings->getBaseLR(),
                this->caSettings->getSolverMode());
            std::cout << "about to call train()" << std::endl;
            caffeTrainer.train();
            std::cout << "fine tuning training done" << std::endl;

            // aiSaac::CaffeClassifier caffeClassifier(this->caSettings->getCaffeModelFilePath(),
            //     this->caSettings->getNewCaffeTrainedFilePath(),
            //     this->caSettings->getMeanFilePath(),
            //     this->caSettings->getLabelFilePath());

            // std::cout << "classifier created" << std::endl;
        } else {
            std::cout << "wrong training directory specified" << std::endl;
        }
    }
}

void aiSaac::CustomAnalytics::trainWithCrossVal() {
    // if (this->caSettings->getMode() == 0) {
    //     if (!this->caSettings->getTrainingJsonDataPath().empty()) {
    //         std::ifstream f(this->caSettings->getTrainingJsonDataPath());
    //         if (f.good()) {
    //             std::cout << "Loading training" << std::endl;
    //             this->loadData(this->caSettings->getTrainingJsonDataPath(), this->trainingJsonData);
    //         } else {
    //             std::cout << "Sorry the training file " <<
    //                 this->caSettings->getTrainingJsonDataPath()
    //                 << " does not exist and will be created"
    //                 << std::endl;
    //             this->extractFeatures(this->caSettings->getTrainingDataPath(),
    //                 this->trainingFeatureVectors,
    //                 this->trainingIntLabels,
    //                 this->trainingStringToIntLabels,
    //                 this->trainingJsonData);
    //
    //             this->saveData(this->caSettings->getTrainingJsonDataPath(), this->trainingJsonData);
    //         }
    //     }
    //
    //     if (!this->caSettings->getTestingJsonDataPath().empty()) {
    //         std::ifstream f(this->caSettings->getTestingJsonDataPath());
    //         if (f.good()) {
    //             std::cout << "Loading testing" << std::endl;
    //             this->loadData(this->caSettings->getTestingJsonDataPath(), this->testingJsonData);
    //         } else {
    //             std::cout << "Sorry the testing file " <<
    //                 this->caSettings->getTestingJsonDataPath()
    //                 << " does not exist and will be created"
    //                 << std::endl;
    //             this->extractFeatures(this->caSettings->getTestingDataPath(),
    //                 this->testingFeatureVectors,
    //                 this->testingIntLabels,
    //                 this->testingStringToIntLabels,
    //                 this->testingJsonData);
    //             this->saveData(this->caSettings->getTestingJsonDataPath(), this->testingJsonData);
    //         }
    //     }
    // }
}

void aiSaac::CustomAnalytics::test(bool loadData,
    int *numCorrect,
    int *numTotal) {
    if (this->caSettings->getMode() == 0) {
        if (!this->model) {
            std::cerr << "DATK Model has not been loaded" << std::endl;
            return;
        }
        std::cout << "selected transfer learning" << std::endl;
        if(loadData) {
            if (!this->caSettings->getTestingJsonDataPath().empty()) {
                std::ifstream f(this->caSettings->getTestingJsonDataPath());
                if (f.good() && this->caSettings->getIsTested()) {
                    std::cout << "Loading testing" << std::endl;
                    this->loadData(this->caSettings->getTestingJsonDataPath(), this->testingJsonData);

                    //get label mapping from label file
                    int intLabel = 0;
                    std::ifstream labelFile(this->caSettings->getLabelFilePath());
                    std::string line;
                    while (std::getline(labelFile, line)) {
                        std::istringstream iss(line);
                        std::string className;
                        if (!(iss >> className)) { break; } // error
                        this->testingStringToIntLabels.push_back(std::make_pair(intLabel,className));
                        intLabel++;
                    }

                    std::cout << "testing Data size: " << this->testingJsonData.size() << std::endl;
                    for (nlohmann::json::iterator theClass = this->testingJsonData.begin();
                        theClass != this->testingJsonData.end();
                        theClass++) {
                        std::cout << "theClass.key: " << theClass.key() << std::endl;
                        for (std::vector< std::pair<int, std::string> >::iterator it = this->testingStringToIntLabels.begin();
                            it != this->testingStringToIntLabels.end();) {
                            if(it->second == theClass.key().c_str()) {
                                intLabel = it->first;
                                break;
                            }
                            ++it;
                        }
                        std::cout << "theClass intLabel: " << intLabel << std::endl;
                        for (nlohmann::json::iterator storedFeature = theClass.value().begin();
                            storedFeature != theClass.value().end();
                            storedFeature++) {
                            this->testingFeatureVectors.push_back(*storedFeature);
                            this->testingIntLabels.push_back(intLabel);
                        }
                    }
                } else {
                    std::cout << "Sorry the testing file "
                        << this->caSettings->getTestingJsonDataPath()
                        << " does not exist and will be created"
                        << std::endl;
                    this->extractFeatures(this->caSettings->getTestingDataPath(),
                        this->testingFeatureVectors,
                        this->testingIntLabels,
                        this->testingStringToIntLabels,
                        this->testingJsonData,
                        true);
                    std::cout << "is it over? " << std::endl << std::flush;
                    this->saveData(this->caSettings->getTestingJsonDataPath(), this->testingJsonData);
                }
            } else {
                std::cout << "You dont want to store the testing data" << std::endl;
                this->extractFeatures(this->caSettings->getTestingDataPath(),
                    this->testingFeatureVectors,
                    this->testingIntLabels,
                    this->testingStringToIntLabels,
                    this->testingJsonData);
            }
        }
        std::cout << "creating sparse feature vector" << std::endl;
        std::vector<datk::SparseFeature> sparseFeatures = std::vector<datk::SparseFeature>();
        for (int i = 0; i < this->testingFeatureVectors.size(); i++) {
            std::cout << "feature: " << this->testingFeatureVectors[i].size() << " - label: "
                << this->testingIntLabels[i] << std::endl;
            datk::SparseFeature s;
            s.featureIndex = std::vector<int>();
            s.featureVec = std::vector<double>();
            s.index = i;
            for (int j = 0; j < this->testingFeatureVectors[i].size(); j++) {
                if (this->testingFeatureVectors[i][j] != 0) {
                    s.featureIndex.push_back(j);
                    s.featureVec.push_back(this->testingFeatureVectors[i][j]);
                }
            }
            s.numFeatures = this->testingFeatureVectors[i].size();
            s.numUniqueFeatures = s.featureVec.size();
            sparseFeatures.push_back(s);
        }

        if (this->testingStringToIntLabels.size() == 2) {
            for (int i = 0; i < this->testingIntLabels.size(); i++) {
                this->testingIntLabels[i] = 2 * this->testingIntLabels[i] - 1;
            }
        }
        double accuracy = predictAccuracy(sparseFeatures, this->testingIntLabels);
        double accuracy_percentage = accuracy/testingIntLabels.size();
        if (numCorrect != NULL) {
            *numCorrect = accuracy;
        }
        if (numTotal != NULL) {
            *numTotal = testingIntLabels.size();
        }
        cout << "The test acuracy of the classifier is " << accuracy_percentage << "(" << accuracy
            << "/" << testingIntLabels.size() << ")" << std::endl;
    }
}

void aiSaac::CustomAnalytics::extractFeatures(std::string dataPath,
    std::vector<std::vector<float>> &featureVectors,
    datk::Vector &intLabels,
    std::vector<std::pair<int, std::string>> &stringToIntLabels,
    nlohmann::json &jsonData,
    bool storeJson) {
    DIR *dataStorageDir;
    struct dirent *direntDataStorageDir;
    DIR *classPath;
    struct dirent *direntClassPath;
    dataStorageDir = opendir(dataPath.c_str());
    std::string classFolder;
    std::string className;
    std::string classImagePath;

    int count = 0;

    while((direntDataStorageDir = readdir(dataStorageDir)) != NULL) {
        // std::cout << direntDataStorageDir->d_name;
        classFolder = dataPath;
        if (direntDataStorageDir->d_type != DT_DIR
            || !std::strcmp(direntDataStorageDir->d_name, ".")
            || !std::strcmp(direntDataStorageDir->d_name, "..")) {
            continue;
        }

        className = direntDataStorageDir->d_name;
        classFolder = classFolder + "/" + direntDataStorageDir->d_name;
        classPath = opendir(classFolder.c_str());
        int currentIntLabel;
        if (className == "Unknown_Classes") {
            currentIntLabel = -2;
        }
        else {
            currentIntLabel = count;
            count++;
        }
        stringToIntLabels.push_back(std::make_pair(currentIntLabel, className));
        while (direntClassPath = readdir(classPath)) {
            if (direntClassPath->d_type != DT_REG) {
                continue;
            }
            std::string fileName(direntClassPath->d_name);
            if (fileName.substr(fileName.find_last_of(".") + 1) != "DS_Store") {
                classImagePath = classFolder + "/" + fileName;
                std::cout << "Extracting features for: " << classImagePath << std::endl << std::flush;
                std::vector<float> currentFeatureVector = this->caffeClassifier->Predict(
                    cv::imread(classImagePath),
                    this->caSettings->getCaffeFeatureExtractionLayer());
                if (storeJson) {
                    //std::cout << "hello here i am" << std::endl;
                    jsonData[className].push_back(currentFeatureVector);
                }
                featureVectors.push_back(currentFeatureVector);
                intLabels.push_back(currentIntLabel);
            } else {
                std::cout << "Ignoring " << fileName << std::endl;
            }
        }
    }
}

void aiSaac::CustomAnalytics::extractFeatures(std::string dataPath,
    std::vector<std::vector<float>> &featureVectors,
    datk::Vector &intLabels,
    std::vector<std::pair<int, std::string>> &stringToIntLabels,
    nlohmann::json &jsonData,
    nlohmann::json &jsonNamesData,
    bool storeJson) {
    DIR *dataStorageDir;
    struct dirent *direntDataStorageDir;
    DIR *classPath;
    struct dirent *direntClassPath;
    dataStorageDir = opendir(dataPath.c_str());
    std::string classFolder;
    std::string className;
    std::string classImagePath;

    int count = 0;

    while((direntDataStorageDir = readdir(dataStorageDir)) != NULL) {
        // std::cout << direntDataStorageDir->d_name;
        classFolder = dataPath;
        if (direntDataStorageDir->d_type != DT_DIR
            || !std::strcmp(direntDataStorageDir->d_name, ".")
            || !std::strcmp(direntDataStorageDir->d_name, "..")) {
            continue;
        }

        className = direntDataStorageDir->d_name;
        classFolder = classFolder + "/" + direntDataStorageDir->d_name;
        classPath = opendir(classFolder.c_str());
        int currentIntLabel;
        if (className == "Unknown_Classes") {
            currentIntLabel = -2;
        }
        else {
            currentIntLabel = count;
            count++;
        }
        stringToIntLabels.push_back(std::make_pair(currentIntLabel, className));
        while (direntClassPath = readdir(classPath)) {
            if (direntClassPath->d_type != DT_REG) {
                continue;
            }
            std::string fileName(direntClassPath->d_name);
            if (fileName.substr(fileName.find_last_of(".") + 1) != "DS_Store") {
                classImagePath = classFolder + "/" + fileName;
                std::cout << "Extracting features for: " << classImagePath << std::endl << std::flush;
                std::vector<float> currentFeatureVector = this->caffeClassifier->Predict(
                    cv::imread(classImagePath),
                    this->caSettings->getCaffeFeatureExtractionLayer());
                if (storeJson) {
                    //std::cout << "hello here i am, pushed " << fileName << std::endl;
                    jsonData[className].push_back(currentFeatureVector);
                    jsonNamesData[className].push_back(classImagePath);
                }
                featureVectors.push_back(currentFeatureVector);
                intLabels.push_back(currentIntLabel);
            } else {
                std::cout << "Ignoring " << fileName << std::endl;
            }
        }
    }
}

double aiSaac::CustomAnalytics::predictAccuracy(
    std::vector<datk::SparseFeature>& testFeatures,
    datk::Vector& ytest) {
    // assert(testFeatures.size() == ytest.size());
    double accuracy = 0;
    for (int i = 0; i < testFeatures.size(); i++) {
        datk::Vector predictions;
        this->model->predictProbability(testFeatures[i], predictions);
        // int prediction = this->model->predict(s);
        // if (prediction == -1)
        //     prediction = 0;
        int predictionMax = datk::argMax(predictions);
        std::cout << "Predicted " << ytest[i] << " as " << predictionMax
            << " with probability " << predictions[predictionMax] << std::endl << std::flush;
        if (predictions.size() == 2) {
            if (predictionMax == 0) {
                predictionMax = -1;
            }
        }
        // std::cout << "Predicted " << ytest[i] << " as " << predictionMax
        //     << " with probability " << predictions[predictionMax] << std::endl << std::flush;
        if (predictions.size() > 2) {
          if (predictions[predictionMax] >= this->caSettings->getDATKPredictionProbThresh()) {
              if (predictionMax == ytest[i]) {
                  accuracy++;
              }
          }
        }
        else {
            if (predictionMax == ytest[i]) {
                accuracy++;
            }
        }
        // else if (ytest[i] == -2) {
        //     accuracy++;
        // }
    }
    return accuracy;
}

void aiSaac::CustomAnalytics::loadData(std::string filePath, nlohmann::json &jsonData) {
    std::ifstream dataFile;
    dataFile.open(filePath);
    dataFile >> jsonData;
}

void aiSaac::CustomAnalytics::saveData(std::string filePath, nlohmann::json jsonData) {
    std::cout << "saving file" << filePath << std::endl;
    std::ofstream dataFile;
    dataFile.open(filePath);
    dataFile << jsonData.dump(4);
    dataFile.close();
}
