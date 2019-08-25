#include "../../src/utils/datk/src/datk.h"
#include "../../src/utils/json.hpp"
#include "../../tools/videoSources/fileStreamer.h"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <vector>
#include <chrono>
#include <time.h>

void loadData(std::string filePath, nlohmann::json &jsonData) {
    std::ifstream dataFile;
    dataFile.open(filePath);
    dataFile >> jsonData;
}

datk::Set getSubsetFeatureBasedFunctions(std::vector<std::vector<float>> &featureVectors,
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
    datk::FeatureBasedFunctions featBasedFunc(
        sparseFeatures.size(),
        mode,
        sparseFeatures,
        featureWeights);
    //call lazygreedymax with this Feature Based Functions fuction, B, null output set
    clock_t time;
    time = clock();
    datk::lazyGreedyMax(featBasedFunc, budget, indices);
    std::cout << "~~~~~~~~~~~ FeatureBased " << mode << " Prediction Time: " << (float) (clock() - time)/CLOCKS_PER_SEC << std::endl;

    return indices;
}

datk::Set getBIndicesFeatureBasedFunctions(std::vector<std::vector<float>> &featureVectors,
    double budgetPercent,
    std::set<int> &subsetIndices,
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
    for (std::set<int>::iterator it = subsetIndices.begin();
        it != subsetIndices.end();
        it++) {
        indexMapping[counter++] = *it;
        datk::SparseFeature s;
        s.featureIndex = std::vector<int>();
        s.featureVec = std::vector<double>();
        s.index = *it;
        for (int j = 0; j < featureVectors[*it].size(); j++) {
            if (featureVectors[*it][j] != 0) {
                s.featureIndex.push_back(j);
                s.featureVec.push_back(featureVectors[*it][j]);
            }
        }
        s.numFeatures = featureVectors[*it].size();
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
    double budget = ceil((budgetPercent * (double) sparseFeatures.size()) / (double) 100);
    std::cout << "*********** total size: ";
    std::cout << sparseFeatures.size() << std::endl << std::flush;
    std::cout << "*********** got budget: ";
    std::cout << budget << std::endl << std::flush;

    clock_t time;
    time = clock();
    datk::lazyGreedyMax(featBasedFunc, budget, indices);
    std::cout << "~~~~~~~~~~~ Feature based " << mode << " Prediction Time: " << (float) (clock() - time)/CLOCKS_PER_SEC << std::endl;

    datk::Set scaledIndices;
    for (datk::Set::iterator it = indices.begin();
        it != indices.end();
        it++) {
        scaledIndices.insert(indexMapping[*it]);
    }
    return scaledIndices;
}

datk::Set getBIndicesDisparityMin(std::vector<std::vector<float>> &kernel,
    double budgetPercent,
    std::set<int> &querySubsetIndices) {
    //for each feature vector i (0 to n), compute its dot product with feature vector j (0 to n), update min, update max, store at k(i,j)
    datk::Set indices;
    std::map<int,int> indexMapping;
    int counter = 0;

    std::vector<std::vector<float>> subKernel;
    for (std::set<int>::iterator it = querySubsetIndices.begin();
        it != querySubsetIndices.end();
        it++) {
        indexMapping[counter++] = *it;
        std::vector<float> oldRow = kernel[*it];
        std::vector<float> updatedRow = std::vector<float>();
        for (std::set<int>::iterator it2 = querySubsetIndices.begin();
            it2 != querySubsetIndices.end();
            it2++) {
            updatedRow.push_back(oldRow[*it2]);
        }
        std::cout << "number of rows: " << updatedRow.size() << std::endl;
        subKernel.push_back(updatedRow);
    }

    std::cout << "number of columns: " << subKernel.size() << std::endl;

    //instantiate facility location using kernel
    datk::DisparityMin dispMin(subKernel.size(), subKernel);
    //call lazygreedymax with this facility location fuction, B, null output set
    double budget = ceil((budgetPercent * (double) subKernel.size()) / (double) 100);
    std::cout << "*********** total size: ";
    std::cout << subKernel.size() << std::endl << std::flush;
    std::cout << "*********** got budget: ";
    std::cout << budget << std::endl << std::flush;
    clock_t time;
    time = clock();
    datk::naiveGreedyMax(dispMin, budget, indices, 0, false, true);
    std::cout << "~~~~~~~~~~~ Disparity Min Prediction Time: " << (float) (clock() - time)/CLOCKS_PER_SEC << std::endl;

    datk::Set scaledIndices;
    for (datk::Set::iterator it = indices.begin();
        it != indices.end();
        it++) {
        scaledIndices.insert(indexMapping[*it]);
    }
    return scaledIndices;
}

datk::Set getBIndicesFacilityLocation(std::vector<std::vector<float>> &kernel,
    double budgetPercent,
    std::set<int> &querySubsetIndices) {

    //for each feature vector i (0 to n), compute its dot product with feature vector j (0 to n), update min, update max, store at k(i,j)
    datk::Set indices;
    std::map<int,int> indexMapping;
    int counter = 0;

    std::vector<std::vector<float>> subKernel;
    for (std::set<int>::iterator it = querySubsetIndices.begin();
        it != querySubsetIndices.end();
        it++) {
        indexMapping[counter++] = *it;
        std::vector<float> oldRow = kernel[*it];
        std::vector<float> updatedRow = std::vector<float>();
        for (std::set<int>::iterator it2 = querySubsetIndices.begin();
            it2 != querySubsetIndices.end();
            it2++) {
            updatedRow.push_back(oldRow[*it2]);
        }
        std::cout << "number of rows: " << updatedRow.size() << std::endl;
        subKernel.push_back(updatedRow);
    }

    std::cout << "number of columns: " << subKernel.size() << std::endl;

    //instantiate facility location using kernel
    datk::FacilityLocation facLoc(subKernel.size(), subKernel);
    //call lazygreedymax with this facility location fuction, B, null output set
    double budget = ceil((budgetPercent * (double) subKernel.size()) / (double) 100);
    std::cout << "*********** total size: ";
    std::cout << subKernel.size() << std::endl << std::flush;
    std::cout << "*********** got budget: ";
    std::cout << budget << std::endl << std::flush;
    clock_t time;
    time = clock();
    datk::naiveGreedyMax(facLoc, budget, indices, 0, false, true);
    std::cout << "~~~~~~~~~~~ Facility Location Prediction Time: " << (float) (clock() - time)/CLOCKS_PER_SEC << std::endl;

    datk::Set scaledIndices;
    for (datk::Set::iterator it = indices.begin();
        it != indices.end();
        it++) {
        scaledIndices.insert(indexMapping[*it]);
    }
    return scaledIndices;
}

int main(int argc, char *argv[]) {
    if (argc < 5) {
      std::cout << "usage: ./contentAndQuerySummarization [path/to/video] [path/to/json/folder] [path/to/output/folder/] [budget%]" << std::endl;
      return -1;
    }
    std::string videoFilePath = argv[1];
    std::string jsonFolderPath = argv[2];
    std::string outputFolderPath = argv[3];
    float budgetPercent = std::atof(argv[4]);
    int totalSubmodularFunctions = 6;

    nlohmann::json sceneJson, ffObjectJson, dnObjectJson, faceGenderJson, faceAgeJson, faceVggJson;

    //Loading scene data
    std::cout << "Loading scene data" << std::endl;
    loadData(jsonFolderPath + "/SceneSchema.json", sceneJson);
    std::cout << "Loaded scene data" << std::endl;

    //Loading full frame object data
    std::cout << "Loading ff object data" << std::endl;
    loadData(jsonFolderPath + "/FFObjectSchema.json", ffObjectJson);
    std::cout << "Loaded ff object data" << std::endl;

    //Loading darknet object data
    std::cout << "Loading dn object data" << std::endl;
    loadData(jsonFolderPath + "/DNObjectSchema.json", dnObjectJson);
    std::cout << "Loaded dn object data" << std::endl;

    //Load face gender data
    std::cout << "Loading face gender data" << std::endl;
    loadData(jsonFolderPath + "/FaceGenderSchema.json", faceGenderJson);
    std::cout << "Loaded face gender data" << std::endl;

    //Loading face age data
    std::cout << "Loading face age data" << std::endl;
    loadData(jsonFolderPath + "/FaceAgeSchema.json", faceAgeJson);
    std::cout << "Loaded face age data" << std::endl;

    //Loading face vgg data
    std::cout << "Loading face rec data" << std::endl;
    loadData(jsonFolderPath + "/FaceRecSchema.json", faceVggJson);
    std::cout << "Loaded face rec data" << std::endl;

    std::vector<std::vector<std::vector<float>>> allConceptFeatureVectors;
    std::cout << "Pushing scene features to allConceptFeatureVectors" << std::endl;
    allConceptFeatureVectors.push_back(sceneJson["features"]);
    std::cout << "Pushing ff object features to allConceptFeatureVectors" << std::endl;
    allConceptFeatureVectors.push_back(ffObjectJson["features"]);
    std::cout << "Pushing dn object features to allConceptFeatureVectors" << std::endl;
    allConceptFeatureVectors.push_back(dnObjectJson["features"]);
    std::cout << "Pushing face gender features to allConceptFeatureVectors" << std::endl;
    allConceptFeatureVectors.push_back(faceGenderJson["features"]);
    std::cout << "Pushing face age features to allConceptFeatureVectors" << std::endl;
    allConceptFeatureVectors.push_back(faceAgeJson["features"]);
    std::cout << "Pushing face rec features to allConceptFeatureVectors" << std::endl;
    allConceptFeatureVectors.push_back(faceVggJson["features"]);

    std::vector<std::vector<std::string>> allConceptLabels;
    std::vector<std::set<std::string>> allUniqueConceptLabels;

    std::cout << "Pushing scene labels to allConceptLabels" << std::endl;
    allConceptLabels.push_back(sceneJson["labels"]);
    std::set<std::string> uniqueSceneLabels(
        allConceptLabels[0].begin(),
        allConceptLabels[0].end());
    allUniqueConceptLabels.push_back(uniqueSceneLabels);
    allConceptLabels.push_back(ffObjectJson["labels"]);

    std::cout << "Pushing ff object labels to allConceptLabels" << std::endl;
    std::set<std::string> uniqueFfObjectLabels(
        allConceptLabels[1].begin(),
        allConceptLabels[1].end());
    allUniqueConceptLabels.push_back(uniqueFfObjectLabels);

    std::cout << "Pushing dn object labels to allConceptLabels" << std::endl;
    allConceptLabels.push_back(dnObjectJson["labels"]);
    std::set<std::string> uniqueDnObjectLabels(
        allConceptLabels[2].begin(),
        allConceptLabels[2].end());
    allUniqueConceptLabels.push_back(uniqueDnObjectLabels);

    std::cout << "Pushing face gender labels to allConceptLabels" << std::endl;
    allConceptLabels.push_back(faceGenderJson["labels"]);
    std::set<std::string> uniqueFaceGenderLabels(
        allConceptLabels[3].begin(),
        allConceptLabels[3].end());
    allUniqueConceptLabels.push_back(uniqueFaceGenderLabels);

    std::cout << "Pushing face age labels to allConceptLabels" << std::endl;
    allConceptLabels.push_back(faceAgeJson["labels"]);
    std::set<std::string> uniqueFaceAgeLabels(
        allConceptLabels[4].begin(),
        allConceptLabels[4].end());
    allUniqueConceptLabels.push_back(uniqueFaceAgeLabels);

    std::cout << "Pushing face rec labels to allConceptLabels" << std::endl;
    allConceptLabels.push_back(faceVggJson["labels"]);
    std::set<std::string> uniqueFaceRecLabels(
        allConceptLabels[5].begin(),
        allConceptLabels[5].end());
    allUniqueConceptLabels.push_back(uniqueFaceRecLabels);

    std::vector<std::vector<int>> allConceptFrameNumbers;
    std::cout << "Pushing scene frame numbers to allConceptFrameNumbers" << std::endl;
    allConceptFrameNumbers.push_back(sceneJson["frameNumber"]);
    std::cout << "Pushing ff object frame numbers to allConceptFrameNumbers" << std::endl;
    allConceptFrameNumbers.push_back(ffObjectJson["frameNumber"]);
    std::cout << "Pushing dn object frame numbers to allConceptFrameNumbers" << std::endl;
    allConceptFrameNumbers.push_back(dnObjectJson["frameNumber"]);
    std::cout << "Pushing face gender frame numbers to allConceptFrameNumbers" << std::endl;
    allConceptFrameNumbers.push_back(faceGenderJson["frameNumber"]);
    std::cout << "Pushing face age frame numbers to allConceptFrameNumbers" << std::endl;
    allConceptFrameNumbers.push_back(faceAgeJson["frameNumber"]);
    std::cout << "Pushing face rec frame numbers to allConceptFrameNumbers" << std::endl;
    allConceptFrameNumbers.push_back(faceVggJson["frameNumber"]);

    std::vector<std::vector<std::vector<float>>> allConceptKernels;

    // Load scene kernel
    allConceptKernels.push_back(std::vector<std::vector<float>>());
    std::cout << "Loading scene kernel and pushing into allConceptKernels" << std::endl;
    for (int i = 0; i < sceneJson["kernel"].size(); i++) {
        allConceptKernels[0].push_back(sceneJson["kernel"][i]);
    }

    // Load Full Frame object kernel
    allConceptKernels.push_back(std::vector<std::vector<float>>());
    std::cout << "Loading full frame object and pushing into allConceptKernels" << std::endl;
    for (int i = 0; i < ffObjectJson["kernel"].size(); i++) {
        allConceptKernels[1].push_back(ffObjectJson["kernel"][i]);
    }

    // Load Darknet object kernel
    allConceptKernels.push_back(std::vector<std::vector<float>>());
    std::cout << "Loading darknet object and pushing into allConceptKernels" << std::endl;
    for (int i = 0; i < dnObjectJson["kernel"].size(); i++) {
        allConceptKernels[2].push_back(dnObjectJson["kernel"][i]);
    }

    // Load Face Gender kernel
    allConceptKernels.push_back(std::vector<std::vector<float>>());
    std::cout << "Loading Face Gender and pushing into allConceptKernels" << std::endl;
    for (int i = 0; i < faceGenderJson["kernel"].size(); i++) {
        allConceptKernels[3].push_back(faceGenderJson["kernel"][i]);
    }

    // Load Face Age kernel
    allConceptKernels.push_back(std::vector<std::vector<float>>());
    std::cout << "Loading Face Age and pushing into allConceptKernels" << std::endl;
    for (int i = 0; i < faceAgeJson["kernel"].size(); i++) {
        allConceptKernels[4].push_back(faceAgeJson["kernel"][i]);
    }

    // Load Face Vgg kernel
    allConceptKernels.push_back(std::vector<std::vector<float>>());
    std::cout << "Loading Face Vgg and pushing into allConceptKernels" << std::endl;
    for (int i = 0; i < faceVggJson["kernel"].size(); i++) {
        allConceptKernels[5].push_back(faceVggJson["kernel"][i]);
    }

    std::vector<std::vector<cv::Rect>> allConceptRects;
    std::vector<cv::Rect> dnObjectRectangles;
    std::cout << "Pushing dn object rectangles to allConceptRects" << std::endl;
    for (int i = 0; i < dnObjectJson["rectangle"].size(); i++) {
        cv::Rect cutOut = cv::Rect(dnObjectJson["rectangle"][i][0],
            dnObjectJson["rectangle"][i][1],
            dnObjectJson["rectangle"][i][2],
            dnObjectJson["rectangle"][i][3]);
        std::cout << "Area: " << cutOut.area() << std::endl;
        dnObjectRectangles.push_back(cutOut);
    }
    allConceptRects.push_back(dnObjectRectangles);

    std::vector<cv::Rect> faceGenderRectangles;
    std::cout << "Pushing face gender rectangles to allConceptRects" << std::endl;
    for (int i = 0; i < faceGenderJson["rectangle"].size(); i++) {
        faceGenderRectangles.push_back(cv::Rect(faceGenderJson["rectangle"][i][0],
            faceGenderJson["rectangle"][i][1],
            faceGenderJson["rectangle"][i][2],
            faceGenderJson["rectangle"][i][3]));
    }
    allConceptRects.push_back(faceGenderRectangles);

    std::vector<cv::Rect> faceAgeRectangles;
    std::cout << "Pushing face age rectangles to allConceptRects" << std::endl;
    for (int i = 0; i < faceAgeJson["rectangle"].size(); i++) {
        faceAgeRectangles.push_back(cv::Rect(faceAgeJson["rectangle"][i][0],
            faceAgeJson["rectangle"][i][1],
            faceAgeJson["rectangle"][i][2],
            faceAgeJson["rectangle"][i][3]));
    }
    allConceptRects.push_back(faceAgeRectangles);

    std::vector<cv::Rect> faceRecRectangles;
    std::cout << "Pushing face rec rectangles to allConceptRects" << std::endl;
    for (int i = 0; i < faceVggJson["rectangle"].size(); i++) {
        faceRecRectangles.push_back(cv::Rect(faceVggJson["rectangle"][i][0],
            faceVggJson["rectangle"][i][1],
            faceVggJson["rectangle"][i][2],
            faceVggJson["rectangle"][i][3]));
    }
    allConceptRects.push_back(faceRecRectangles);

    std::string command = "rm -rf " + outputFolderPath;
    system(command.c_str());
    command = "mkdir -p \"" + outputFolderPath + "/ConceptualSummary\"";
    system(command.c_str());
    command = "mkdir -p \"" + outputFolderPath + "/QueryBasedSummary\"";
    system(command.c_str());
    FileStreamer *fileStreamer = new FileStreamer(videoFilePath);

    std::cout << "going through all 6 concepts: " << std::endl;
    for (int conceptIndex = 0;
        conceptIndex < allConceptFeatureVectors.size();
        conceptIndex++) {
        std::string conceptPath = outputFolderPath + "/ConceptualSummary/";
        std::string queryRootPath = outputFolderPath + "/QueryBasedSummary/";

        switch (conceptIndex) {
            case 0:
                std::cout << "Scene" << std::endl;
                conceptPath = conceptPath + "Scene";
                queryRootPath = queryRootPath + "Scene";
                break;
            case 1:
                std::cout << "FF Object" << std::endl;
                conceptPath = conceptPath + "FullFrameObject";
                queryRootPath = queryRootPath + "FullFrameObject";
                break;
            case 2:
                std::cout << "DN Object" << std::endl;
                conceptPath = conceptPath + "DarknetObject";
                queryRootPath = queryRootPath + "DarknetObject";
                break;
            case 3:
                std::cout << "Face Gender Object" << std::endl;
                conceptPath = conceptPath + "FaceGender";
                queryRootPath = queryRootPath + "FaceGender";
                break;
            case 4:
                std::cout << "Face Age Object" << std::endl;
                conceptPath = conceptPath + "FaceAge";
                queryRootPath = queryRootPath + "FaceAge";
                break;
            case 5:
                std::cout << "Face Rec Object" << std::endl;
                conceptPath = conceptPath + "FaceVgg";
                queryRootPath = queryRootPath + "FaceVgg";
                break;
        }
        command = "mkdir -p \"" + conceptPath + "\"";
        system(command.c_str());

        command = "mkdir -p \"" + queryRootPath + "\"";
        system(command.c_str());

        std::cout << "starting submodular functions loop" << std::endl;
        for (int submodularFunction = 0;
            submodularFunction < totalSubmodularFunctions;
            submodularFunction++) {
            std::string conceptSubmodPath = conceptPath;
            std::string querySubmodPath = queryRootPath;
            datk::Set conceptSubsetIndices;
            std::vector<std::string> queryLabels;
            std::vector<datk::Set> queryIndices;
            switch (submodularFunction) {
                case 0:
                {
                    std::cout << "Using ground set: " << std::endl;
                    conceptSubmodPath += "/GroundSet/";
                    for (int i = 0; i < allConceptFeatureVectors[conceptIndex].size(); i++) {
                        conceptSubsetIndices.insert(i);
                    }
                    for (std::set<std::string>::iterator it = allUniqueConceptLabels[conceptIndex].begin();
                        it != allUniqueConceptLabels[conceptIndex].end();
                        ++it) {
                        datk::Set querySubsetIndices;
                        for (int labelIndex = 0;
                            labelIndex < allConceptLabels[conceptIndex].size();
                            labelIndex++) {
                                if (!allConceptLabels[conceptIndex][labelIndex].compare(*it)) {
                                    querySubsetIndices.insert(labelIndex);
                                }
                        }
                        queryLabels.push_back(*it);
                        queryIndices.push_back(querySubsetIndices);
                    }
                    break;

                }
                case 1:
                {
                    std::cout << "Using facility location" << std::endl;
                    conceptSubmodPath += "/FacilityLocation/";
                    // querySubmodPath += "/FacilityLocation/";
                    //instantiate facility location using kernel
                    datk::FacilityLocation facLoc(allConceptKernels[conceptIndex].size(),
                        allConceptKernels[conceptIndex]);
                    //call lazygreedymax with this facility location fuction, B, null output set
                    clock_t time;
                    time = clock();
                    datk::lazyGreedyMax(facLoc,
                        ceil((budgetPercent * (double) allConceptKernels[conceptIndex].size()) / (double )100),
                        conceptSubsetIndices);
                    std::cout << "~~~~~~~~~~~ Facility Location Prediction Time: " << (float) (clock() - time)/CLOCKS_PER_SEC << std::endl;

                    std::cout << "running for labels: " << allUniqueConceptLabels[conceptIndex].size() << std::endl;

                    for (std::set<std::string>::iterator it = allUniqueConceptLabels[conceptIndex].begin();
                        it != allUniqueConceptLabels[conceptIndex].end();
                        ++it) {
                            std::cout << "running for a unique label" << std::endl;
                        std::set<int> querySubsetIndices;
                        for (int labelIndex = 0;
                            labelIndex < allConceptLabels[conceptIndex].size();
                            labelIndex++) {
                                if (!allConceptLabels[conceptIndex][labelIndex].compare(*it)) {
                                    querySubsetIndices.insert(labelIndex);
                                }
                        }
                        queryLabels.push_back(*it);
                        queryIndices.push_back(
                            getBIndicesFacilityLocation(
                                allConceptKernels[conceptIndex],
                                budgetPercent,
                                querySubsetIndices));
                    }
                    break;
                }
                case 2:
                {
                    std::cout << "Using Disparity Min" << std::endl;
                    conceptSubmodPath += "/DisparityMin/";
                    // querySubmodPath  = querySubmodPath + "/DisparityMin/";
                    //instantiate facility location using kernel
                    datk::DisparityMin dispMin(allConceptKernels[conceptIndex].size(),
                        allConceptKernels[conceptIndex]);
                    //call lazygreedymax with this facility location fuction, B, null output set
                    clock_t time;
                    time = clock();
                    datk::naiveGreedyMax(dispMin,
                        ceil((budgetPercent * (double) allConceptKernels[conceptIndex].size()) / (double) 100),
                        conceptSubsetIndices,
                        0,
                        false,
                        true);
                    std::cout << "~~~~~~~~~~~ Disparity Min Prediction Time: " << (float) (clock() - time)/CLOCKS_PER_SEC << std::endl;


                        std::cout << "running for labels: " << allUniqueConceptLabels[conceptIndex].size() << std::endl;

                    for (std::set<std::string>::iterator it = allUniqueConceptLabels[conceptIndex].begin();
                        it != allUniqueConceptLabels[conceptIndex].end();
                        ++it) {
                            std::cout << "running for a unique label" << std::endl;
                        std::set<int> querySubsetIndices;
                        for (int labelIndex = 0;
                            labelIndex < allConceptLabels[conceptIndex].size();
                            labelIndex++) {
                                if (!allConceptLabels[conceptIndex][labelIndex].compare(*it)) {
                                    querySubsetIndices.insert(labelIndex);
                                }
                        }
                        queryLabels.push_back(*it);
                        queryIndices.push_back(
                            getBIndicesDisparityMin(
                                allConceptKernels[conceptIndex],
                                budgetPercent,
                                querySubsetIndices));
                    }
                    break;
                }
                case 3:
                {
                    std::cout
                        << "Using Feature Based Functions with sqrt over modular"
                        << std::endl;
                    conceptSubmodPath += "/FeatureBasedFunctionWithSqrtOverModular/";
                    // querySubmodPath += "/FeatureBasedFunctionWithSqrtOverModular/";
                    conceptSubsetIndices = getSubsetFeatureBasedFunctions(
                        allConceptFeatureVectors[conceptIndex],
                        ceil((budgetPercent * (double) allConceptKernels[conceptIndex].size()) / (double) 100),
                        1);
                        std::cout << "running for labels: " << allUniqueConceptLabels[conceptIndex].size() << std::endl;
                    for (std::set<std::string>::iterator it = allUniqueConceptLabels[conceptIndex].begin();
                        it != allUniqueConceptLabels[conceptIndex].end();
                        ++it) {
                            std::cout << "running for a unique label" << std::endl;
                        std::set<int> querySubsetIndices;
                        for (int labelIndex = 0;
                            labelIndex < allConceptLabels[conceptIndex].size();
                            labelIndex++) {
                                if (!allConceptLabels[conceptIndex][labelIndex].compare(*it)) {
                                    querySubsetIndices.insert(labelIndex);
                                }
                        }
                        queryLabels.push_back(*it);
                        queryIndices.push_back(
                            getBIndicesFeatureBasedFunctions(
                                allConceptFeatureVectors[conceptIndex],
                                budgetPercent,
                                querySubsetIndices,
                                1));
                    }

                    break;
                }
                case 4:
                {
                    std::cout
                        << "Using Feature Based Functions with inverse function"
                        << std::endl;
                    conceptSubmodPath += "/FeatureBasedFunctionWithInverseFunction/";
                    // querySubmodPath += "/FeatureBasedFunctionWithInverseFunction/";
                    conceptSubsetIndices = getSubsetFeatureBasedFunctions(
                        allConceptFeatureVectors[conceptIndex],
                        ceil((budgetPercent * (double) allConceptKernels[conceptIndex].size()) / (double) 100),
                        2);
                        std::cout << "running for labels: " << allUniqueConceptLabels[conceptIndex].size() << std::endl;
                    for (std::set<std::string>::iterator it = allUniqueConceptLabels[conceptIndex].begin();
                        it != allUniqueConceptLabels[conceptIndex].end();
                        ++it) {
                            std::cout << "running for a unique label" << std::endl;
                        std::set<int> querySubsetIndices;
                        for (int labelIndex = 0;
                            labelIndex < allConceptLabels[conceptIndex].size();
                            labelIndex++) {
                                if (!allConceptLabels[conceptIndex][labelIndex].compare(*it)) {
                                    querySubsetIndices.insert(labelIndex);
                                }
                        }
                        queryLabels.push_back(*it);
                        queryIndices.push_back(
                            getBIndicesFeatureBasedFunctions(
                                allConceptFeatureVectors[conceptIndex],
                                budgetPercent,
                                querySubsetIndices,
                                2));
                    }
                    break;
                }
                case 5:
                {
                    std::cout
                        << "Using Feature Based Functions with Log function"
                        << std::endl;
                    conceptSubmodPath += "/FeatureBasedFunctionWithLogFunction/";
                    // querySubmodPath  = querySubmodPath + "/FeatureBasedFunctionWithLogFunction/";
                    conceptSubsetIndices = getSubsetFeatureBasedFunctions(
                        allConceptFeatureVectors[conceptIndex],
                        ceil((budgetPercent * (double) allConceptKernels[conceptIndex].size()) / (double) 100),
                        3);
                        std::cout << "running for labels: " << allUniqueConceptLabels[conceptIndex].size() << std::endl;
                    for (std::set<std::string>::iterator it = allUniqueConceptLabels[conceptIndex].begin();
                        it != allUniqueConceptLabels[conceptIndex].end();
                        ++it) {
                            std::cout << "running for a unique label" << std::endl;
                        std::set<int> querySubsetIndices;
                        for (int labelIndex = 0;
                            labelIndex < allConceptLabels[conceptIndex].size();
                            labelIndex++) {
                                if (!allConceptLabels[conceptIndex][labelIndex].compare(*it)) {
                                    querySubsetIndices.insert(labelIndex);
                                }
                        }
                        queryLabels.push_back(*it);
                        queryIndices.push_back(
                            getBIndicesFeatureBasedFunctions(
                                allConceptFeatureVectors[conceptIndex],
                                budgetPercent,
                                querySubsetIndices,
                                3));
                    }
                    break;
                }
                default:
                    std::cout << "Invalid mode (submodular function), quitting" << std::endl;
                    return -1;
            }
            command = "mkdir -p \"" + conceptSubmodPath + "\"";
            system(command.c_str());

            command = "mkdir -p \"" + querySubmodPath + "\"";
            system(command.c_str());
            std::cout << "" << std::endl;
            for (datk::Set::iterator it = conceptSubsetIndices.begin();
                it != conceptSubsetIndices.end();
                it++) {
                std::cout << "conceptSubsetIndices: " << *it << std::endl;
                int videoFrameNumber = allConceptFrameNumbers[conceptIndex][*it];
                fileStreamer->cap->set(CV_CAP_PROP_POS_FRAMES, videoFrameNumber);
                cv::Mat interestFrame;
                fileStreamer->cap->read(interestFrame);
                int imageInLabelCount = std::distance(conceptSubsetIndices.begin(), it);
                std::string fileName = conceptSubmodPath + "/";
                fileName = fileName
                    + std::to_string(videoFrameNumber)
                    + "-"
                    + allConceptLabels[conceptIndex][*it]
                    + "-"
                    + std::to_string(imageInLabelCount)
                    + ".jpeg";
                // TODO: Save one image with localisation (not full frame)
                if (interestFrame.data != NULL) {
                    if (conceptIndex == 0 || conceptIndex == 1) {
                        cv::imshow("Saving this: ", interestFrame);
                        cv::waitKey(5);
                        cv::imwrite(fileName, interestFrame);
                    } else {
                        std::cout << "Saving cut out image"  << std::endl;
                        std::cout << "size of rect: " << allConceptRects[conceptIndex - 2][*it].area() << std::endl;
                        cv::imshow("Saving this: ", interestFrame(allConceptRects[conceptIndex - 2][*it]));
                        cv::waitKey(5);
                        cv::imwrite(
                            fileName,
                            interestFrame(allConceptRects[conceptIndex - 2][*it])
                        );
                    }
                }
            }

            for (std::vector<datk::Set>::iterator it = queryIndices.begin();
                it != queryIndices.end();
                it++) {
                int index = it - queryIndices.begin();
                std::cout << "queryLabels[i]:" << queryLabels[index] << std::endl;
                std::cout << "Should be equal to: " << std::endl;

                datk::Set queryIndex = *it;

                for (datk::Set::iterator it2 = queryIndex.begin();
                    it2 != queryIndex.end();
                    it2++) {
                    int videoFrameNumber = allConceptFrameNumbers[conceptIndex][*it2];
                    fileStreamer->cap->set(CV_CAP_PROP_POS_FRAMES, videoFrameNumber);
                    cv::Mat interestFrame;
                    fileStreamer->cap->read(interestFrame);
                    // TODO: Save one image with localisation (not full frame)
                    if (interestFrame.data != NULL) {
                        std::string fileName = querySubmodPath + "/" + queryLabels[index] + "/";
                        switch(submodularFunction) {
                            case 0:
                                fileName = fileName + "/GroundSet/";
                                break;
                            case 1:
                                fileName = fileName + "/FacilityLocation/";
                                break;
                            case 2:
                                fileName = fileName + "/DisparityMin/";
                                break;
                            case 3:
                                fileName = fileName +
                                    "/FeatureBasedFunctionWithSqrtOverModular/";
                                break;
                            case 4:
                                fileName = fileName +
                                    "/FeatureBasedFunctionWithInverseFunction/";
                                break;
                            case 5:
                                fileName = fileName + "/FeatureBasedFunctionWithLogFunction/";
                                break;
                        }
                        command = "mkdir -p \"" + querySubmodPath + "/" + queryLabels[index] + "\"";
                        system(command.c_str());
                        command = "mkdir -p \"" + fileName + "\"";
                        // std::cout << "******fileName2: " << fileName << std::endl;
                        system(command.c_str());
                        int imageInLabelCount = std::distance(queryIndex.begin(), it2);
                        fileName = fileName
                            + std::to_string(videoFrameNumber)
                            + "-"
                            + std::to_string(imageInLabelCount)
                            + ".jpeg";

                        // std::cout << "*****fileName: " << fileName << std::endl;

                        std::cout << "\t" << allConceptLabels[conceptIndex][*it2] << std::endl;
                        if (conceptIndex == 0 || conceptIndex == 1) {
                            cv::imshow("Saving this: ", interestFrame);
                            cv::waitKey(5);

                            cv::imwrite(fileName, interestFrame);
                            continue;
                        } else {
                            std::cout << "\tCorresponding rectangle :" << std::endl;
                            std::cout
                                << "\t\tx" << allConceptRects[conceptIndex-2][*it2].x << std::endl;
                            std::cout
                                << "\t\ty" << allConceptRects[conceptIndex-2][*it2].y << std::endl;
                            std::cout
                                << "\t\twidth" << allConceptRects[conceptIndex-2][*it2].width << std::endl;
                            std::cout
                                << "\t\theight" << allConceptRects[conceptIndex-2][*it2].height << std::endl;
                            std::cout << "Saving cut out image"  << std::endl;
                            std::cout << "size of rect: " << allConceptRects[conceptIndex - 2][*it2].area() << std::endl;
                            cv::imshow("Saving this: ", interestFrame(allConceptRects[conceptIndex - 2][*it2]));
                            std::cout << "Saving " << fileName << std::endl;
                            cv::waitKey(5);
                            cv::imwrite(fileName,
                                interestFrame(allConceptRects[conceptIndex - 2][*it2])
                            );

                        }
                    }
                }
            }
        }
    }
}
