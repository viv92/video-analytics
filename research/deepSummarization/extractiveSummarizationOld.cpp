#include <iostream>
#include <cv.h>
#include <opencv2/highgui.hpp>
// #include "aiSaac.h"
#include "../../tools/videoSources/fileStreamer.h"
#include "../../tools/logging.h"
#include "../../src/utils/caffeClassifier.h"
#include "datk.h"

void normalizeVector(std::vector<double> &featureVector) {
    float max = 0;
    for (int i = 0; i < featureVector.size(); i++) {
        max = (featureVector[i] > max) ? featureVector[i] : max;
    }
    for (int i = 0; i < featureVector.size(); i++) {
        featureVector[i] = featureVector[i] / max;
    }
}

void computeKernel(std::vector<std::vector<double>> &featureVectors,
    std::vector<std::vector<float>> &kernel) {
    double val = 0;
    double min = std::numeric_limits<double>::max();
    double max = std::numeric_limits<double>::min();
    for (int i = 0; i < featureVectors.size(); i++) {
        std::vector<float> currentVector;
        for (int j = 0; j < featureVectors.size(); j++) {
            val = datk::innerProduct(featureVectors[i], featureVectors[j]);
            max = (val > max) ? val : max;
            min = (val < min) ? val : min;
            currentVector.push_back(val);
        }
        kernel.push_back(currentVector);
    }
    // Normalize kernel matrix:
    double range = max - min;
    for (int i = 0; i < kernel.size(); i++) {
        for (int j = 0; j < kernel.size(); j++) {
            kernel[i][j] = (kernel[i][j] - min) / range;
        }
    }
}

