#include <iostream>
#include "subsetselection.h"
#include <string>
#include <ctime>

extern std::string storagePath;

struct Similarity {
    int label;
    float sim;
};

bool sortBySimilarity(const Similarity &a, const Similarity &b) {
    return a.sim > b.sim;
}

SubsetSelection::SubsetSelection(aiSaac::CustomAnalytics *ca) {
    std::srand(std::time(0));
    this->ca = ca;
}

SubsetSelection::~SubsetSelection() {
}

void SubsetSelection::runExperiment(int algo,
        double budget,
        int k,
        int &numCorrect,
        int &numInCorrect,
        int mode,
        int &total,
        nlohmann::json fullTrainJsonData,
        nlohmann::json fullTrainFileNamesJsonData) {

    //std::cout << "Received fullTrainJsonData.size = " << fullTrainJsonData.size() << std::endl;
    //std::cout << "Received fullTrainFileNamesJsonData.size = " << fullTrainFileNamesJsonData.size() << std::endl;
    ca->trainingFeatureVectors.clear();
    ca->trainingIntLabels.clear();
    ca->trainingJsonData.clear();
    ca->trainingStringToIntLabels.clear();
    std::vector<std::string> fileNames;
    std::vector<std::vector<double>> featureVectorsOfAClass;

    std::string command;

    //prepare B% training data set - appropriate feature vectors for KNN and a new data set in file system for fine tuning CNN

    if (algo == 1 || algo == 2) {
        //create a top level directory for this  subset of training data set
        std::cout << "Cleaning parent folder for subset of training data" << std::endl;
        command = "rm -rf " + storagePath + "/data/goal1-cnn-" + std::to_string(mode) + "-" + std::to_string((int)budget) + "/";
        system(command.c_str());

        std::cout << "Creating parent folder for subset of training data" << std::endl;
        command = "mkdir -p " + storagePath + "/data/goal1-cnn-" + std::to_string(mode) + "-" + std::to_string((int)budget) + "/";
        system(command.c_str());
    }

    int intLabel = 0;
    std::string strLabel;
    int fullTrainJsonDataSize = 0;
    //for every class
    for (nlohmann::json::iterator theClass = fullTrainJsonData.begin(), theClassFileName = fullTrainFileNamesJsonData.begin(); theClass != fullTrainJsonData.end(), theClassFileName != fullTrainFileNamesJsonData.end(); theClass++, theClassFileName++) {
        fileNames.clear();
        featureVectorsOfAClass.clear();
        //get features of all images of this class and store in a vector of vectors (featureVectorsOfAClass)
        //std::cout << "theClass.key: " << theClass.key() << std::endl;
        strLabel = theClass.key().c_str();
        if (algo == 1 || algo == 2) {
            //create a directory corresponding to this class
            std::cout << "Creating a subdirectory for " << strLabel << std::endl;
            command = "mkdir -p " + storagePath + "/data/goal1-cnn-" + std::to_string(mode) + "-" + std::to_string((int)budget) + "/" + strLabel + "/";
            system(command.c_str());

        }
        ca->trainingStringToIntLabels.push_back(std::make_pair(intLabel, strLabel));
        int numInClass = 0;

        for (nlohmann::json::iterator storedFeature = theClass.value().begin(), storedFileName = theClassFileName.value().begin(); storedFeature != theClass.value().end(), storedFileName != theClassFileName.value().end(); storedFeature++, storedFileName++) {
            fullTrainJsonDataSize += theClass.value().size();
            featureVectorsOfAClass.push_back(*storedFeature);
            fileNames.push_back(*storedFileName);
            //std::cout << "File name: " << *storedFileName << std::endl;
            numInClass++;
        }
        std::cout << "Number of images in class " << strLabel << " = " << numInClass << std::endl;
        //extract B indices
        std::cout << "Let's find a subset" << std::endl;
        datk::Set indices;

        switch (mode) {
            case 0:
                std::cout << "Using random mode" << std::endl;
                indices = getBIndicesRandom(numInClass,
                        featureVectorsOfAClass,
                        ceil((budget * numInClass) / 100));
                break;
            case 1:
                std::cout << "Using facility location" << std::endl;
                indices = getBIndicesFacilityLocation(numInClass,
                        featureVectorsOfAClass,
                        ceil((budget * numInClass) / 100));
                break;
            case 2:
                std::cout << "Using Disparity Min" << std::endl;
                indices = getBIndicesDisparityMin(numInClass,
                        featureVectorsOfAClass,
                        ceil((budget * numInClass) / 100));
                break;
            case 3:
                std::cout
                        << "Using Feature Based Functions with sqrt over modular"
                        << std::endl;
                indices = getBIndicesFeatureBasedFunctions(numInClass,
                        featureVectorsOfAClass,
                        ceil((budget * numInClass) / 100),
                        1);
                break;
            case 4:
                std::cout
                        << "Using Feature Based Functions with inverse function"
                        << std::endl;
                indices = getBIndicesFeatureBasedFunctions(numInClass,
                        featureVectorsOfAClass,
                        ceil((budget * numInClass) / 100),
                        2);
                break;
            case 5:
                std::cout
                        << "Using Feature Based Functions with Log function"
                        << std::endl;
                indices = getBIndicesFeatureBasedFunctions(numInClass,
                        featureVectorsOfAClass,
                        ceil((budget * numInClass) / 100),
                        3);
                break;
            default:
                std::cout << "Invalid mode (submodular function), quitting" << std::endl;
                return;
        }
        std::cout << "Selected " << indices.size() << " elements out of a total of " << numInClass << " for class " << strLabel << std::endl;

        if (algo == 0) {
            //store corresponding feature vectors in final trainFeatureVectors and corresponding labels in trainLabelsVector
            std::cout << "Since it is KNN, creating appropriate trainFeatureVectors using selected subset" << std::endl;
            datk::Set::iterator it;
            for (it = indices.begin(); it != indices.end(); ++it) {
                //std::cout << *it << " ";
                vector<double> featureVectorsOfClassIt = featureVectorsOfAClass.at(*it);
                vector<float> floatVector(featureVectorsOfClassIt.begin(),
                        featureVectorsOfClassIt.end());
                ca->trainingFeatureVectors.push_back(floatVector);
                ca->trainingIntLabels.push_back(intLabel);
            }
            //std::cout << endl;
        } else if (algo == 1 || algo == 2) {
            if(indices.size() < 2) {
                std::cout << "***Not enough Data, moving on to next sub experiment***" << std::endl;
                return;
            }
            std::cout << "Algo is CNN, storing selected images at appropriate location in sub data set" << std::endl;
            //store corresponding images into a new data set following appropriate folder structure
            datk::Set::iterator it;
            for (it = indices.begin(); it != indices.end(); ++it) {
                vector<double> featureVectorsOfClassIt = featureVectorsOfAClass.at(*it);
                std::string fullFileName = fileNames.at(*it);
                std::cout << "File name selected: " << fullFileName << std::endl;
                command = "cp \"" + fullFileName + "\" " + storagePath + "/data/goal1-cnn-" + std::to_string(mode) + "-" + std::to_string((int)budget) + "/" + strLabel + "/";
                system(command.c_str());
            }
        }

        intLabel++;
    }

    if (algo == 0) {
        //predict class of every data point in test data set using kNN on trainFeatureVectors and trainLabelsVector
        std::cout << "Making predictions using KNN" << std::endl;
        std::cout << "Total no. of training faces = " << ca->trainingFeatureVectors.size() << std::endl;
        std::cout << "Total no. of test faces = " << ca->testingFeatureVectors.size() << std::endl;
        int mostLikelyLabel = -1;
        clock_t begin = clock();
        total = 0;
        numCorrect = 0;
        numInCorrect = 0;
        for (int i = 0; i < ca->testingFeatureVectors.size(); i++) {
            total++;
            std::vector<Similarity> similarities;
            for (int j = 0; j < ca->trainingFeatureVectors.size(); j++) {
                float innerProduct = 0;
                for (int k = 0; k < ca->testingFeatureVectors.at(i).size(); k++) {
                    innerProduct +=
                            ca->trainingFeatureVectors.at(j).at(k) *
                            ca->testingFeatureVectors.at(i).at(k);
                }
                Similarity similarity;
                similarity.label = ca->trainingIntLabels.at(j);
                similarity.sim = innerProduct / (
                        norm(ca->trainingFeatureVectors.at(j),
                        cv::NORM_L2) *
                        norm(ca->testingFeatureVectors.at(i), cv::NORM_L2));
                similarities.push_back(similarity);
            }
            std::sort(similarities.begin(), similarities.end(), sortBySimilarity);
            std::unordered_map<int, int> topKSims;
            for (int m = 0; m < k && m != similarities.size(); m++) {
                if (!topKSims.count(similarities[m].label)) {
                    topKSims[similarities[m].label] = 0;
                }
                topKSims[similarities[m].label]++;
            }
            int maxCount = -1;
            for (auto n = topKSims.begin(); n != topKSims.end(); n++) {
                if (n->second > maxCount) {
                    maxCount = n->second;
                    mostLikelyLabel = n->first;
                }
            }
            if (mostLikelyLabel == ca->testingIntLabels.at(i)) {
                numCorrect++;
            } else {
                numInCorrect++;
            }
        }
        clock_t end = clock();
        double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
        std::cout << "Time taken for KNN to complete = " << std::to_string(elapsed_secs) << " seconds." << std::endl << std::flush;
    } else if (algo == 1 || algo == 2) {
        //fine tune CNN based on this subset of training data
        std::cout << "Have been asked to fine tune a CNN" << std::endl;
        std::cout << "Training subset data path" << storagePath << "/data/goal1-cnn-" << std::to_string(mode) << "-" << std::to_string((int)budget) << "/" << std::endl;
        std::cout << "New Network Name" << "goal1-cnn-" << std::to_string(mode) << "-" << std::to_string((int)budget) << std::endl;
        std::cout << "Cleaning working directory" <<std::endl;
        command = "rm -rf " +ca->caSettings->getWorkingDir();
        system(command.c_str());
        aiSaac::CaffeTrainer caffeTrainer(storagePath + "/data/goal1-cnn-" + std::to_string(mode) + "-" + std::to_string((int)budget) + "/",
                ca->caSettings->getTestingDataPath(),
                ca->caSettings->getCaffeTemplateArchitecturePath(),
                ca->caSettings->getCaffeTemplateSolverPath(),
                ca->caSettings->getOriginalCaffeModelPath(),
                ca->caSettings->getSolverStatePath(),
                ca->caSettings->getWorkingDir(),
                "goal1-cnn-"+std::to_string(mode)+"-"+std::to_string((int)budget),
                ca->caSettings->getBackpropOptimizationAlgo(),
                ca->caSettings->getMaxIterations(),
                ca->caSettings->getTestIterationSize(),
                ca->caSettings->getStepSize(),
                ca->caSettings->getBaseLR(),
                ca->caSettings->getSolverMode());
        std::cout << "about to call train()" << std::endl;
        clock_t begin = clock();
        caffeTrainer.train();
        clock_t end = clock();
        double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
        std::cout << "Time taken for fine-tuning = " << std::to_string(elapsed_secs) << " seconds." << std::endl << std::flush;
        //std::cout << "fine tuning training done" << std::endl;
        //TODO:report accuracy of this model
    }//TODO:implement end-to-end training
}

