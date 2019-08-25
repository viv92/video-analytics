#include "../../src/aiSaac.h"
#include "../../src/utils/datk/src/datk.h"
#include <iostream>
#include <vector>
#include <math.h>

aiSaac::CustomAnalytics *ca;
std::vector<std::vector <float>> seedFeatureVectors;
datk::Vector seedIntLabels;
int uncertaintyMode;

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

bool sortByUncertainty(const std::pair<int,double> &a, const std::pair<int,double> &b) {
    return a.second > b.second;
}

double getUncertainty(datk::Vector &predictions, int mode) {
    double uncertainty;
    switch(mode) {
        case 0: //Do arg max
        {
            int index = datk::argMax(predictions);
            uncertainty = 1 - predictions[index];
            break;
        }
        case 1:
        {
            datk::Vector predCopy = predictions;
            int maxIndex = datk::argMax(predCopy);
            double maxProbability = predCopy[maxIndex];
            predCopy.erase(predCopy.begin() + maxIndex);
            double secondMax = predCopy[datk::argMax(predCopy)];
            double difference = maxProbability - secondMax;
            uncertainty = 1 - difference;
            break;
        }
        case 2:
        {
            // Use entropy
            // std::cout << "Using Entropy" << std::endl;
            uncertainty = 0;
            // std::cout << "Iterating Entropy" << std::endl;
            for (datk::Vector::iterator it = predictions.begin();
                it != predictions.end();
                it++) {
                if (*it != 0) {
                    uncertainty = uncertainty
                        + (*it * log2(*it));
                    // std::cout << "Using Entropy calc" << uncertainty << std::endl;
                    // std::cout << "Probability " << *it <<
                    //     "changes entropy by " << (*it * log2(*it)) << std::endl;
                }
            }
            // std::cout << "Finished iterating Entropy" << std::endl;
            uncertainty = 0 - uncertainty;
        }
        break;
    }
    return uncertainty;
}
datk::Set getBetaUncertainIndices(std::vector<std::vector<float>> &unlabelledFeatureVectors,int beta) {
    std::vector<std::pair<int, double>> predictionUncertainties;
    int index = 0;
    for (std::vector<std::vector<float>>::iterator it = unlabelledFeatureVectors.begin();
        it < unlabelledFeatureVectors.end();
        it++, index++) {
        datk::SparseFeature s;
        sparsifyFeatures(*it, s);
        datk::Vector predictions;
        ca->model->predictProbability(s, predictions);
        double uncertainty = getUncertainty(predictions, uncertaintyMode);
        std::cout << "Uncertainty is " << uncertainty << std::endl;
        predictionUncertainties.push_back(std::make_pair(index,
            uncertainty));
    }
    std::sort(predictionUncertainties.begin(), predictionUncertainties.end(),sortByUncertainty);
    datk::Set uncertainIndices;
    for (int i = 0; i < beta; i++) {
        uncertainIndices.insert(predictionUncertainties[i].first);
    }
    float lastUncertainty = predictionUncertainties[beta - 1].second;
    for (int i = beta; i < predictionUncertainties.size(); i++) {
        if (predictionUncertainties[i].second < lastUncertainty) {
            break;
        }
        uncertainIndices.insert(predictionUncertainties[i].first);
    }
    std::cout << "Asked for "
        << beta
        << " uncertain samples but gave "
        << uncertainIndices.size()
        << std::endl;
    return uncertainIndices;
}

void initalizeSeedForClass(std::vector<std::vector<float>> &featureVectorsOfAClass,
    int budget,
    int intLabel,
    std::vector<int> &seededIndicesOfAClass) {
    std::vector<int> possibilities;
    for (int i = 0; i < featureVectorsOfAClass.size(); i++) {
        possibilities.push_back(i);
    }
    //auto engine = std::default_random_engine{};
    std::random_shuffle(possibilities.begin(), possibilities.end());
    for (int i = 0; i < budget; i++) {
        seedFeatureVectors.push_back(featureVectorsOfAClass[possibilities[i]]);
        seedIntLabels.push_back(intLabel);
        seededIndicesOfAClass.push_back(possibilities[i]);
    }
}

