#include "../../src/aiSaac.h"
#include <iostream>
#include "subsetselection.h"
#include <fstream>
#include <ctime>

int main(int argc, char *argv[]) {
    if (argc < 4) {
        //TODO: do not take results.csv file name, generate smartly
        //TODO: provide one end-to-end training option
        std::cout << "usage: ./aiSaacResearchGoal1 customAnalyticsSettingsPath {NN | TrainCNN | FineTuneCNN} [path/to/output/folder] [randSeed(optional)]" << std::endl;
        std::cout << "Make sure the customAnalyticsSettings file points to the right dataset(test+train), label file, caffe model, json files (test+train) and caffe feature extraction layer" << std::endl;
        return 0;
    }
    auto seed = std::time(0);
    if (argc > 4) {
        seed = std::atoi(argv[4]);
    }
    std::srand(seed);
    clock_t grandBegin = clock();

    int algo = -1;
    if (std::string(argv[2]) == "NN" || std::string(argv[2]) == "nn") algo = 0;
    else if (std::string(argv[2]) == "TrainCNN") algo = 1;
    else if (std::string(argv[2]) == "FineTuneCNN") algo = 2;
    else {
        std::cout << "Invalid algorithm, quitting" << std::endl;
        return 0;
    }

    std::cout << "Loading caSettings file .." << std::endl;
    aiSaac::CASettings *caSettings = new aiSaac::CASettings(argv[1]);

    std::string str = caSettings->getTrainingDataPath();
    std::size_t found = str.find_last_of("/\\");
    std::string dataSetPath = str.substr(0,found);
    found = dataSetPath.find_last_of("/\\");
    std::string dataSetName = dataSetPath.substr(found+1);
    std::string resultsFileName("goal1-" + std::string(argv[2]) + "-" + dataSetName + ".csv");

    //preparing output csv file
    std::cout << "Preparing results csv file ..." << std::endl;
    ofstream resultsFile;
    resultsFile.open(std::string(argv[3]) + "/" + resultsFileName);
    resultsFile << "Desired algo," << std::string(argv[2]) << std::endl << std::flush;
    resultsFile << "Train data path," << caSettings->getTrainingDataPath() << std::endl << std::flush;
    resultsFile << "Test data path," << caSettings->getTestingDataPath() << std::endl << std::flush;
    resultsFile << "Label file path," << caSettings->getLabelFilePath() << std::endl << std::flush;
    resultsFile << "Caffe network file path," << caSettings->getCaffeNetworkFilePath() << std::endl << std::flush;
    resultsFile << "Caffe trained model path," << caSettings->getCaffeTrainedFilePath() << std::endl << std::flush;
    resultsFile << "Training data features json path," << caSettings->getTrainingJsonDataPath() << std::endl << std::flush;
    resultsFile << "Training data file names json path," << caSettings->getTrainingFileNamesJsonDataPath() << std::endl << std::flush;
    resultsFile << "Testing data features json path," << caSettings->getTestingJsonDataPath() << std::endl << std::flush;
    resultsFile << "Caffe feature extraction layer," << caSettings->getCaffeFeatureExtractionLayer() << std::endl << std::flush;
    resultsFile << "\n\n" << std::flush;

    if (algo == 1 || algo == 2) {
        resultsFile << "Caffe Template Architecture Path," << caSettings->getCaffeTemplateArchitecturePath() << std::endl << std::flush;
        resultsFile << "Caffe Template Solver Path," << caSettings->getCaffeTemplateSolverPath() << std::endl << std::flush;
        resultsFile << "Original Caffe Model Path," << caSettings->getOriginalCaffeModelPath() << std::endl << std::flush;
        resultsFile << "Working dir," << caSettings->getWorkingDir() << std::endl << std::flush;
        resultsFile << "Back prop algo," << caSettings->getBackpropOptimizationAlgo() << std::endl << std::flush;
        resultsFile<< "Max iterations," << caSettings->getMaxIterations() << std::endl << std::flush;
        resultsFile << "Test iterations size," << caSettings->getTestIterationSize() << std::endl << std::flush;
        resultsFile << "Step size," << caSettings->getStepSize() << std::endl << std::flush;
        resultsFile << "Base LR," << caSettings->getBaseLR() << std::endl << std::flush;
        resultsFile << "Solver Mode," << caSettings->getSolverMode() << std::endl << std::flush;
    }

    resultsFile << "F/B,20,30,40,50\n" << std::flush;

    int k = 5; //for kNN


    aiSaac::CustomAnalytics *ca = new aiSaac::CustomAnalytics(caSettings);

    //extract features of entire training data set, if the json file doesn't already exist
    std::vector<std::vector<float>> fullTrainingFeatureVectors;
    datk::Vector fullTrainingIntLabels;
    std::vector<std::pair<int, std::string>> fullTrainingStringToIntLabels;
    nlohmann::json fullTrainJsonData;
    nlohmann::json fullTrainFileNamesJsonData;
    std::ifstream fTrain(caSettings->getTrainingJsonDataPath());
    bool createdSomeJsonFile = false;
    if (!fTrain.good()) {
        fTrain.close();
        std::cout << "Full training data json file doesn't exist, extracting features and creating one and loading fullTrainJSON in memory...same for file names json" << std::endl;
        clock_t begin = clock();
        ca->extractFeatures(caSettings->getTrainingDataPath(), fullTrainingFeatureVectors, fullTrainingIntLabels, fullTrainingStringToIntLabels, fullTrainJsonData, fullTrainFileNamesJsonData, true);
        clock_t end = clock();
        double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
        std::cout << "Extracted features from entire train data. Number of classes = " << fullTrainJsonData.size() << " Time taken to extract train and load = " << std::to_string(elapsed_secs) << " seconds." << std::endl << std::flush;
        std::cout << "Saving this JSON Data into file: " << caSettings->getTrainingJsonDataPath() << std :: endl;
        ca->saveData(caSettings->getTrainingJsonDataPath(), fullTrainJsonData);
        ca->saveData(caSettings->getTrainingFileNamesJsonDataPath(), fullTrainFileNamesJsonData);
        createdSomeJsonFile = true;
    } else {
        std::cout << "Full training data json and file names json file exists, loading them in memory." << std::endl;
        std::ifstream f(caSettings->getTrainingJsonDataPath());
        std::ifstream fFileNames(caSettings->getTrainingFileNamesJsonDataPath());
        if (f.good() && fFileNames.good()) {
            f.close();
            fFileNames.close();
            clock_t begin = clock();
            ca->loadData(caSettings->getTrainingJsonDataPath(), fullTrainJsonData);
            ca->loadData(caSettings->getTrainingFileNamesJsonDataPath(), fullTrainFileNamesJsonData);
            clock_t end = clock();
            double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
            std::cout << "Time taken to load train json and file names= " << std::to_string(elapsed_secs) << " seconds." << std::endl << std::flush;
            std::cout << "Loaded full train json data in memory" << std::endl;
        }
    }
    std::cout << "Full train JSON data.size = " << fullTrainJsonData.size() << std::endl;

    //now lets load the full test data in memory - needed only if algo is KNN
    if (algo == 0) {
        std::cout << "Since KNN has been chosen, loading test data json into memory to keep it ready..." << std::endl;
        std::ifstream fTest(caSettings->getTestingJsonDataPath());
        if (!fTest.good()) {
            fTest.close();
            std::cout << "Test json file doesn't already exist, creating one..." << std::endl;
            clock_t begin = clock();
            ca->extractFeatures(
                    caSettings->getTestingDataPath(),
                    ca->testingFeatureVectors,
                    ca->testingIntLabels,
                    ca->testingStringToIntLabels,
                    ca->testingJsonData, true);
            clock_t end = clock();
            double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
            std::cout << "Time taken to extract and load test json= " << std::to_string(elapsed_secs) << " seconds." << std::endl << std::flush;
            std::cout << "Extracted all features from test data.. going to save to JSON" << std::endl << std::flush;
            ca->saveData(caSettings->getTestingJsonDataPath(), ca->testingJsonData);
            createdSomeJsonFile = true;
        } else {
            fTest.close();
            clock_t begin = clock();
            ca->loadData(caSettings->getTestingJsonDataPath(), ca->testingJsonData);
            clock_t end = clock();
            double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
            std::cout << "Time taken to load test json data = " << std::to_string(elapsed_secs) << " seconds." << std::endl << std::flush;
            double intLabel = 0;
            for (nlohmann::json::iterator theClass = ca->testingJsonData.begin();
                    theClass != ca->testingJsonData.end();
                    theClass++) {
                //std::cout << "theClass.key: " << theClass.key() << std::endl;
                ca->testingStringToIntLabels.push_back(
                        std::make_pair(intLabel, theClass.key().c_str()));
                for (nlohmann::json::iterator storedFeature = theClass.value().begin();
                        storedFeature != theClass.value().end();
                        storedFeature++) {
                    ca->testingFeatureVectors.push_back(*storedFeature);
                    ca->testingIntLabels.push_back(intLabel);
                }
                intLabel++;
            }
        }
        std::cout << "Loaded test data json in ca data structure" << std::endl;
    }
    if(createdSomeJsonFile) {
        std::cout << "Just wrote a Json file to filesystem. Cannot start the experiments right now due to ordering issues. Please rerun." << std::endl;
        return 0;
    }
    SubsetSelection *subSet = new SubsetSelection(ca);
    int counter = 0;
    for (int mode = 0; mode < 6; mode++) {
        resultsFile << mode << "," << std::flush;
        for (double budget = 20; budget <= 50; budget = budget + 10) {
            std::cout << "*****Experiment no. " << counter << " with mode = " << mode << " and budget = " << budget << "% *****" << std::endl;
            int numCorrect = -1;
            int numInCorrect = -1;
            int total = -1;
            subSet->runExperiment(algo, budget, k, numCorrect, numInCorrect, mode, total, fullTrainJsonData, fullTrainFileNamesJsonData);
            if (algo == 0) {
                std::cout << "Total predictions made = " << total << std::endl;
                std::cout << "Number of correct predictions = " << numCorrect << std::endl;
                std::cout << "Number of incorrect predictions = " << numInCorrect << std::endl;
                if (total != numCorrect + numInCorrect) std::cout << "Oops!! What went wrong?" << std::endl;
                std::cout << "Accuracy = " << ((float) numCorrect) / total << std::endl;
                resultsFile << ((float) numCorrect) / total << "," << std::flush;
            } else if (algo == 1 || algo == 2) {
                std::cout << "Result for this run is available in the stderr redirected file" << std::endl;
                resultsFile << "X," << std::flush;
            }

            std::cout << "Experiment no. " << counter << " over *****" << std::endl;
            counter++;
        }
        resultsFile << "\n" << std::flush;
    }
    clock_t grandEnd = clock();
    double grandElapsedSecs = double(grandEnd - grandBegin) / CLOCKS_PER_SEC;
    resultsFile << "Total time for experiment," << std::to_string(grandElapsedSecs) << " seconds." << std::endl << std::flush;
    resultsFile.close();
}