datk::Set SubsetSelection::getBIndicesRandom(int n,
        std::vector<std::vector<double>> &featureVectors,
        double budget) {
    //for each feature vector i (0 to n), identify random featureVectors and populate indices
    datk::Set indices;
    std::vector<int> possibilities;
    for (int i = 0; i < n; i++) {
        possibilities.push_back(i);
    }
    //auto engine = std::default_random_engine{};
    std::random_shuffle(possibilities.begin(), possibilities.end());
    for (int i = 0; i < budget; i++) {
        std::cout << "Possibilities: " << possibilities[i] << std::endl;
        indices.insert(possibilities[i]);
    }
    return indices;
}

datk::Set SubsetSelection::getBIndicesFacilityLocation(int n,
        std::vector<std::vector<double>> &featureVectors,
        double budget) {
    //for each feature vector i (0 to n), compute its dot product with feature vector j (0 to n), update min, update max, store at k(i,j)
    datk::Set indices;
    double val = 0;
    double min = std::numeric_limits<double>::max();
    double max = std::numeric_limits<double>::min();
    std::vector<std::vector<float>> kernel;
    for (int i = 0; i < featureVectors.size(); i++) {
        std::vector<float> currvector;
        for (int j = 0; j < featureVectors.size(); j++) {
            val = datk::innerProduct(featureVectors.at(i), featureVectors.at(j));
            if (val < min) min = val;
            if (val > max) max = val;
            currvector.push_back(val);
        }
        kernel.push_back(currvector);
    }
    //0-1 normalize using min and max - this becomes the kernel
    double range = max - min;
    for (int i = 0; i < featureVectors.size(); i++) {
        for (int j = 0; j < featureVectors.size(); j++) {
            kernel[i][j] = (kernel[i][j] - min) / range;
        }
    }
    //instantiate facility location using kernel
    datk::FacilityLocation facLoc(n, kernel);
    //call lazygreedymax with this facility location fuction, B, null output set
    datk::lazyGreedyMax(facLoc, budget, indices);
    return indices;
}