datk::Set getBIndicesRandom(int n,
    double budget,
    datk::Set subsetIndices) {
    //for each feature vector i (0 to n), identify random featureVectors and populate indices
    datk::Set indices;
    std::vector<int> possibilities;
    for (int i = 0; i < n; i++) {
        if (subsetIndices.contains(i)) {
            possibilities.push_back(i);
        }
    }
    //auto engine = std::default_random_engine{};
    std::random_shuffle(possibilities.begin(), possibilities.end());
    for (int i = 0; i < budget; i++) {
        indices.insert(possibilities[i]);
    }
    return indices;
}

datk::Set getBIndicesFacilityLocation(std::vector<std::vector<float>> &featureVectors,
    double budget,
    datk::Set &subsetIndices) {
    //for each feature vector i (0 to n), compute its dot product with feature vector j (0 to n), update min, update max, store at k(i,j)
    datk::Set indices;
    double val = 0;
    double min = std::numeric_limits<double>::max();
    double max = std::numeric_limits<double>::min();
    std::map<int,int> indexMapping;
    int counter = 0;
    std::vector<std::vector<float>> kernel;
    for (int i=0; i < featureVectors.size(); i++) {
        if(!subsetIndices.contains(i)) {
            continue;
        }
        indexMapping[counter++] = i;
        std::vector<float> currvector;
        for (int j=0; j<featureVectors.size(); j++) {
            if(!subsetIndices.contains(j)) {
                continue;
            }
            std::vector<double> A(featureVectors.at(i).begin(),featureVectors.at(i).end());
            std::vector<double> B(featureVectors.at(j).begin(),featureVectors.at(j).end());
            val = datk::innerProduct(A,B);
            if(val < min) min = val;
            if(val > max) max = val;
            currvector.push_back(val);
        }
        kernel.push_back(currvector);
    }
    //0-1 normalize using min and max - this becomes the kernel
    double range = max-min;
    for (int i=0; i < kernel.size(); i++) {
        for (int j=0; j < kernel.size(); j++) {
            kernel[i][j] = (kernel[i][j]-min)/range;
        }
    }
    //instantiate facility location using kernel
    datk::FacilityLocation facLoc(kernel.size(), kernel);
    //call lazygreedymax with this facility location fuction, B, null output set
    datk::lazyGreedyMax(facLoc, budget, indices);

    datk::Set scaledIndices;
    for (datk::Set::iterator it = indices.begin();
        it != indices.end();
        it++) {
        scaledIndices.insert(indexMapping[*it]);
    }
    return scaledIndices;
}

datk::Set getBIndicesDisparityMin(std::vector<std::vector<float>> &featureVectors,
    double budget,
    datk::Set &subsetIndices) {
    //for each feature vector i (0 to n), compute its dot product with feature vector j (0 to n), update min, update max, store at k(i,j)
    datk::Set indices;
    double val = 0;
    double min = std::numeric_limits<double>::max();
    double max = std::numeric_limits<double>::min();
    std::vector<std::vector<float>> kernel;
    std::map<int,int> indexMapping;
    int counter = 0;
    for (int i=0; i < featureVectors.size(); i++) {
        if(!subsetIndices.contains(i)) {
            continue;
        }
        indexMapping[counter++] = i;
        std::vector<float> currvector;
        for (int j=0; j<featureVectors.size(); j++) {
            if(!subsetIndices.contains(j)) {
                continue;
            }
            std::vector<double> A(featureVectors.at(i).begin(),featureVectors.at(i).end());
            std::vector<double> B(featureVectors.at(j).begin(),featureVectors.at(j).end());
            val = datk::innerProduct( A,B);
            if(val < min) min = val;
            if(val > max) max = val;
            currvector.push_back(val);
        }
        kernel.push_back(currvector);
    }
    //0-1 normalize using min and max - this becomes the kernel
    double range = max-min;
    for (int i=0; i < kernel.size(); i++) {
        for (int j=0; j < kernel.size(); j++) {
            kernel[i][j] = (kernel[i][j]-min)/range;
        }
    }
    //instantiate facility location using kernel
    datk::DisparityMin dispMin(kernel.size(), kernel);
    //call lazygreedymax with this facility location fuction, B, null output set
    datk::naiveGreedyMax(dispMin, budget, indices, 0, false, true);
    datk::Set scaledIndices;
    for (datk::Set::iterator it = indices.begin();
        it != indices.end();
        it++) {
        scaledIndices.insert(indexMapping[*it]);
    }
    return scaledIndices;
}

