/*
    Copyright 2016 AITOE
*/

#ifndef CA_SETTINGS_H
#define CA_SETTINGS_H

#include <string>
#include <fstream>
#include <vector>
#include "../utils/json.hpp"

namespace aiSaac {
class CASettings {
 public:
    [[deprecated("Replaced with CASettings(std::string caSettingsFilePath)")]]
    CASettings(std::string caSettingsFilePath, std::string CAID);
    CASettings(std::string caSettingsFilePath);
    ~CASettings();

    void setMode(int mode);
    int getMode();

    void setCaffeNetworkFilePath(std::string networkFilePath);
    std::string getCaffeNetworkFilePath();

    void setCaffeTrainedFilePath(std::string trainedFilePath);
    std::string getCaffeTrainedFilePath();

    void setCaffeTemplateArchitecturePath(std::string templateArchitecturePath);
    std::string getCaffeTemplateArchitecturePath();

    void setCaffeTemplateSolverPath(std::string templateSolverPath);
    std::string getCaffeTemplateSolverPath();

    void setOriginalCaffeModelPath(std::string originalCaffemodelPath);
    std::string getOriginalCaffeModelPath();

    void setSolverStatePath(std::string solverStatePath);
    std::string getSolverStatePath();

    void setWorkingDir(std::string workingDir);
    std::string getWorkingDir();

    void setCaffeModelFilePath(std::string caffeModelFilePath);
    std::string getCaffeModelFilePath();

    void setNewCaffeTrainedFilePath(std::string newCaffeTrainedFilePath);
    std::string getNewCaffeTrainedFilePath();

    void setMeanFilePath(std::string meanFilePath);
    std::string getMeanFilePath();

    void setCaffeFeatureExtractionLayer(std::string layer);
    std::string getCaffeFeatureExtractionLayer();

    void setLabelFilePath(std::string filePath);
    std::string getLabelFilePath();

    void setTrainingDataPath(std::string trainingDataPath);
    std::string getTrainingDataPath();

    void setTestingDataPath(std::string testingDataPath);
    std::string getTestingDataPath();

    void setTrainingJsonDataPath(std::string trainingJsonDataPath);
    std::string getTrainingJsonDataPath();
    
    void setTrainingFileNamesJsonDataPath(std::string trainingFileNamesJsonDataPath);
    std::string getTrainingFileNamesJsonDataPath();

    void setTestingJsonDataPath(std::string testingJsonDataPath);
    std::string getTestingJsonDataPath();

    void setIsTrained(bool isTrained);
    bool getIsTrained();

    void setIsTested(bool isTrained);
    bool getIsTested();

    void setMLAlgo(std::string algo);
    std::string getMLAlgo();

    void setDATKPredictionProbThresh(double thresh);
    double getDATKPredictionProbThresh();

    // Logistic Regression L1
    void setLRL1RegularizationParam(int lambda);
    int getLRL1RegulatizationParam();

    void setLRL1OptimizationAlgo(int optAlgo);
    int getLRL1OptimizationAlgo();

    void setLRL1NumOfIterations(int iterations);
    int getLRL1NumOfIterations();

    void setLRL1Tolerance(double tolerance);
    double getLRL1Tolerance();

    // Logistic Regression L2
    void setLRL2RegularizationParam(int lambda);
    int getLRL2RegulatizationParam();

    void setLRL2OptimizationAlgo(int optAlgo);
    int getLRL2OptimizationAlgo();

    void setLRL2NumOfIterations(int iterations);
    int getLRL2NumOfIterations();

    void setLRL2Tolerance(double tolerance);
    double getLRL2Tolerance();

    // Hinge-SVM L2
    void setHSVML2RegularizationParam(int lambda);
    int getHSVML2RegulatizationParam();

    void setHSVML2OptimizationAlgo(int optAlgo);
    int getHSVML2OptimizationAlgo();

    void setHSVML2NumOfIterations(int iterations);
    int getHSVML2NumOfIterations();

    void setHSVML2Tolerance(double tolerance);
    double getHSVML2Tolerance();

    // Smooth-SVM L1
    void setSSVML1RegularizationParam(int lambda);
    int getSSVML1RegulatizationParam();

    void setSSVML1OptimizationAlgo(int optAlgo);
    int getSSVML1OptimizationAlgo();

    void setSSVML1NumOfIterations(int iterations);
    int getSSVML1NumOfIterations();

    void setSSVML1Tolerance(double tolerance);
    double getSSVML1Tolerance();

    // Smooth-SVM L2
    void setSSVML2RegularizationParam(int lambda);
    int getSSVML2RegulatizationParam();

    void setSSVML2OptimizationAlgo(int optAlgo);
    int getSSVML2OptimizationAlgo();

    void setSSVML2NumOfIterations(int iterations);
    int getSSVML2NumOfIterations();

    void setSSVML2Tolerance(double tolerance);
    double getSSVML2Tolerance();

    void setDATKSaveModelPath(std::string modelPath);
    std::string getDATKSaveModelPath();

    // Solver Parameters
    void setNewNetworkName(std::string newNetworkName);
    std::string getNewNetworkName();

    void setBackpropOptimizationAlgo(std::string backpropOptimizationAlgo);
    std::string getBackpropOptimizationAlgo();

    void setTrainingRatio(double trainingRatio);
    double getTrainingRatio();

    void setResizeWidth(int resizeWidth);
    int getResizeWidth();

    void setResizeHeight(int resizeHeight);
    int getResizeHeight();

    void setIsColor(bool isColor);
    bool getIsColor();

    void setMaxIterations(int maxIterations);
    int getMaxIterations();

    void setTestIterationSize(int testIterationSize);
    int getTestIterationSize();

    void setStepSize(int stepSize);
    int getStepSize();

    void setBaseLR(float base_lr);
    float getBaseLR();

    void setGamma(float gamma);
    float getGamma();

    void setSolverMode(int solverMode);
    int getSolverMode();

    void loadSettings();
    void saveSettings();
    void resetDefaultValues();
 private:
    nlohmann::json settings;

    std::string CAID;
    std::string caSettingsFileName;
    std::string caSettingsFilePath;
};
}

#endif  // CA_SETTINGS_H
