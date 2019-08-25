#include "../../src/utils/datk/src/datk.h"
#include "../../src/utils/json.hpp"
#include "../../tools/videoSources/fileStreamer.h"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <vector>
#include <chrono>
#include <time.h>

std::string videoFilePath;
std::string jsonFolderPath;
std::string outputFolderPath;
float budgetPercent;
FileStreamer *fileStreamer;


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

void saveData(
    std::string concept,
    std::string submodularFunction,
    std::vector<std::string> &labels,
    std::vector<int> &frameNumbers,
    std::vector<cv::Rect> &rectangles,
    datk::Set &conceptIndicesToSave,
    std::vector<std::string> &queryLabelsToSave,
    std::vector<datk::Set> &queryIndicesToSave) {

    std::string conceptSubmodPath = outputFolderPath
        + "/ConceptualSummary/"
        + concept
        + "/"
        + submodularFunction
        + "/";
    std::string command = "mkdir -p \"" + conceptSubmodPath + "\"";
    system(command.c_str());

    for (datk::Set::iterator it = conceptIndicesToSave.begin();
        it != conceptIndicesToSave.end();
        it++) {
        std::cout << "conceptIndicesToSave: " << *it << std::endl;
        int videoFrameNumber = frameNumbers[*it];
        fileStreamer->cap->set(CV_CAP_PROP_POS_FRAMES, videoFrameNumber);
        cv::Mat interestFrame;
        fileStreamer->cap->read(interestFrame);
        int imageInLabelCount = std::distance(conceptIndicesToSave.begin(), it);
        std::string fileName = conceptSubmodPath
            + "/"
            + std::to_string(videoFrameNumber)
            + "-"
            + labels[*it]
            + "-"
            + std::to_string(imageInLabelCount)
            + ".jpeg";
        if (interestFrame.data != NULL) {
            if (rectangles.empty()) {
                cv::imshow("Saving this: ", interestFrame);
                cv::waitKey(5);
                cv::imwrite(fileName, interestFrame);
            } else {
                std::cout << "Saving cut out image"  << std::endl;
                std::cout << "size of rect: " << rectangles[*it].area() << std::endl;
                cv::imshow("Saving this: ", interestFrame(rectangles[*it]));
                cv::waitKey(5);
                cv::imwrite(
                    fileName,
                    interestFrame(rectangles[*it])
                );
            }
        }
    }

    std::string querySubmodPath = outputFolderPath
        + "/QueryBasedSummary/"
        + concept;

    command = "mkdir -p \"" + querySubmodPath + "\"";
    system(command.c_str());

    for (std::vector<datk::Set>::iterator it = queryIndicesToSave.begin();
        it != queryIndicesToSave.end();
        it++) {
        int index = it - queryIndicesToSave.begin();
        std::cout << "queryLabelsToSave[i]:" << queryLabelsToSave[index] << std::endl;
        std::cout << "Should be equal to: " << std::endl;

        std::string labelFolder = querySubmodPath
            + "/"
            + queryLabelsToSave[index]
            + "/"
            + submodularFunction
            + "/";

        command = "mkdir -p \"" + labelFolder + "\"";
        system(command.c_str());

        datk::Set labelIndices = *it;

        for (datk::Set::iterator it2 = labelIndices.begin();
            it2 != labelIndices.end();
            it2++) {
            int videoFrameNumber = frameNumbers[*it2];
            fileStreamer->cap->set(CV_CAP_PROP_POS_FRAMES, videoFrameNumber);
            cv::Mat interestFrame;
            fileStreamer->cap->read(interestFrame);
            if (interestFrame.data != NULL) {

                int imageInLabelCount = std::distance(labelIndices.begin(), it2);
                std::string fileName =
                    labelFolder
                    + std::to_string(videoFrameNumber)
                    + "-"
                    + std::to_string(imageInLabelCount)
                    + ".jpeg";


                std::cout << "\t" << labels[*it2] << std::endl;
                if (rectangles.empty()) {
                    cv::imshow("Saving this: ", interestFrame);
                    cv::waitKey(5);
                    cv::imwrite(fileName, interestFrame);
                    continue;
                } else {
                    std::cout << "\tCorresponding rectangle :" << std::endl;
                    std::cout
                        << "\t\tx" << rectangles[*it2].x << std::endl;
                    std::cout
                        << "\t\ty" << rectangles[*it2].y << std::endl;
                    std::cout
                        << "\t\twidth" << rectangles[*it2].width << std::endl;
                    std::cout
                        << "\t\theight" << rectangles[*it2].height << std::endl;
                    std::cout << "Saving cut out image"  << std::endl;
                    std::cout << "size of rect: " << rectangles[*it2].area() << std::endl;
                    cv::imshow("Saving this: ", interestFrame(rectangles[*it2]));
                    std::cout << "Saving " << fileName << std::endl;
                    cv::waitKey(5);
                    cv::imwrite(fileName,
                        interestFrame(rectangles[*it2])
                    );
                }
            }
        }
    }
}