datk::Set SubsetSelection::getBIndicesDisparityMin(int n,
        std::vector<std::vector<double>> &featureVectors,
        double budget) {
    //for each feature vector i (0 to n), compute its dot product with feature vector j (0 to n), update min, update max, store at k(i,j)
    datk::Set indices;
    double val = 0;
    double min = std::numeric_limits<double>::max();
    double max = std::numeric_limits<double>::min();
    std::vector<std::vector<float>> kernel;
    for (int i = 0; i < featureVectors.size(); i++) {
        std::vector<float> currvector;
        for (int j = 0; j < featureVectors.size(); j++) {
            val = datk::innerProduct(featureVectors.at(i), featureVectors.at(j));
            if (val < min) min = val;
            if (val > max) max = val;
            currvector.push_back(val);
        }
        kernel.push_back(currvector);
    }
    //0-1 normalize using min and max - this becomes the kernel
    double range = max - min;
    for (int i = 0; i < featureVectors.size(); i++) {
        for (int j = 0; j < featureVectors.size(); j++) {
            kernel[i][j] = (kernel[i][j] - min) / range;
        }
    }
    //instantiate facility location using kernel
    datk::DisparityMin dispMin(n, kernel);
    //call lazygreedymax with this facility location fuction, B, null output set
    datk::naiveGreedyMax(dispMin, budget, indices, 0, false, true);
    return indices;
}

datk::Set SubsetSelection::getBIndicesFeatureBasedFunctions(int n,
        std::vector<std::vector<double>> &featureVectors,
        double budget,
        int mode) {
    //for each feature vector i (0 to n), compute its sparse features
    datk::Set indices;
    std::vector<datk::SparseFeature> sparseFeatures = std::vector<datk::SparseFeature>();
    std::vector<double> featureWeights = std::vector<double>();
    for (int i = 0; i < featureVectors[0].size(); i++) {
        featureWeights.push_back(1);
    }
    for (int i = 0; i < featureVectors.size(); i++) {
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
        s.numUniqueFeatures = s.featureVec.size();
        sparseFeatures.push_back(s);
    }
    //instantiate Feature Based Functions using sparseFeatures
    datk::FeatureBasedFunctions featBasedFunc(n, mode, sparseFeatures, featureWeights);
    //call lazygreedymax with this Feature Based Functions fuction, B, null output set
    datk::lazyGreedyMax(featBasedFunc, budget, indices);
    return indices;
}
