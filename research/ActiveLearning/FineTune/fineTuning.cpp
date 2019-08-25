#include <iostream>
#include <vector>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <fstream>
#include <ctime>
#include "../../../src/aiSaac.h"
#include "../../../src/utils/datk/src/datk.h"

std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
    if (!pipe) throw std::runtime_error("popen() failed!");
    while (!feof(pipe.get())) {
        if (fgets(buffer.data(), 128, pipe.get()) != NULL)
            result += buffer.data();
    }
    return result;
}


string getFileName(const string& s) {

   char sep = '/';
   std::size_t i = s.rfind(sep, s.length());
   if (i != string::npos) {
      return(s.substr(i+1, s.length() - i));
   }

   return("");
}

aiSaac::CustomAnalytics *ca;
std::vector<std::vector <float>> seedFeatureVectors;
datk::Vector seedIntLabels;

void sparsifyFeatures(std::vector<float> &featureVector, datk::SparseFeature s) {
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

bool sortByProbability(const std::pair<int,float> &a, const std::pair<int,float> &b) {
    return a.second < b.second;
}

datk::Set getBetaUncertainIndices(std::vector<std::vector<float>> &unlabelledFeatureVectors,int beta) {
    std::vector<std::pair<int, float>> predictionProbabilities;
    int index =0;
    for (std::vector<std::vector<float>>::iterator it = unlabelledFeatureVectors.begin();
        it < unlabelledFeatureVectors.end();
        it++, index++) {
        datk::SparseFeature s;
        sparsifyFeatures(*it, s);
        datk::Vector predictions;
        ca->model->predictProbability(s, predictions);
        int predictionMax = datk::argMax(predictions);
        predictionProbabilities.push_back(std::make_pair(index,
            predictions[predictionMax]));
    }
    std::sort(predictionProbabilities.begin(), predictionProbabilities.end(),sortByProbability);
    datk::Set uncertainIndices;
    for (int i = 0; i < beta; i++) {
        uncertainIndices.insert(predictionProbabilities[i].first);
    }
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

bool sortByUncertainty(const std::pair<int,float> &a, const std::pair<int,float> &b) {
    return a.second > b.second;
}

float getUncertainty( std::vector<std::pair<std::string, float>> &results) {
    float uncertainty;
    datk::Vector predictions;
    for(int i=0;i<results.size();i++){
      predictions.push_back(results[i].second);
    }

    datk::Vector predCopy = predictions;
    int maxIndex = datk::argMax(predCopy);
    float maxProbability = predCopy[maxIndex];
    predCopy.erase(predCopy.begin() + maxIndex);
    float secondMax = predCopy[datk::argMax(predCopy)];
    float difference = maxProbability - secondMax;
    uncertainty = 1 - difference;
    return uncertainty;
}


datk::Set getBIndicesRandom(int n,
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

datk::Set getBIndicesFacilityLocation(int n,
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

datk::Set getBIndicesDisparityMin(int n,
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

datk::Set getBIndicesFeatureBasedFunctions(int n,
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

//TODO: write in csv
//TODO: take seed for random  ---Done. please verify
int main(int argc, char *argv[]) {
    if (argc < 5) {
      std::cout << "usage: ./aiSaacActiveLearning [CaSettingsFilePath] [Beta%] [B%] [path/to/csv/research/folder] [randomSeedInitializer]" << std::endl;
      return -1;
    }

    std::srand(std::time(0));
    int totalSubsetFunctionsSize = 8;
    float initialSeedPercentage = 1;   //TODO: keep it 1% as initial but make it dynamic. ----Done. please verify
    std::string caSettingsFilePath = argv[1];
    double betaPercent = std::atof(argv[2]);
    double bPercent = std::atof(argv[3]);
    std::string outputFilePath = argv[4];
    unsigned randomSeed;
    if (argc < 6){
      randomSeed = unsigned ( std::time(0) );
    }
    else{
      randomSeed = unsigned(std::atoi(argv[5]));
    }


	std::srand ( randomSeed );
    std::cout<<"ramdom seed = "<<randomSeed<<std::endl;
    aiSaac::CASettings *caSettings = new aiSaac::CASettings(caSettingsFilePath);
    ca = new aiSaac::CustomAnalytics(caSettings);
    std::string workDir=ca->caSettings->getWorkingDir();
    std::cout << "working directory is " + workDir << std ::endl;
    std::string command;
    clock_t begins;
    clock_t ends;
    double  elapsed_secs;


   std::string str = caSettings->getTrainingDataPath();
    std::size_t found = str.find_last_of("/\\");
    std::string dataSetPath = str.substr(0,found);
    found = dataSetPath.find_last_of("/\\");
    std::string dataSetName = dataSetPath.substr(found+1);
    std::string resultsFileName("goal2-FIneTuning -" + dataSetName + ".csv");

    //global variables to hold information-start
    std::map<int,std::vector<float>> globalIdFullFeatureVectors;
    std::map<int, int> globalIdFullLabels;
    std::map<int,std::string> globalIdFullFileNames;
    std::map<int,std::string> int2StrLabel;
    //global variables to hold information-end

    std::cout << "populating global information" << std::endl;


    globalIdFullFeatureVectors.clear();
    globalIdFullLabels.clear();
    globalIdFullFileNames.clear();
    int2StrLabel.clear();



    std::vector<std::vector<float>> fullTrainingFeatureVectors;
    datk::Vector fullTrainingIntLabels;
    std::vector<std::pair<int, std::string>> fullTrainingStringToIntLabels;
    nlohmann::json fullTrainJsonData;
    nlohmann::json fullTrainFileNamesJsonData;
    std::cout << "Full training data json and file names json file exists, loading them in memory." << std::endl;
    std::ifstream f(caSettings->getTrainingJsonDataPath());
    std::ifstream fFileNames(caSettings->getTrainingFileNamesJsonDataPath());
    if (f.good() && fFileNames.good()) {
        f.close();
        fFileNames.close();
        begins = clock();
        ca->loadData(caSettings->getTrainingJsonDataPath(), fullTrainJsonData);
        ca->loadData(caSettings->getTrainingFileNamesJsonDataPath(), fullTrainFileNamesJsonData);
        ends = clock();
        elapsed_secs = double(ends - begins) / CLOCKS_PER_SEC;
        std::cout << "Time taken to load train json and file names= " << std::to_string(elapsed_secs) << " seconds." << std::endl << std::flush;
        std::cout << "Loaded full train json data in memory" << std::endl;
    }
    else{
      std::cout << "ERROR.. !!! Full training data json or file names json file doesn't exists, " << std::endl;
      return 0;
    }

    ca->trainingFeatureVectors.clear();
    ca->trainingIntLabels.clear();
    ca->trainingStringToIntLabels.clear();

    int intLabel = 0;
    std::string strLabel;
    int fullTrainJsonDataSize = 0;
    int gId=0;
    //for every class

    begins = clock();
    for (nlohmann::json::iterator theClass = fullTrainJsonData.begin(), theClassFileName = fullTrainFileNamesJsonData.begin(); theClass != fullTrainJsonData.end(), theClassFileName != fullTrainFileNamesJsonData.end(); theClass++, theClassFileName++) {
        //get features of all images of this class and store in a vector of vectors (featureVectorsOfAClass)
        //std::cout << "theClass.key: " << theClass.key() << std::endl;
        strLabel = theClass.key().c_str();
        int2StrLabel.insert(std::make_pair(intLabel,strLabel));
        ca->trainingStringToIntLabels.push_back(std::make_pair(intLabel, strLabel));
        for (nlohmann::json::iterator storedFeature = theClass.value().begin(), storedFileName = theClassFileName.value().begin(); storedFeature != theClass.value().end(), storedFileName != theClassFileName.value().end(); storedFeature++, storedFileName++) {
            fullTrainJsonDataSize += theClass.value().size();
            globalIdFullFileNames.insert(std::make_pair(gId,getFileName(*storedFileName)));
            std::vector<float> v = *storedFeature;
            globalIdFullFeatureVectors.insert(std::make_pair(gId,v));
            globalIdFullLabels.insert(std::make_pair(gId,intLabel));
            //std::cout << "File name: " << *storedFileName << std::endl;
            gId++;
        }
        intLabel++;

      }
      ends = clock();
      elapsed_secs = double(ends - begins) / CLOCKS_PER_SEC;
      std::cout << "Time taken for populating Global id Maps = " << std::to_string(elapsed_secs) << " seconds." << std::endl << std::flush;


	ofstream resultsFile;
    resultsFile.open(std::string(argv[4]) + "/" + resultsFileName);
    resultsFile << "Train data path," << caSettings->getTrainingDataPath() << std::endl << std::flush;
    resultsFile << "Test data path," << caSettings->getTestingDataPath() << std::endl << std::flush;
    resultsFile << "Label file path," << caSettings->getLabelFilePath() << std::endl << std::flush;
    resultsFile << "Caffe network file path," << caSettings->getCaffeNetworkFilePath() << std::endl << std::flush;
    resultsFile << "Caffe trained model path," << caSettings->getCaffeTrainedFilePath() << std::endl << std::flush;
    resultsFile << "Training data features json path," << caSettings->getTrainingJsonDataPath() << std::endl << std::flush;
    resultsFile << "Training data file names json path," << caSettings->getTrainingFileNamesJsonDataPath() << std::endl << std::flush;
    resultsFile << "Testing data features json path," << caSettings->getTestingJsonDataPath() << std::endl << std::flush;
    resultsFile << "Caffe feature extraction layer," << caSettings->getCaffeFeatureExtractionLayer() << std::endl << std::flush;
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
	resultsFile << "Total Training data ,"
        << (globalIdFullFeatureVectors.size()) << std::endl << std::flush;
    resultsFile << "Initial Seed% ," << initialSeedPercentage << std::endl << std::flush;
    resultsFile << "Initial Seed ," << seedFeatureVectors.size() << std::endl << std::flush;
    //resultsFile << "Remaining Unlabelled Data ," << fullTrainingFeatureVectors.size() << std::endl << std::flush;
    //resultsFile << "Seed," << seed << std::endl << std::flush;


  int T = floor(100/bPercent);
    if (T < 1)
        T =1;
    int beta = ceil((betaPercent*globalIdFullFeatureVectors.size())/100);
    int b = ceil((bPercent*globalIdFullFeatureVectors.size())/100);
    if (b > beta) {
        beta = b;
    }
	 resultsFile << "Initial Beta% ," << betaPercent << std::endl << std::flush;
    resultsFile << "Initial Beta ," << beta << std::endl << std::flush;
    resultsFile << "Initial B% ," << bPercent << std::endl << std::flush;
    resultsFile << "Initial B ," << b << std::endl << std::flush;
	resultsFile << "\n\n";
    resultsFile << "Mode\\t,";
	 for (int t = 0; t<=T ; t++) {
        resultsFile << t << ",";
    }


  //    for each submodular function, do following
  //------------------------------------------------------------
  for(int submod=0;submod< totalSubsetFunctionsSize;submod++){
	  resultsFile << std::endl << std::flush;
        switch (submod) {
            case 0:
                resultsFile << "FASS+RS" << std::flush;
                break;
            case 1:
                resultsFile << "FASS with SMF1 = FacilityLocation" << std::flush;
                break;
            case 2:
                resultsFile << "FASS with SMF2  = Disparity Min" << std::flush;
                break;
            case 3:
                resultsFile << "FASS with SMF3 = Feature Based Functions with sqrt. over modular" << std::flush;
                break;
            case 4:
                resultsFile << "FASS with SMF3 = Feature Based Functions with inverse function" << std::flush;
                break;
            case 5:
                resultsFile << "FASS with SMF3 = Feature Based Functions with log function" << std::flush;
                break;
            case 6:
                resultsFile << "Uncertainty Sampling" << std::flush;
                break;
            case 7:
                resultsFile << "Random selection" << std::flush;
                break;
        }

    std::cout << "Submodular Function " <<submod<< std::endl;

    int ufCount=0;
    int lfCount=0;

    //bool extract_features=false;
    std::vector<std::vector<int>> uf;
    std::vector<std::vector<int>> lf;

    //initialize uf and lf
    for(int i=0; i < int2StrLabel.size();i++){
      std::vector<int> a;
      uf.push_back(a);
      lf.push_back(a);
    }
    for(std::map<int,int>::iterator it = globalIdFullLabels.begin(); it != globalIdFullLabels.end();it++){
      uf[it->second].push_back(it->first);
      ufCount++;
    }


    // creating folders for UF and initializing it with original train datapath
    //create copy of training root folder as UF
    begins = clock();
    std::cout << "Creating parent folder for full initial training data" << std::endl;
    command = "mkdir -p \"" + workDir + "\"/data/goal2-ft-" + std::to_string(submod) + "-" + std::to_string(int(betaPercent)) + "/";
    system(command.c_str());
    //TODO : delete train folders after experiment is done.
    std::cout << "Copying full training data from " +  ca->caSettings->getTrainingDataPath() +"to  " + workDir + "/data/goal2-ft-" + std::to_string(submod) + "-" + std::to_string(int(betaPercent)) + "/" << std::endl;
    command = "cp -r \"" + ca->caSettings->getTrainingDataPath() + "\" \"" + workDir + "\"/data/goal2-ft-" + std::to_string(submod) + "-" + std::to_string(int(betaPercent)) + "/";
    system(command.c_str());
    std::cout << "Copying training data done" <<std::endl;

    std::cout << "" << std::endl;

    // creating folders for LF and initializing it as empty
    std::cout << "Creating labeled data folder for seed initial seed data" << std::endl;

    for(int p=0; p< lf.size();p++){
      //todo: handle space in names every where
      command = "mkdir -p \"" + workDir + "\"/data/goal2-ft-labeled" + std::to_string(submod) + "-" + std::to_string(int(betaPercent)) + "/Train/\"" + int2StrLabel[p] + "\"/";
      system(command.c_str());
    }
    std::cout << "created labeled data folder for seed data " << std::endl;
    /*till here folder data and UF and LF datastructure are in sync*/
    ends = clock();
    elapsed_secs = double(ends - begins) / CLOCKS_PER_SEC;
    std::cout << "Time taken for creating UF and LF with folders= " << std::to_string(elapsed_secs) << " seconds." << std::endl << std::flush;


    std::cout << "work on seed data" << std::endl;
    begins = clock();

    //set initialSeedPercentage deterministically and dynamically based on the available data. as caffe trainer fails to create lmdb if the data size per class is less than 2. So,here, we need to make sure that we have enough seed data.
    int minDataPerClass=uf[0].size(); //just initializing
    for(int i=0; i < uf.size();i++){
        if(minDataPerClass>uf[i].size()) minDataPerClass = uf[i].size();
    }
    int reqP = ceil(2*100.0/minDataPerClass);
    if( initialSeedPercentage < reqP ) initialSeedPercentage = reqP;

    //work on seed data
    // here i is class label
    for(int i=0; i < uf.size();i++){
      int budget = ceil(uf[i].size()*initialSeedPercentage/100);
      std::vector<int> arr = uf[i];
      random_shuffle(arr.begin(),arr.end());
      for(int j = 0; j < budget; j++){
        int glId = arr[j];
        std::vector<int>::iterator position = std::find(uf[i].begin(), uf[i].end(), glId);
        uf[i].erase(position);
        lf[i].push_back(glId);
        ufCount--;
        lfCount++;
        //manage images in folders accordingly
        std::cout << "Moving seed data from " + workDir + "/data/goal2-ft-" + std::to_string(submod) + "-" + std::to_string(int(betaPercent)) + "/Train/" + int2StrLabel[i] +  " to" + workDir + "/data/goal2-ft-labeled" + std::to_string(submod) + "-" + std::to_string(int(betaPercent)) + "/Train/" + int2StrLabel[i] +" " << std::endl;
        command = "mv \"" +workDir + "\"/data/goal2-ft-" + std::to_string(submod) + "-" + std::to_string(int(betaPercent)) + "/Train/\""  + int2StrLabel[i] + "\"/\"" + globalIdFullFileNames[glId] + "\" \"" +workDir + "\"/data/goal2-ft-labeled" + std::to_string(submod) + "-" + std::to_string(int(betaPercent)) + "/Train/\""  + int2StrLabel[i] + "\"/";
        system(command.c_str());
        std::cout << "moving seed data from unlabeled to labeled training data done" <<std::endl;

      }

    std::cout <<"Moved " << lf[i].size() <<  "images in total from unlabeled to labeled folder of " + int2StrLabel[i] + " " << std::endl;
    }
    std::cout <<"Moved "  <<lfCount << "images in total from unlabeled to labeled folder" << std::endl;
    ends = clock();
    elapsed_secs = double(ends - begins) / CLOCKS_PER_SEC;
    std::cout << "Time taken for working with seed data = " << std::to_string(elapsed_secs) << " seconds." << std::endl << std::flush;


    std::cout<<"Active learning iteration"<<std::endl;


    //T=2; //: remove this. only for testing purpose. ask vishal regarding if we need to do it for all t (almost 20 values) or some of them . change the loop accordingly

    std::string trained_file=ca->caSettings->getOriginalCaffeModelPath(); //"originalCaffemodelPath": "/home/pankaj/Research/aisaac-iitb-2/build/storageDir/caffeFiles/face/VGG_FACE.caffemodel",
    std::string mean_file;
    std::string label_file;

    for (int t = 1; t<=T ; t++) {
      std::cout << "remove database files and model files" << std::endl;
      command = "rm -rf \""+workDir+"\"/*.caffemodel";
      system(command.c_str());
      command = "rm -rf \""+workDir+"\"/database/";
      system(command.c_str());


      std::cout<<"Active learning iteration number: " << t <<std::endl;


      begins = clock();
      cout<<"Size of first class of lf : "<<lf[0].size()<<std::endl;
      aiSaac::CaffeTrainer caffeTrainer(workDir+ "/data/goal2-ft-labeled" + std::to_string(submod) + "-" + std::to_string(int(betaPercent)) + "/Train/",
                      ca->caSettings->getTestingDataPath(),
                      ca->caSettings->getCaffeTemplateArchitecturePath(),
                      ca->caSettings->getCaffeTemplateSolverPath(),
                      ca->caSettings->getOriginalCaffeModelPath(),
                      ca->caSettings->getSolverStatePath(),
                      ca->caSettings->getWorkingDir(),
                      //ca->caSettings->getNewNetworkName(),
                      "goal2-ft-"+std::to_string(submod)+"-"+std::to_string(t)+"-"+std::to_string(betaPercent),
                      ca->caSettings->getBackpropOptimizationAlgo(),
                      ca->caSettings->getMaxIterations(),
                      ca->caSettings->getTestIterationSize(),
                      ca->caSettings->getStepSize(),
                      ca->caSettings->getBaseLR(),
                      ca->caSettings->getSolverMode()); //cpu - 0
      ends = clock();
      elapsed_secs = double(ends - begins) / CLOCKS_PER_SEC;
      std::cout << "Time taken for creating caffe trainer = " << std::to_string(elapsed_secs) << " seconds." << std::endl << std::flush;


      std::cout << "Started training Caffe trainer" << std::endl;
      begins = clock();
      caffeTrainer.train(); // originalCafemodel, template prototxt and template solver (do not touch these files)
      ends = clock();
      elapsed_secs = double(ends - begins) / CLOCKS_PER_SEC;
      std::cout << "Time taken for Training the caffe trainer = " << std::to_string(elapsed_secs) << " seconds." << std::endl << std::flush;

      std::cout << "Caffe trainer ended" << std::endl;

      std::cout << "getting the latest model file" << std::endl;
      //TODO: confirm the path where you create caffe model files
      //command = "ls -Art \""+workDir+"\"/*.caffemodel | tail -n1";
      command = "ls -Art \""+workDir+"\"/*.caffemodel | tail -n1";
      trained_file=exec(command.c_str());
      //erasing newline character from the end.
      trained_file.erase(std::remove(trained_file.begin(), trained_file.end(), '\n'), trained_file.end());

      mean_file = workDir+"/model/mean.binaryproto";
      label_file=workDir+"/model/labels.txt";

      ifstream file(trained_file);
      if (!file.good())
      {
          // Can't open file
          std::cout << "Can't find file! " << trained_file<<std::endl;

      }

      //TODO: the above method of computing the trained file path is not usable as it gives error when you try ot test file.good(). Even though the output of ls command is correct. BUt it may be because of some NON-PRINTABLE character etc.... Please handle it.
      //trained_file="/home/pankaj/Research/aisaac-iitb-2/build/storageDir/fineTuning/vgg_face-facedatateam/vggface_aitoefacedata2_iter_2.caffemodel"; //snapshotprefix+_iter_+maxIteration.caffe
      cout <<"the latest trained file is : "<<trained_file<<endl;
      begins = clock();
      //TODO: make sure that ca->caSettings->getCaffeNetworkFilePath() file has same number of output layer as the nunmber of classes in the train dataset. You need to hardcode it in prototext for every experiment.
      std::string deploy_file = "/home/suyash/workspace/aisaac/aisaac-fork/build/storageDir/fineTuning/Goal2CNNVggFaceFaceData2/model/goal2-ft-0-1-10.000000_deploy.prototxt";
      aiSaac::CaffeClassifier C(deploy_file,trained_file,mean_file,label_file);
      std::cout << "Caffe classifier parameters:\n";
      std::cout << deploy_file << std::endl << trained_file << std::endl << mean_file << std::endl << label_file << std::endl;
      std::cout << "computing uncertainty vector uv over UF" << std::endl;
      //compute uncertainty vector uv over UF
      std::vector<std::pair<int,float>> uv;
      //uci is class index
      for(int uci=0; uci < uf.size();uci++){
        for(int ui=0;ui<uf[uci].size();ui++){
          std::string imgPath = workDir + "/data/goal2-ft-" + std::to_string(submod) + "-" + std::to_string(int(betaPercent)) + "/Train/"  + int2StrLabel[uci] + "/" + globalIdFullFileNames[uf[uci][ui]];
          std::cout << "image path: " << imgPath << std::endl;
          cv::imshow("image", cv::imread(imgPath));

          std::vector<std::pair<std::string, float>> result;
          result = C.Classify(cv::imread(imgPath),int2StrLabel.size());
          std::cout << "result returned\n";
          uv.push_back(std::make_pair(uf[uci][ui],getUncertainty(result)));
        }
      }
      ends = clock();
      elapsed_secs = double(ends - begins) / CLOCKS_PER_SEC;
      std::cout << "Time taken for creating classifer and creating uncertainty vector after classifying UF = " << std::to_string(elapsed_secs) << " seconds." << std::endl << std::flush;

      //handling the corner case, if beta is greater than UL size
      int maxBeta = beta;
      if (beta > ufCount ) {
        std::cout << "handling the corner case, if beta is greater than UL size" << std::endl;

          maxBeta = ufCount;
      }
      int maxB = b;
      if (b > ufCount ) {
        std::cout << "handling the corner case, if b is greater than UL size" << std::endl;

          maxB = ufCount;
      }

      begins = clock();
      std::cout << "sort uv" << std::endl;
      //sort uv
      std::sort(uv.begin(), uv.end(),sortByUncertainty);

      std::cout << "pick top beta uncertainGlobalIndices. here uncertainIndices are set of global Ids" << std::endl;

      //pick top beta uncertainGlobalIndices. here uncertainIndices are set of global Ids
      datk::Set uncertainIndices;
      //std::vector<int> v;
      for (int k = 0; k < maxBeta; k++) {
          uncertainIndices.insert(uv[k].first);
      }



      //create uncertainFeatureVectors

      std::vector<std::vector<float>> uncertainFeatureVectors;
      // key-> index in uncertainFeatureVectors and value -> gloabalID
      std::map<int,int> findex2Id;
      //iterating over uncertainIndices and populating uncertainFeatureVectors and findex2Id;
      int c=0;
      for(datk::Set::iterator it = uncertainIndices.begin(); it != uncertainIndices.end(); it++ ){
        uncertainFeatureVectors.push_back(globalIdFullFeatureVectors[*it]);
        findex2Id[c]=*it;
        c++;
      }

/*
      int c=0;
      //TODO:call vishal's submodular function with beta feature vectors and handle the mapping of those beta feature vector indices to gid
      for(int uci=0; uci < uf.size();uci++){
        for(int ui=0;ui<uf[uci].size();ui++){
          int globalId=uf[uci][ui];
          unlabeledTrainingFeatureVectors.push_back(globalIdFullFeatureVectors[globalId]);
          findex2Id[c]=globalId;
          c++;
        }
      }

*/

      //call submodularFunction to get b indices
      std::cout << "submodularFunction to get b indices" << std::endl;
      //need vector<vector<double>> rather than vector<vector<float>>
      std::vector<std::vector<double>> uncertainFeatureVectorsDouble;

      for(int i=0;i<uncertainFeatureVectors.size();i++){
        uncertainFeatureVectorsDouble.push_back(std::vector<double>(uncertainFeatureVectors[i].begin(),uncertainFeatureVectors[i].end()));
      }



      datk::Set indicesToTrain;
      switch(submod) {
          case 0:
              std::cout << "Using FASS+random mode" << std::endl;
              indicesToTrain = getBIndicesRandom(
                  uncertainFeatureVectors.size(),
                  //std::vector<std::vector<double>>(uncertainFeatureVectors.begin(),uncertainFeatureVectors.end()),
                  uncertainFeatureVectorsDouble,
                  maxB
                );
              break;
          case 1:
              std::cout << "Using facility location" << std::endl;
              indicesToTrain = getBIndicesFacilityLocation(
                uncertainFeatureVectors.size(),
                //std::vector<std::vector<double>>(uncertainFeatureVectors.begin(),uncertainFeatureVectors.end()),
                uncertainFeatureVectorsDouble,
                maxB
              );
              break;
          case 2:
              std::cout << "Using Disparity Min" << std::endl;
              indicesToTrain = getBIndicesDisparityMin(
                uncertainFeatureVectors.size(),
                //std::vector<std::vector<double>>(uncertainFeatureVectors.begin(),uncertainFeatureVectors.end()),
                uncertainFeatureVectorsDouble,
                maxB
              );
              break;
          case 3:
              std::cout
                  << "Using Feature Based Functions with sqrt over modular"
                  << std::endl;
              indicesToTrain = getBIndicesFeatureBasedFunctions(
                uncertainFeatureVectors.size(),
                //std::vector<std::vector<double>>(uncertainFeatureVectors.begin(),uncertainFeatureVectors.end()),
                uncertainFeatureVectorsDouble,
                maxB,
                  1);
              break;
          case 4:
              std::cout
                  << "Using Feature Based Functions with inverse function"
                  << std::endl;
              indicesToTrain = getBIndicesFeatureBasedFunctions(
                uncertainFeatureVectors.size(),
                //std::vector<std::vector<double>>(uncertainFeatureVectors.begin(),uncertainFeatureVectors.end()),
                uncertainFeatureVectorsDouble,
                maxB,
                  2);
              break;
          case 5:
              std::cout
                  << "Using Feature Based Functions with Log function"
                  << std::endl;
              indicesToTrain = getBIndicesFeatureBasedFunctions(
                uncertainFeatureVectors.size(),
                //std::vector<std::vector<double>>(uncertainFeatureVectors.begin(),uncertainFeatureVectors.end()),
                uncertainFeatureVectorsDouble,
                maxB,
                  3);
              break;
          case 6: //TODO: handle case 6 and 7 carefully from scratch...Done please verify
              std::cout
                  << "Using Uncertainty Sampling"
                  << std::endl;
                  for(int k=0;k<maxB;k++){
                    indicesToTrain.insert(uv[k].first);
                  }
              break;
          case 7: {
              std::cout << "Using random mode" << std::endl;
              std::vector<int> allIndices;
              for(int uci=0; uci < uf.size();uci++){
                for(int ui=0;ui<uf[uci].size();ui++){
                  int globalId=uf[uci][ui];
                  allIndices.push_back(globalId);
                }
              }
              std::random_shuffle(allIndices.begin(), allIndices.end());
              for(int k=0;k<maxB;k++){
                indicesToTrain.insert(allIndices[k]);
              }

              break;
          }
          default:
              std::cout << "no submodular function found" << std::endl;
              return -1;
      }


      std::cout << "use indicesToTrain and findex2Id and return globalIDs" << std::endl;
      std::vector<int> gloabalIndicesToTrain;
      datk::Set::iterator it;
      for (it = indicesToTrain.begin(); it != indicesToTrain.end(); ++it) {
        if(submod != 6 && submod != 7 ){
          gloabalIndicesToTrain.push_back(findex2Id[*it]);
        }
        else{
          gloabalIndicesToTrain.push_back(*it);
        }
      }



      std::cout << "work on gloabalIndicesToTrain. remove from UF and put it in LF. Also move train images accordingly" << std::endl;
      // here i is class label
      for(int i=0; i < gloabalIndicesToTrain.size();i++){
          int glId = gloabalIndicesToTrain[i];
          int label = globalIdFullLabels[glId];

          std::vector<int>::iterator position = std::find(uf[label].begin(), uf[label].end(), glId);
          uf[label].erase(position);

          lf[label].push_back(glId);

          ufCount--;
          lfCount++;


          std::cout << "manage images in folders accordingly" << std::endl;
          std::cout << "Moving data from " + workDir + "/data/goal2-ft-" + std::to_string(submod) + "-" + std::to_string(int(betaPercent)) + "/Train/"+int2StrLabel[label] + "/" + globalIdFullFileNames[glId]+"  to  "+workDir + "/data/goal2-ft-labeled" + std::to_string(submod) + "-" + std::to_string(int(betaPercent)) + "/Train/"  + int2StrLabel[label]+ "/" << std::endl;
          command = "mv \"" +workDir + "\"/data/goal2-ft-" + std::to_string(submod) + "-" + std::to_string(int(betaPercent)) + "/Train/\""  + int2StrLabel[label] + "\"/\"" + globalIdFullFileNames[glId] + "\" \"" +workDir + "\"/data/goal2-ft-labeled" + std::to_string(submod) + "-" + std::to_string(int(betaPercent)) + "/Train/\""  + int2StrLabel[label]+ "\"/";
          system(command.c_str());
          std::cout << "moving data from unlabeled to labeled training data done" <<std::endl;

      }
      ends = clock();
      elapsed_secs = double(ends - begins) / CLOCKS_PER_SEC;
      std::cout << "TIme taken till the end of this iteration after computing uncertainity vector  = " << std::to_string(elapsed_secs) << " seconds." << std::endl << std::flush;
      return 0;
    }
  }
}