void runGroundSet(
    std::string concept,
    std::vector<std::vector<float>> &featureVectors,
    std::vector<std::string> &labels,
    std::set<std::string> &uniqueLabels,
    std::vector<int> &frameNumbers,
    std::vector<std::vector<float>> &kernel,
    std::vector<cv::Rect> &rectangles) {

    std::cout << "Using Ground set" << std::endl << std::flush;
    datk::Set conceptIndicesToSave;
    for (int i = 0; i < featureVectors.size(); i++) {
        conceptIndicesToSave.insert(i);
    }

    std::cout << "running for labels: " << labels.size() << std::endl;
    std::vector<std::string> queryLabelsToSave;
    std::vector<datk::Set> queryIndicesToSave;

    for (std::set<std::string>::iterator it = uniqueLabels.begin();
        it != uniqueLabels.end();
        ++it) {
        std::cout << "running for a unique label " << *it << std::endl;
        datk::Set queryLabelIndices;
        for (int labelIndex = 0;
            labelIndex < labels.size();
            labelIndex++) {
                if (!labels[labelIndex].compare(*it)) {
                    queryLabelIndices.insert(labelIndex);
                }
        }
        queryLabelsToSave.push_back(*it);
        queryIndicesToSave.push_back(queryLabelIndices);
    }

    saveData(
        concept,
        "GroundSet",
        labels,
        frameNumbers,
        rectangles,
        conceptIndicesToSave,
        queryLabelsToSave,
        queryIndicesToSave
    );
}

void runFacilityLocation(
    std::string concept,
    std::vector<std::vector<float>> &featureVectors,
    std::vector<std::string> &labels,
    std::set<std::string> &uniqueLabels,
    std::vector<int> &frameNumbers,
    std::vector<std::vector<float>> &kernel,
    std::vector<cv::Rect> &rectangles) {

    std::cout << "Using facility location" << std::endl << std::flush;

    datk::Set conceptIndicesToSave;
    datk::FacilityLocation facLoc(
        kernel.size(),
        kernel);
    //call lazygreedymax with this facility location fuction, B, null output set
    clock_t time;
    time = clock();
    datk::lazyGreedyMax(
        facLoc,
        ceil((budgetPercent * (double) kernel.size()) / (double )100),
        conceptIndicesToSave);
    std::cout << "~~~~~~~~~~~ Facility Location Prediction Time: " << (float) (clock() - time)/CLOCKS_PER_SEC << std::endl;

    std::cout << "running for labels: " << labels.size() << std::endl;
    std::vector<std::string> queryLabelsToSave;
    std::vector<datk::Set> queryIndicesToSave;

    for (std::set<std::string>::iterator it = uniqueLabels.begin();
        it != uniqueLabels.end();
        ++it) {
        std::cout << "running for a unique label " << *it << std::endl;
        std::set<int> queryLabelIndices;
        for (int labelIndex = 0;
            labelIndex < labels.size();
            labelIndex++) {
                if (!labels[labelIndex].compare(*it)) {
                    queryLabelIndices.insert(labelIndex);
                }
        }
        queryLabelsToSave.push_back(*it);
        queryIndicesToSave.push_back(
            getBIndicesFacilityLocation(
                kernel,
                budgetPercent,
                queryLabelIndices));
    }

    saveData(
        concept,
        "FacilityLocation",
        labels,
        frameNumbers,
        rectangles,
        conceptIndicesToSave,
        queryLabelsToSave,
        queryIndicesToSave
    );
}