int main (int argc, char *argv[]) {
    if (argc < 3) {
        std::cout << "Usage: ./deepSummarization <video_to_summarize> <summary_destination_path> <mode_of_operation> <%> <snippet_size> <model_1_folder> <model_1_feature_layer> <model_2_folder> <model_2_feature_layer> ... " << std::endl;
        return 0;
    }
    FileStreamer *fileStreamer = new FileStreamer(argv[1]);
    std::string summaryDestinationPath = argv[2];
    // int budgetPerformance = std::atoi(argv[3]);
    int modeOfOperation = std::atoi(argv[3]);
    std::cout << "Frame Rate: " << fileStreamer->getFileFPS() << std::endl;
    int snippetSize = std::atoi(argv[5]) * fileStreamer->getFileFPS();
    std::cout << "snippetSize: " << snippetSize << std::endl;


    std::cout << "Waiting for file to stream ...";
    while (!fileStreamer->isStreaming()) {
        std::cout << ".";
    }
    std::cout << "Ready and streaming!" << std::endl;

    std::vector<std::pair<aiSaac::CaffeClassifier, std::string>> caffeFeatureExtractors;

    std::cout << "loading caffe classifiers" << std::endl;

    for (int i = 6; i < argc; i += 2) {
        std::string modelFolder = argv[i];
        std::cout << "loading " << argv[i]
            << " ... feature extraction layer: " << argv[i + 1] << std::endl;
        aiSaac::CaffeClassifier classifier(modelFolder + "/deploy.prototxt",
            modelFolder + "/deploy.caffemodel",
            modelFolder + "/mean.binaryproto",
            modelFolder + "/labels.txt");
        std::pair<aiSaac::CaffeClassifier, std::string> caffeFeatureExtractor = std::make_pair(classifier, argv[i + 1]);
        caffeFeatureExtractors.push_back(caffeFeatureExtractor);
    }

    std::cout << "loaded "
        << caffeFeatureExtractors.size()
        << " caffe classifiers"
        << std::endl;

    // std::cout << "here i am 1" << std::endl;
    cv::Mat currentFrame;
    std::vector<std::vector<double>> finalFeatureVectors;
    std::vector<cv::Mat> snippetBuffer;
    std::vector<int> frameNumbers;
    int frameNumber = 0;
    int snippetNumber = 0;
    bool videoEnded = false;
    std::cout << "Going through video and extracting features" << std::endl;
    while (true) {
        snippetBuffer.push_back(fileStreamer->getFrame());
        // std::cout << "here i am 3" << std::endl;
        if (snippetBuffer.back().data == NULL ||
            snippetBuffer.back().empty() ||
            !fileStreamer->isStreaming()) {
            videoEnded = true;
            snippetBuffer.pop_back();
            std::cout << "Stopping file reading: " << videoEnded << std::endl;
        } else {
            // std::cout << "videoEnded: " << videoEnded << std::endl;
            cv::imshow("Video", snippetBuffer.back());
        }

        // std::cout << "here i am 4" << std::endl;
        if (snippetBuffer.size() && (snippetBuffer.size() == snippetSize || videoEnded)) {
            // std::cout << "extracting features for past "
                // << snippetBuffer.size() << " frames" << std::endl;
            std::vector<double> snippetFeatureVector;
            for (int i = 0; i < snippetBuffer.size(); i++) {
                // std::cout << "inserting features of each classifier into massiveFeatureVector" << std::endl;
                std::vector<double> massiveFeatureVector;
                for (int j = 0; j < caffeFeatureExtractors.size(); j++) {
                    // std::cout << "classifier ==========" << j << std::endl;
                    std::vector<float> floatFeatureVector = caffeFeatureExtractors[j].first.Predict(
                        snippetBuffer[i],
                        caffeFeatureExtractors[j].second);

                    std::vector<double> featureVector(floatFeatureVector.begin(), floatFeatureVector.end());
                    // std::cout << " ==================== " << std::endl;
                    if (massiveFeatureVector.empty()) {
                        massiveFeatureVector = featureVector;
                    } else {
                        massiveFeatureVector.insert(massiveFeatureVector.end(),
                            featureVector.begin(),
                            featureVector.end());
                    }
                }
                // std::cout << "Size of final massiveFeatureVector: " << massiveFeatureVector.size() << std::endl;
                // std::cout << "Normalizing massiveFeatureVector" << std::endl;
                normalizeVector(massiveFeatureVector);
                // for (int i = 0; i < massiveFeatureVector.size(); i++) {
                //     std::cout << "massiveFeatureVector : " << massiveFeatureVector[i] << std::endl;
                // }
                if (snippetFeatureVector.empty()) {
                    snippetFeatureVector = massiveFeatureVector;
                } else {
                    // std::cout << "doing vector addition: " << std::endl;

                    for (int j = 0; j < snippetFeatureVector.size(); j++) {
                        // std::cout << snippetFeatureVector[j] << " + " << massiveFeatureVector[j] << " = ";
                        snippetFeatureVector[j] += massiveFeatureVector[j];
                        // std::cout << snippetFeatureVector[j] << std::endl;
                    }
                    // datk::vectorAddition(snippetFeatureVector,
                    //     massiveFeatureVector,
                    //     snippetFeatureVector);
                }
            }
            // std::cout << "*********Averaging feature vector*********" << std::endl;

            for (int i = 0; i < snippetFeatureVector.size(); i++) {
                // std::cout << snippetFeatureVector[i] << " / " << snippetBuffer.size() << " = ";
                snippetFeatureVector[i] = snippetFeatureVector[i] / (double) snippetBuffer.size();
                // std::cout << snippetFeatureVector[i] << std::endl;
            }
            // datk::scalarMultiplication(snippetFeatureVector, 1 / snippetBuffer.size(), snippetFeatureVector);
            finalFeatureVectors.push_back(snippetFeatureVector);
            frameNumbers.push_back(snippetNumber);
            snippetBuffer.clear();
            snippetNumber = frameNumber + 1;
        }
        if (videoEnded) {
            std::cout << "video has ended" << std::endl;
            break;
        }

        frameNumber++;
    }
    delete fileStreamer;

    double budget = ceil((std::atoi(argv[4]) * finalFeatureVectors.size()) / 100);
    std::cout << "Budget: " << budget << std::endl;

    std::vector<std::vector<float>> kernel;
    computeKernel(finalFeatureVectors, kernel);
    datk::Set indices;
    if (modeOfOperation == 0) {
        datk::FacilityLocation facilityLocation(kernel.size(), kernel);
        datk::lazyGreedyMax(facilityLocation, budget, indices);
    } else if (modeOfOperation == 1) {
        datk::DisparityMin dispMin(kernel.size(), kernel);
        //call lazygreedymax with this facility location fuction, B, null output set
        datk::naiveGreedyMax(dispMin, budget, indices, 0, false, true);
    }
    std::cout << "Indices: " << indices.size() << std::endl;
    std::set<int> keyPointIndices = set<int>();
    for (datk::Set::iterator it = indices.begin(); it!=indices.end(); it++) {
        keyPointIndices.insert(*it);
    }

    // now store the video
    fileStreamer = new FileStreamer(argv[1]);
    cv::VideoWriter summarizedVideo;
    cv::Size S = fileStreamer->getFileFrameSize();
    int FPS = fileStreamer->getFileFPS();
    summarizedVideo.open(argv[2], CV_FOURCC('M', 'J', 'P', 'G') , FPS, S, true);

    std::cout << "Waiting for file to stream ...";
    while (!fileStreamer->isStreaming()) {
        std::cout << ".";
    }
    std::cout << "Ready and streaming!" << std::endl;
    cv::Mat summaryVideoFrames;
    bool stop = false;
    // for (set<int>::iterator it = keyPointIndices.begin(); it!=keyPointIndices.end(); it++) {
    //     std::cout << "keyPoint: " << *it << std::endl;
    //     std::cout << "keyPoint * fileStreamer->getFileFPS(): " << *it * fileStreamer->getFileFPS() << std::endl;
    // }

    for (set<int>::iterator it = keyPointIndices.begin(); it!=keyPointIndices.end(); it++) {
        std::cout << "keyPoint: " << *it << std::endl;
        std::cout << "frameNumbers[keyPoint] * fileStreamer->getFileFPS: "
            << *it * fileStreamer->getFileFPS() << std::endl;
        int seekFrame = *it * snippetSize;
        fileStreamer->cap->set(CV_CAP_PROP_POS_FRAMES, seekFrame);
        int count = 0;
        while (count <= snippetSize) {
            if (!fileStreamer->cap->read(summaryVideoFrames)) {
                stop = true;
                break;
            } else {
                summarizedVideo.write(summaryVideoFrames);
            }
            count++;
        }
        if (stop) {
            break;
        }
    }
    summarizedVideo.release();

    return 0;
}
