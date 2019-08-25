/*
    Copyright 2016 AITOE
*/

#ifndef CAFFE_TRAINER_H
#define CAFFE_TRAINER_H

#include <iostream>
#include <vector>
#include <string>
#include <dirent.h>
#include <fstream>
#include <unordered_map>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include "boost/scoped_ptr.hpp"
#include "boost/algorithm/string.hpp"

#include <caffe/caffe.hpp>
#include "caffe/proto/caffe.pb.h"
#include "caffe/util/db.hpp"
#include "caffe/util/format.hpp"
#include "caffe/util/io.hpp"
#include "caffe/util/rng.hpp"
#include "caffe/util/upgrade_proto.hpp"
#include "caffe/util/signal_handler.h"

// typedef std::pair<std::string, float> Prediction;

namespace aiSaac {
class CaffeTrainer {
 public:
     CaffeTrainer(std::string trainingDataPath,
        std::string testingDataPath,
        std::string templateArchitecturePath,
        std::string originalCaffemodelPath,
        std::string workingDir,
        std::string newNetworkName = "Custom",
        std::string templateSolverPath = "");
    CaffeTrainer(std::string trainingDataPath,
        std::string testingDataPath,
        std::string templateArchitecturePath,
        std::string templateSolverPath,
        std::string originalCaffemodelPath,
        std::string solverStatePath,
        std::string workingDir,
        std::string newNetworkName = "Custom",
        std::string backpropOptimizationAlgo = "SGD",
        int maxIterations = 100,
        int testIterationSize = 100,
        int stepSize = 2500,
        float base_lr = 0.001,
        int solverMode = 0,
        double trainingRatio = 0.83,
        int resizeWidth = 227,
        int resizeHeight = 227,
        bool isColor = true,
        float gamma = 0.1);
    ~CaffeTrainer();

    void train();
 private:
    void createLmdb();
    void createLmdb(std::string trainingDataPath, std::string testingDataPath);
    void generateMean();
    void modifyAndSaveNetwork();
    void modifyAndSaveSolver();
    std::string dataFilePath;
    std::string trainingDataPath;
    std::string testingDataPath;
    std::string templateArchitecturePath;
    std::string templateSolverPath;
    std::string originalCaffemodelPath;
    std::string solverStatePath;
    std::string workingDir;
    std::string trainLmdbPath;
    std::string validationLmdbPath;
    std::string newModelFolderPath;
    std::string labelFilePath;
    std::string meanFilePath;
    std::string networkFilePath;
    std::string solverFilePath;

    std::vector<std::pair<std::string, int>> trainingList;
    std::vector<std::pair<std::string, int>> validationList;

    double trainingRatio;
    int resizeWidth;
    int resizeHeight;
    bool isColor;
    std::string newNetworkName;
    std::string backpropOptimizationAlgo;
    int numberOfClasses;
    int maxIterations;
    int testIterationSize;
    int stepSize;
    float baseLr;
    float gamma;
    int solverMode;
};
}  // namespace aiSaac

#endif  // CAFFE_TRAINER_H