void runDisparityMin(
    std::string concept,
    std::vector<std::vector<float>> &featureVectors,
    std::vector<std::string> &labels,
    std::set<std::string> &uniqueLabels,
    std::vector<int> &frameNumbers,
    std::vector<std::vector<float>> &kernel,
    std::vector<cv::Rect> &rectangles) {

    std::cout << "Using Disparity Min" << std::endl << std::flush;

    datk::Set conceptIndicesToSave;
    datk::DisparityMin dispMin(
        kernel.size(),
        kernel);
    //call lazygreedymax with this facility location fuction, B, null output set
    clock_t time;
    time = clock();
    datk::naiveGreedyMax(dispMin,
        ceil((budgetPercent * (double) kernel.size()) / (double) 100),
        conceptIndicesToSave,
        0,
        false,
        true);
    std::cout << "~~~~~~~~~~~ Disparity Min Prediction Time: " << (float) (clock() - time)/CLOCKS_PER_SEC << std::endl;

    std::cout << "running for labels: " << labels.size() << std::endl;
    std::vector<std::string> queryLabelsToSave;
    std::vector<datk::Set> queryIndicesToSave;

    for (std::set<std::string>::iterator it = uniqueLabels.begin();
        it != uniqueLabels.end();
        ++it) {
        std::cout << "running for a unique label " << *it << std::endl;
        std::set<int> queryLabelIndices;
        for (int labelIndex = 0;
            labelIndex < labels.size();
            labelIndex++) {
                if (!labels[labelIndex].compare(*it)) {
                    queryLabelIndices.insert(labelIndex);
                }
        }
        queryLabelsToSave.push_back(*it);
        queryIndicesToSave.push_back(
            getBIndicesDisparityMin(
                kernel,
                budgetPercent,
                queryLabelIndices));
    }

    saveData(
        concept,
        "DisparityMin",
        labels,
        frameNumbers,
        rectangles,
        conceptIndicesToSave,
        queryLabelsToSave,
        queryIndicesToSave
    );
}

void runFeatureBasedFunctionsLocation(
    std::string concept,
    std::vector<std::vector<float>> &featureVectors,
    std::vector<std::string> &labels,
    std::set<std::string> &uniqueLabels,
    std::vector<int> &frameNumbers,
    std::vector<std::vector<float>> &kernel,
    std::vector<cv::Rect> &rectangles,
    int mode) {

    std::cout
        << "Using Feature Based Functions with mode"
        << mode
        << std::endl << std::flush;

    clock_t time;
    time = clock();
    datk::Set conceptIndicesToSave =
        getSubsetFeatureBasedFunctions(
            featureVectors,
            ceil((budgetPercent * (double) kernel.size()) / (double) 100),
            mode);
    std::cout << "~~~~~~~~~~~ Feature Based Functions Prediction Time: " << (float) (clock() - time)/CLOCKS_PER_SEC << std::endl;

    std::cout << "running for labels: " << labels.size() << std::endl;
    std::vector<std::string> queryLabelsToSave;
    std::vector<datk::Set> queryIndicesToSave;

    for (std::set<std::string>::iterator it = uniqueLabels.begin();
        it != uniqueLabels.end();
        ++it) {
        std::cout << "running for a unique label " << *it << std::endl;
        std::set<int> queryLabelIndices;
        for (int labelIndex = 0;
            labelIndex < labels.size();
            labelIndex++) {
                if (!labels[labelIndex].compare(*it)) {
                    queryLabelIndices.insert(labelIndex);
                }
        }
        queryLabelsToSave.push_back(*it);
        queryIndicesToSave.push_back(
            getBIndicesFeatureBasedFunctions(
                kernel,
                budgetPercent,
                queryLabelIndices,
                mode));
    }
    std::string featureName = "FeatureBasedFunctions";
    if (mode == 1) {
        featureName = featureName + "WithSqrtOverModular";
    }
    else if (mode == 2) {
        featureName = featureName + "WithInverseFunction";
    }
    else {
        featureName = featureName + "WithLogFunction";
    }

    saveData(
        concept,
        featureName,
        labels,
        frameNumbers,
        rectangles,
        conceptIndicesToSave,
        queryLabelsToSave,
        queryIndicesToSave
    );
}

