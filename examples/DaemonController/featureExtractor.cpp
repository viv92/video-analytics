#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <vector>
#include <string>
#include "../../tools/mongoLink/serialLink.h"
#include "../../src/utils/caffeClassifier.h"

int main(int argc, char const *argv[]) {
    if (argc < 4) {
        std::cout << "Usage: ./featureExtractor [caffeNetworkFilePath] [caffeTrainedFilePath] [featureExtractionLayer] [featureVectorSize] [featureType] [numImages] <[imageFilePath]>" << std::endl;
        return -1;
    }

    std::string caffeNetworkFilePath = argv[1];
    std::string caffeTrainedFilePath = argv[2];
    std::string featureExtractionLayer = argv[3];
    int featureVectorSize = atoi(argv[4]);
    std::string featureType = argv[5];
    int numImages = atoi(argv[6]);
    std::vector<float> extractedFeatureVector = std::vector<float>(featureVectorSize, 0.0);
    std::vector<float> avgFeatureVector = std::vector<float>(featureVectorSize, 0.0);

    mongoLink* mongoObject = new mongoLink(3);
    aiSaac::CaffeClassifier* caffeClassifier = new aiSaac::CaffeClassifier(caffeNetworkFilePath, caffeTrainedFilePath);

    for (int i = 0; i < numImages; i++) {
        extractedFeatureVector = caffeClassifier->Predict(cv::imread(argv[7 + i]), featureExtractionLayer);
        for (int j = 0; j < featureVectorSize; j++) {
            avgFeatureVector.at(j) += extractedFeatureVector.at(j);
        }
    }

    for (int i = 0; i < featureVectorSize; i++) {
        avgFeatureVector.at(i) = (avgFeatureVector.at(i) / numImages);
    }

    mongoObject->extractedFeatureMONGO(featureType, avgFeatureVector);
    return 0;
}