datk::Set getBIndicesFeatureBasedFunctions(std::vector<std::vector<float>> &featureVectors,
    double budget,
    datk::Set &subsetIndices,
    int mode) {
    //for each feature vector i (0 to n), compute its sparse features
    datk::Set indices;
    std::vector<datk::SparseFeature> sparseFeatures = std::vector<datk::SparseFeature>();
    std::vector<double> featureWeights = std::vector<double>();
    for (int i = 0; i < featureVectors[0].size();i++) {
        featureWeights.push_back(1);
    }

    std::map<int,int> indexMapping;
    int counter = 0;
    for (int i = 0; i < featureVectors.size(); i++) {
        if(!subsetIndices.contains(i)) {
            continue;
        }
        indexMapping[counter++] = i;
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
    datk::FeatureBasedFunctions featBasedFunc(
        sparseFeatures.size(),
        mode,
        sparseFeatures,
        featureWeights);
    //call lazygreedymax with this Feature Based Functions fuction, B, null output set
    datk::lazyGreedyMax(featBasedFunc, budget, indices);
    datk::Set scaledIndices;
    for (datk::Set::iterator it = indices.begin();
        it != indices.end();
        it++) {
        scaledIndices.insert(indexMapping[*it]);
    }
    return scaledIndices;
}

datk::Set getBIndicesUncertaintySampling(int budget, datk::Set &subsetIndices) {
    datk::Set indices;
    datk::Set::iterator it;
    int i =0;
    for (it = subsetIndices.begin(); it != subsetIndices.end(); it++) {
        if (i == budget)
            break;
        indices.insert(*it);
        i++;
    }
    return indices;
}

int main(int argc, char *argv[]) {
    if (argc < 7) {
      std::cout << "usage: ./aiSaacActiveLearning [CaSettingsFilePath] [Beta%]"
        << " [B%] [path/to/output/folder/] [seedPercent%] [upperLimit%]"
        << " [randSeed(optional)] [uncertaintyMode-optional(0-argMax, 1-marginalSampline,2-Entropy)]"
        << std::endl;
      return -1;
    }
    auto seed = std::time(0);
    if (argc > 7) {
        seed = std::atoi(argv[7]);
    }
    uncertaintyMode = 2;
    if (argc > 8) {
        uncertaintyMode = std::atoi(argv[8]);
    }
    std::srand(seed);
    int totalSubsetFunctionsSize = 8;
    std::string caSettingsFilePath = argv[1];
    double betaPercent = std::atof(argv[2]);
    double bPercent = std::atof(argv[3]);
    float initialSeedPercentage = std::atof(argv[5]);
    double upperLimitPercent = std::atof(argv[6]);
    if (upperLimitPercent > 100) {
        upperLimitPercent = 100;
    }

    aiSaac::CASettings *caSettings = new aiSaac::CASettings(caSettingsFilePath);

    ca = new aiSaac::CustomAnalytics(caSettings);
    std::vector<std::vector<float>> fullTrainingFeatureVectors;
    datk::Vector fullTrainingIntLabels;

    //Load Training variables
    std::ifstream fTrain(ca->caSettings->getTrainingJsonDataPath());
    bool createdSomeJsonFile = false;
    if (!fTrain.good()) {
        fTrain.close();
        ca->extractFeatures(
            ca->caSettings->getTrainingDataPath(),
            fullTrainingFeatureVectors,
            fullTrainingIntLabels,
            ca->trainingStringToIntLabels,
            ca->trainingJsonData, true);
        std::cout
            << "extracted all features from train data.. going to save to JSON: "
            << ca->trainingJsonData.size()
            << std::endl << std::flush;
        ca->saveData(ca->caSettings->getTrainingJsonDataPath(), ca->trainingJsonData);
        createdSomeJsonFile = true;
    }
    fullTrainingFeatureVectors.clear();
    fullTrainingIntLabels.clear();
    ca->loadData(ca->caSettings->getTrainingJsonDataPath(), ca->trainingJsonData);
    ca->trainingFeatureVectors.clear();
    ca->trainingIntLabels.clear();
    ca->trainingStringToIntLabels.clear();
    int intLabel = 0;
    for (nlohmann::json::iterator theClass = ca->trainingJsonData.begin();
        theClass != ca->trainingJsonData.end();
        theClass++) {
        std::cout << "theClass.key: " << theClass.key() << std::endl;
        ca->trainingStringToIntLabels.push_back(std::make_pair(intLabel,
            theClass.key().c_str()));
        std::vector<std::vector<float>> featureVectorsOfAClass;
        for (nlohmann::json::iterator storedFeature = theClass.value().begin();
            storedFeature != theClass.value().end();
            storedFeature++) {
            fullTrainingFeatureVectors.push_back(*storedFeature);
            featureVectorsOfAClass.push_back(*storedFeature);
            fullTrainingIntLabels.push_back(intLabel);
        }
        int budget = ceil((initialSeedPercentage*featureVectorsOfAClass.size())/100);
        std::vector<int> seededIndicesOfAClass;
        initalizeSeedForClass(featureVectorsOfAClass,
            budget,
            intLabel,
            seededIndicesOfAClass);
        std::sort(seededIndicesOfAClass.begin(), seededIndicesOfAClass.end(), std::greater<int>());
        int removeIndex = fullTrainingFeatureVectors.size()
            - featureVectorsOfAClass.size();
        for (int it = 0; it < seededIndicesOfAClass.size(); it++) {
            int currentRemoveIndex = removeIndex + seededIndicesOfAClass[it];
            fullTrainingFeatureVectors.erase(fullTrainingFeatureVectors.begin()+currentRemoveIndex);
            fullTrainingIntLabels.erase(fullTrainingIntLabels.begin()+currentRemoveIndex);
        }
        intLabel++;
    }
    //Load Testing variables
    std::ifstream fTest(ca->caSettings->getTestingJsonDataPath());
    if (!fTest.good()) {
        fTest.close();
        ca->extractFeatures(
            ca->caSettings->getTestingDataPath(),
            ca->testingFeatureVectors,
            ca->testingIntLabels,
            ca->testingStringToIntLabels,
            ca->testingJsonData, true);
        std::cout
            << "extracted all features from test data.. going to save to JSON: "
            << ca->testingJsonData.size()
            << std::endl << std::flush;
        ca->saveData(ca->caSettings->getTestingJsonDataPath(), ca->testingJsonData);
        createdSomeJsonFile = true;
    }
    if(createdSomeJsonFile) {
        std::cout << "Just wrote a Json file to filesystem. Cannot start the experiments right now due to ordering issues. Please rerun." << std::endl;
        return 0;
    }
    ca->loadData(ca->caSettings->getTestingJsonDataPath(), ca->testingJsonData);
    intLabel = 0;
    for (nlohmann::json::iterator theClass = ca->testingJsonData.begin();
        theClass != ca->testingJsonData.end();
        theClass++) {
        ca->testingStringToIntLabels.push_back(std::make_pair(intLabel,
            theClass.key().c_str()));
        for (nlohmann::json::iterator storedFeature = theClass.value().begin();
            storedFeature != theClass.value().end();
            storedFeature++) {
            ca->testingFeatureVectors.push_back(*storedFeature);
            ca->testingIntLabels.push_back(intLabel);
        }
        intLabel++;
    }
    datk::Vector testingIntLabels = ca->testingIntLabels;


    int T = floor(upperLimitPercent/bPercent);
    if (T < 1)
        T =1;
    int beta = ceil((betaPercent*fullTrainingFeatureVectors.size())/100);
    int b = ceil((bPercent*fullTrainingFeatureVectors.size())/100);
    if (b > beta) {
        beta = b;
    }

    std::cout << "Debug UpperLimitPercent " << upperLimitPercent <<std::endl << std::flush;
    std::cout << "Debug bPercent " << bPercent <<std::endl << std::flush;
    std::cout << "Debug b " << b <<std::endl << std::flush;
    std::cout << "Debug T " << T <<std::endl << std::flush;
    std::cout << "Debug betaPercent " << betaPercent <<std::endl << std::flush;
    std::cout << "Debug beta " << beta <<std::endl << std::flush;
    std::string str = caSettings->getTrainingDataPath();
    std::size_t found = str.find_last_of("/\\");
    std::string dataSetPath = str.substr(0,found);
    found = dataSetPath.find_last_of("/\\");
    std::string dataSetName = dataSetPath.substr(found+1);
    std::string resultsFileName("goal2-" + dataSetName + ".csv");

    //preparing output csv file
    std::cout << "Preparing results csv file ..." << std::endl;

    ofstream csvFile;
    csvFile.open(std::string(argv[4]) + "/" + resultsFileName);
    csvFile << "Goal 2 - Active Learning with transfer learning\n" << std::flush;
    csvFile << "Training Dataset," << ca->caSettings->getTrainingDataPath() << std::endl << std::flush;
    csvFile << "Testing Dataset," << ca->caSettings->getTestingDataPath() << std::endl << std::flush;
    csvFile << "Training data features json path," << caSettings->getTrainingJsonDataPath() << std::endl << std::flush;
    csvFile << "Testing data features json path," << caSettings->getTestingJsonDataPath() << std::endl << std::flush;
    csvFile << "Caffe feature extraction layer," << caSettings->getCaffeFeatureExtractionLayer() << std::endl << std::flush;
    csvFile << "Pre-trained Model ," << ca->caSettings->getCaffeTrainedFilePath() << std::endl << std::flush;
    csvFile << "Total Training data ,"
        << (seedFeatureVectors.size()+fullTrainingFeatureVectors.size()) << std::endl << std::flush;
    csvFile << "Initial Seed% ," << initialSeedPercentage << std::endl << std::flush;
    csvFile << "Initial Seed ," << seedFeatureVectors.size() << std::endl << std::flush;
    csvFile << "Remaining Unlabelled Data ," << fullTrainingFeatureVectors.size() << std::endl << std::flush;
    csvFile << "Initial Beta% ," << betaPercent << std::endl << std::flush;
    csvFile << "Initial Beta ," << beta << std::endl << std::flush;
    csvFile << "Initial B% ," << bPercent << std::endl << std::flush;
    csvFile << "Initial B ," << b << std::endl << std::flush;
    csvFile << "Seed," << seed << std::endl << std::flush;
    csvFile << "Uncertainty selection Mode," << uncertaintyMode << std::endl << std::flush;
    csvFile << "DATK Threshold," << ca->caSettings->getDATKPredictionProbThresh() << std::endl << std::flush;
    csvFile << "\n\n";
    csvFile << "Mode\\t,";
    for (int t = 0; t<=T ; t++) {
        csvFile << t << ",";
    }
    for (int mode = 0; mode < totalSubsetFunctionsSize; mode++) {
        csvFile << std::endl << std::flush;
        switch (mode) {
            case 0:
                csvFile << "Random selection" << std::flush;
                break;
            case 1:
                csvFile << "FASS+RS" << std::flush;
                break;
            case 2:
                csvFile << "FASS with SMF1 = FacilityLocation" << std::flush;
                break;
            case 3:
                csvFile << "FASS with SMF2  = Disparity Min" << std::flush;
                break;
            case 4:
                csvFile << "FASS with SMF3 = Feature Based Functions with sqrt. over modular" << std::flush;
                break;
            case 5:
                csvFile << "FASS with SMF3 = Feature Based Functions with inverse function" << std::flush;
                break;
            case 6:
                csvFile << "FASS with SMF3 = Feature Based Functions with log function" << std::flush;
                break;
            case 7:
                csvFile << "Uncertainty Sampling" << std::flush;
                break;
        }
        ca->trainingIntLabels = seedIntLabels;
        ca->trainingFeatureVectors = seedFeatureVectors;
        std::vector<std::vector<float>> unlabeledTrainingFeatureVectors = fullTrainingFeatureVectors;
        datk::Vector unlabeledTrainingIntLabels = fullTrainingIntLabels;
        datk::Vector trainingIntLabels = seedIntLabels;
        for (int t = 1; t <= T; t++) {
            ca->trainingIntLabels = trainingIntLabels;
            ca->testingIntLabels = testingIntLabels;
            std::cout << "___________________________________________________________________" << std::endl;
            std::cout << "DEBUG Trainingfeatvector.size() " << ca->trainingFeatureVectors.size() << std::endl;
            std::cout << "DEBUG TrainingIntLabels.size() " << ca->trainingIntLabels.size() << std::endl;
            std::cout << "DEBUG trainingStringToIntLabels.size() " << ca->trainingStringToIntLabels.size() << std::endl;
            int *numCorrect = (int *) malloc(sizeof(int));
            int *numTotal = (int *) malloc(sizeof(int));
            ca->train(false);
            ca->test(false, numCorrect, numTotal);
            csvFile << ",=" << *numCorrect << "/" << *numTotal << std::flush;
            if (unlabeledTrainingFeatureVectors.size() == 0) {
                break;
            }
            int maxBeta = beta;
            if (beta > unlabeledTrainingFeatureVectors.size() ) {
                maxBeta = unlabeledTrainingFeatureVectors.size();
            }
            int maxB = b;
            if (b > unlabeledTrainingFeatureVectors.size() ) {
                maxB = unlabeledTrainingFeatureVectors.size();
            }
            datk::Set uncertainIndices = getBetaUncertainIndices(unlabeledTrainingFeatureVectors, maxBeta);
            datk::Set indicesToTrain;
            switch(mode) {
                case 0: {
                    std::cout << "Using random mode" << std::endl;
                    datk::Set allIndices;
                    for (int i = 0; i < unlabeledTrainingFeatureVectors.size(); i++) {
                        allIndices.insert(i);
                    }
                    indicesToTrain = getBIndicesRandom(
                        unlabeledTrainingFeatureVectors.size(),
                        maxB,
                        allIndices);
                    break;
                }
                case 1:
                    std::cout << "Using FASS+random mode" << std::endl;
                    indicesToTrain = getBIndicesRandom(
                        unlabeledTrainingFeatureVectors.size(),
                        maxB,
                        uncertainIndices);
                    break;
                case 2:
                    std::cout << "Using facility location" << std::endl;
                    indicesToTrain = getBIndicesFacilityLocation(
                        unlabeledTrainingFeatureVectors,
                        maxB,
                        uncertainIndices);
                    break;
                case 3:
                    std::cout << "Using Disparity Min" << std::endl;
                    indicesToTrain = getBIndicesDisparityMin(
                        unlabeledTrainingFeatureVectors,
                        maxB,
                        uncertainIndices);
                    break;
                case 4:
                    std::cout
                        << "Using Feature Based Functions with sqrt over modular"
                        << std::endl;
                    indicesToTrain = getBIndicesFeatureBasedFunctions(
                        unlabeledTrainingFeatureVectors,
                        maxB,
                        uncertainIndices,
                        1);
                    break;
                case 5:
                    std::cout
                        << "Using Feature Based Functions with inverse function"
                        << std::endl;
                    indicesToTrain = getBIndicesFeatureBasedFunctions(
                        unlabeledTrainingFeatureVectors,
                        maxB,
                        uncertainIndices,
                        2);
                    break;
                case 6:
                    std::cout
                        << "Using Feature Based Functions with Log function"
                        << std::endl;
                    indicesToTrain = getBIndicesFeatureBasedFunctions(
                        unlabeledTrainingFeatureVectors,
                        maxB,
                        uncertainIndices,
                        3);
                    break;
                case 7:
                    std::cout
                        << "Using Uncertainty Sampling"
                        << std::endl;
                    indicesToTrain = getBIndicesUncertaintySampling(
                        maxB,
                        uncertainIndices);
                    break;
                default:
                    std::cout << "no submodular function found" << std::endl;
                    return -1;
            }
            datk::Set::iterator it;
            std::vector<int> indiciesToDelete;
            for (it = indicesToTrain.begin(); it != indicesToTrain.end(); ++it) {
                trainingIntLabels.push_back(unlabeledTrainingIntLabels[*it]);
                ca->trainingFeatureVectors.push_back(unlabeledTrainingFeatureVectors[*it]);
                indiciesToDelete.push_back(*it);
            }
            std::sort(indiciesToDelete.begin(), indiciesToDelete.end(), std::greater<int>());
            for(int i = 0; i < indiciesToDelete.size(); i++) {
                unlabeledTrainingFeatureVectors.erase(unlabeledTrainingFeatureVectors.begin()+indiciesToDelete[i]);
                unlabeledTrainingIntLabels.erase(unlabeledTrainingIntLabels.begin()+indiciesToDelete[i]);
            }
            std::cout << "t b budget mode " << t << " " << b << " " << beta << " "  << mode << std::endl;
            std::cout << "___________________________________________________________________" << std::endl;
        }
        ca->trainingIntLabels = trainingIntLabels;
        ca->testingIntLabels = testingIntLabels;
        int *numCorrectFinal = (int *) malloc(sizeof(int));
        int *numTotalFinal = (int *) malloc(sizeof(int));
        ca->train(false);
        ca->test(false, numCorrectFinal, numTotalFinal);
        csvFile << ",=" << *numCorrectFinal << "/" << *numTotalFinal << std::flush;
    }
    csvFile.close();
}