void runConceptSummarization(std::string concept) {

    nlohmann::json jsonObject;

    //Loading json data
    std::cout << "Loading " << concept << " data" << std::endl;
    loadData(jsonFolderPath + "/" + concept + "Schema.json", jsonObject);
    std::cout << "Loaded " << concept << " data" << std::endl;

    std::cout << "Pushing " << concept << " features to featureVectors" << std::endl;
    std::vector<std::vector<float>> featureVectors = jsonObject["features"];

    std::cout << "Pushing " << concept << " labels to labels" << std::endl;
    std::vector<std::string> labels = jsonObject["labels"];
    std::set<std::string> uniqueLabels(
        labels.begin(),
        labels.end());

    std::cout << "Pushing " << concept << " frame numbers to frameNumbers" << std::endl;
    std::vector<int> frameNumbers = jsonObject["frameNumber"];

    std::cout << "Pushing " << concept << " kernel to kernel" << std::endl;
    std::vector<std::vector<float>> kernel = jsonObject["kernel"];

    std::cout << "Pushing " << concept << " rectangles to rectangles" << std::endl;
    std::vector<cv::Rect> rectangles = std::vector<cv::Rect>();
    if (jsonObject["rectangle"] != NULL) {
        for (int i = 0; i < jsonObject["rectangle"].size(); i++) {
            cv::Rect cutOut = cv::Rect(
                jsonObject["rectangle"][i][0],
                jsonObject["rectangle"][i][1],
                jsonObject["rectangle"][i][2],
                jsonObject["rectangle"][i][3]
            );
            std::cout << "Area: " << cutOut.area() << std::endl;
            rectangles.push_back(cutOut);
        }
    }

    runGroundSet(
        concept,
        featureVectors,
        labels,
        uniqueLabels,
        frameNumbers,
        kernel,
        rectangles
    );

    runFacilityLocation(
        concept,
        featureVectors,
        labels,
        uniqueLabels,
        frameNumbers,
        kernel,
        rectangles
    );

    runDisparityMin(
        concept,
        featureVectors,
        labels,
        uniqueLabels,
        frameNumbers,
        kernel,
        rectangles
    );

    runFeatureBasedFunctionsLocation(
        concept,
        featureVectors,
        labels,
        uniqueLabels,
        frameNumbers,
        kernel,
        rectangles,
        1
    );

    runFeatureBasedFunctionsLocation(
        concept,
        featureVectors,
        labels,
        uniqueLabels,
        frameNumbers,
        kernel,
        rectangles,
        2
    );

    runFeatureBasedFunctionsLocation(
        concept,
        featureVectors,
        labels,
        uniqueLabels,
        frameNumbers,
        kernel,
        rectangles,
        3
    );
}

int main(int argc, char *argv[]) {
    if (argc < 5) {
      std::cout << "usage: ./contentAndQuerySummarization [path/to/video] [path/to/json/folder] [path/to/output/folder/] [budget%]" << std::endl;
      return -1;
    }
    videoFilePath = argv[1];
    jsonFolderPath = argv[2];
    outputFolderPath = argv[3];
    budgetPercent = std::atof(argv[4]);
    fileStreamer = new FileStreamer(videoFilePath);

    std::cout
        << "Running Scene Analytics"
        << std::endl << std::flush;
    runConceptSummarization("Scene");

    std::cout
        << "Running FFObject Analytics"
        << std::endl << std::flush;
    runConceptSummarization("FFObject");

    std::cout
        << "Running DNObject Analytics"
        << std::endl << std::flush;
    runConceptSummarization("DNObject");

    std::cout
        << "Running FaceGender Analytics"
        << std::endl << std::flush;
    runConceptSummarization("FaceGender");

    std::cout
        << "Running FaceAge Analytics"
        << std::endl << std::flush;
    runConceptSummarization("FaceAge");

    std::cout
        << "Running FaceRec Analytics"
        << std::endl << std::flush;
    runConceptSummarization("FaceRec");
}
