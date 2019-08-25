/*
    Copyright 2016 AITOE
*/

#ifndef SRC_AISAAC_AISAACSETTINGS_H_
#define SRC_AISAAC_AISAACSETTINGS_H_

#include <string>
#include <fstream>
#include <vector>
#include "utils/json.hpp"

namespace aiSaac {
class AiSaacSettings {
 public:
    [[deprecated("Replaced with AiSaacSettings(std::string aiSaacSettingsFilePath)")]]
    AiSaacSettings(std::string settingsFilePath, std::string instanceId);
    AiSaacSettings(std::string settingsFilePath);
    ~AiSaacSettings();

    // Video Summarization
    void setSummarizationThreshold(double);
    double getSummarizationThreshold();

    void setSummarizationSNPSize(int);
    int getSummarizationSNPSize();

    void setSummarizationStreamAlgo(std::string algo);
    std::string getSummarizationStreamAlgo();

    void setSummarizationGreedyAlgo(std::string algo);
    std::string getSummarizationGreedyAlgo();

    void setSummarizationCollectionDir(std::string);
    std::string getSummarizationCollectionDir();

    void setSummarizationScenePrototxtPath(std::string filePath);
    std::string getSummarizationScenePrototxtPath();

    void setSummarizationSceneCaffemodelPath(std::string filePath);
    std::string getSummarizationSceneCaffemodelPath();

    void setSummarizationSceneMeanFilePath(std::string filePath);
    std::string getSummarizationSceneMeanFilePath();

    void setSummarizationSceneLabelFilePath(std::string filePath);
    std::string getSummarizationSceneLabelFilePath();

    void setSummarizationSceneFeatureExtractionLayer(std::string);
    std::string getSummarizationSceneFeatureExtractionLayer();

    void setSummarizationFfObjectPrototxtPath(std::string filePath);
    std::string getSummarizationFfObjectPrototxtPath();

    void setSummarizationFfObjectCaffemodelPath(std::string filePath);
    std::string getSummarizationFfObjectCaffemodelPath();

    void setSummarizationFfObjectMeanFilePath(std::string filePath);
    std::string getSummarizationFfObjectMeanFilePath();

    void setSummarizationFfObjectLabelFilePath(std::string filePath);
    std::string getSummarizationFfObjectLabelFilePath();

    void setSummarizationFfObjectFeatureExtractionLayer(std::string);
    std::string getSummarizationFfObjectFeatureExtractionLayer();

    void setSummarizationDnObjectPrototxtPath(std::string filePath);
    std::string getSummarizationDnObjectPrototxtPath();

    void setSummarizationDnObjectCaffemodelPath(std::string filePath);
    std::string getSummarizationDnObjectCaffemodelPath();

    void setSummarizationDnObjectMeanFilePath(std::string filePath);
    std::string getSummarizationDnObjectMeanFilePath();

    void setSummarizationDnObjectLabelFilePath(std::string filePath);
    std::string getSummarizationDnObjectLabelFilePath();

    void setSummarizationDnObjectFeatureExtractionLayer(std::string);
    std::string getSummarizationDnObjectFeatureExtractionLayer();

    void setSummarizationFaceGenderPrototxtPath(std::string filePath);
    std::string getSummarizationFaceGenderPrototxtPath();

    void setSummarizationFaceGenderCaffemodelPath(std::string filePath);
    std::string getSummarizationFaceGenderCaffemodelPath();

    void setSummarizationFaceGenderMeanFilePath(std::string filePath);
    std::string getSummarizationFaceGenderMeanFilePath();

    void setSummarizationFaceGenderLabelFilePath(std::string filePath);
    std::string getSummarizationFaceGenderLabelFilePath();

    void setSummarizationFaceGenderFeatureExtractionLayer(std::string);
    std::string getSummarizationFaceGenderFeatureExtractionLayer();

    void setSummarizationFaceAgePrototxtPath(std::string filePath);
    std::string getSummarizationFaceAgePrototxtPath();

    void setSummarizationFaceAgeCaffemodelPath(std::string filePath);
    std::string getSummarizationFaceAgeCaffemodelPath();

    void setSummarizationFaceAgeMeanFilePath(std::string filePath);
    std::string getSummarizationFaceAgeMeanFilePath();

    void setSummarizationFaceAgeLabelFilePath(std::string filePath);
    std::string getSummarizationFaceAgeLabelFilePath();

    void setSummarizationFaceAgeFeatureExtractionLayer(std::string);
    std::string getSummarizationFaceAgeFeatureExtractionLayer();

    void setSummarizationFaceRecPrototxtPath(std::string filePath);
    std::string getSummarizationFaceRecPrototxtPath();

    void setSummarizationFaceRecCaffemodelPath(std::string filePath);
    std::string getSummarizationFaceRecCaffemodelPath();

    void setSummarizationFaceRecMeanFilePath(std::string filePath);
    std::string getSummarizationFaceRecMeanFilePath();

    void setSummarizationFaceRecLabelFilePath(std::string filePath);
    std::string getSummarizationFaceRecLabelFilePath();

    void setSummarizationFaceRecFeatureExtractionLayer(std::string);
    std::string getSummarizationFaceRecFeatureExtractionLayer();

    // BgSubAnalytics
    void setBackgroundSubtractionAlgo(std::string algo);
    std::string getBackgroundSubtractionAlgo();

    void setMinimumWindowFraction(double value);
    double getMinimumWindowFraction();

    void setMaximumWindowFraction(double value);
    double getMaximumWindowFraction();

    void setBackgroundUpdateTime(int frames);
    int getBackgroundUpdateTime();

    void setNoMotionBackgroundUpdateTime(int frames);
    int getNoMotionBackgroundUpdateTime();

    void setBlobDeleteTime(int frames);
    int getBlobDeleteTime();

    void setMinimumBlobFrames(int frames);
    int getMinimumBlobFrames();

    // HeadAnalytics
    void setHeadAnalyticsAlgo(std::string algo);
    std::string getHeadAnalyticsAlgo();

    void setMinimumHeadSize(double x, double y);
    std::vector<double> getMinimumHeadSize();

    void setMaximumHeadSize(double x, double y);
    std::vector<double> getMaximumHeadSize();

    void setHaarCascadeFilePath(std::string filePath);
    std::string getHaarCascadeFilePath();

    void setHeadBlobDeleteTime(int frames);
    int getHeadBlobDeleteTime();

    void setMinimumHeadBlobFrames(int frames);
    int getMinimumHeadBlobFrames();

    double getHeadScaleFactor();
    int getHeadMinNeighbours();
    void setHeadScaleFactor(double val);
    void setHeadMinNeighbours(int val);

    // HumanAnalytics
    void setHumanDetectAnalyticsAlgo(std::string algo);
    std::string getHumanDetectAnalyticsAlgo();

    int getshirtTopYFactor();
    int getshirtBottomYFactor();

    void setHumanBlobDeleteTime(int frames);
    int getHumanBlobDeleteTime();

    void setMinimumHumanBlobFrames(int frames);
    int getMinimumHumanBlobFrames();

    void setDPMPedestrianModelFilePath(std::string filePath);
    std::string getDPMPedestrianModelFilePath();

    void setICFPedestrianModelFilePath(std::string filePath);
    std::string getICFPedestrianModelFilePath();

    int getDPM_interval();
    int getDPM_min_neighbours();
    int getDPM_flags();
    float getDPM_threshold();
    void setDPM_interval(int val);
    void setDPM_min_neighbours(int val);
    void setDPM_flags(int val);
    void setDPM_threshold(float val);

    int getICF_interval();
    int getICF_min_neighbours();
    int getICF_flags();
    int getICF_step_through();
    float getICF_threshold();
    void setICF_interval(int val);
    void setICF_min_neighbours(int val);
    void setICF_flags(int val);
    void setICF_step_through(int val);
    void setICF_threshold(float val);


    // FaceAnalytics
    void setFaceDetectAnalyticsAlgo(std::string algo);
    std::string getFaceDetectAnalyticsAlgo();

    void setFaceBlobDeleteTime(int frames);
    int getFaceBlobDeleteTime();

    void setMinimumFaceBlobFrames(int frames);
    int getMinimumFaceBlobFrames();

    void setMinimumFaceSize(double x, double y);
    std::vector<double> getMinimumFaceSize();

    void setMaximumFaceSize(double x, double y);
    std::vector<double> getMaximumFaceSize();

    void setPositionDistanceFactor(int factor);
    int getPositionDistanceFactor();

    void setFeatureExtractionLayer(std::string layer);
    std::string getFeatureExtractionLayer();

    void setNPDFaceDetectFilePath(std::string filePath);
    std::string getNPDFaceDetectFilePath();

    void setDarknetFaceDetectConfigFilePath(std::string filePath);
    std::string getDarknetFaceDetectConfigFilePath();

    void setDarknetFaceDetectWeightFilePath(std::string filePath);
    std::string getDarknetFaceDetectWeightFilePath();

    void setDarknetFaceDetectLabelFilePath(std::string filePath);
    std::string getDarknetFaceDetectLabelFilePath();

    void setDarknetFaceDetectConfidenceThreshold(double thres);
    double getDarknetFaceDetectConfidenceThreshold();

    void setFaceStorageMode(bool mode);
    bool getFaceStorageMode();

    void setFaceRecognitionTrainingDataPath(std::string folderPath);
    std::string getFaceRecognitionTrainingDataPath();

    void setVGGFacePrototxtPath(std::string filePath);
    std::string getVGGFacePrototxtPath();

    void setVGGFaceCaffemodelPath(std::string filePath);
    std::string getVGGFaceCaffemodelPath();

    void setFaceRecognitionDBPath(std::string filePath);
    std::string getFaceRecognitionDBPath();

    void setFaceRecognitionModelAlgo(std::string algo);
    std::string getFaceRecognitionModelAlgo();

    void setSimilarityThreshold(double k);
    double getSimilarityThreshold();

    void setKnnThreshold(int k);
    int getKnnThreshold();

    void setGenderPrototxtPath(std::string filePath);
    std::string getGenderPrototxtPath();

    void setGenderCaffemodelPath(std::string filePath);
    std::string getGenderCaffemodelPath();

    void setGenderMeanFilePath(std::string filePath);
    std::string getGenderMeanFilePath();

    void setGenderLabelFilePath(std::string filePath);
    std::string getGenderLabelFilePath();

    void setGenderRecognitionThreshold(double threshold);
    double getGenderRecognitionThreshold();

    void setAgePrototxtPath(std::string filePath);
    std::string getAgePrototxtPath();

    void setAgeCaffemodelPath(std::string filePath);
    std::string getAgeCaffemodelPath();

    void setAgeMeanFilePath(std::string filePath);
    std::string getAgeMeanFilePath();

    void setAgeLabelFilePath(std::string filePath);
    std::string getAgeLabelFilePath();

    void setAgeRecognitionThreshold(double threshold);
    double getAgeRecognitionThreshold();

    void setEmotionPrototxtPath(std::string filePath);
    std::string getEmotionPrototxtPath();

    void setEmotionCaffemodelPath(std::string filePath);
    std::string getEmotionCaffemodelPath();

    void setEmotionMeanFilePath(std::string filePath);
    std::string getEmotionMeanFilePath();

    void setEmotionLabelFilePath(std::string filePath);
    std::string getEmotionLabelFilePath();

    // Object Analytics
    void setObjectAnalyticsAlgo(std::string algo);
    std::string getObjectAnalyticsAlgo();

    void setObjectBlobDeleteTime(int frames);
    int getObjectBlobDeleteTime();

    void setMinimumObjectBlobFrames(int frames);
    int getMinimumObjectBlobFrames();

    void setObjectCaffePrototxtPath(std::string filePath);
    std::string getObjectCaffePrototxtPath();

    void setObjectCaffeCaffemodelPath(std::string filePath);
    std::string getObjectCaffeCaffemodelPath();

    void setObjectCaffeMeanFilePath(std::string filePath);
    std::string getObjectCaffeMeanFilePath();

    void setObjectCaffeLabelFilePath(std::string filePath);
    std::string getObjectCaffeLabelFilePath();

    void setObjectDarknetConfigFilePath(std::string filePath);
    std::string getObjectDarknetConfigFilePath();

    void setObjectDarknetWeightFilePath(std::string filePath);
    std::string getObjectDarknetWeightFilePath();

    void setObjectDarknetLabelFilePath(std::string filePath);
    std::string getObjectDarknetLabelFilePath();

    void setObjectDNNPrototxtPath(std::string filePath);
    std::string getObjectDNNPrototxtPath();

    void setObjectDNNCaffemodelPath(std::string filePath);
    std::string getObjectDNNCaffemodelPath();

    void setObjectDNNMeanFilePath(std::string filePath);
    std::string getObjectDNNMeanFilePath();

    void setObjectDNNLabelFilePath(std::string filePath);
    std::string getObjectDNNLabelFilePath();

    void setObjectAnalyticsConfidenceThreshold(float confidence);
    float getObjectAnalyticsConfidenceThreshold();

    // Scene Analytics
    void setSceneAnalyticsAlgo(std::string algo);
    std::string getSceneAnalyticsAlgo();

    void setScenePrototxtPath(std::string filePath);
    std::string getScenePrototxtPath();

    void setSceneCaffemodelPath(std::string filePath);
    std::string getSceneCaffemodelPath();

    void setSceneMeanFilePath(std::string filePath);
    std::string getSceneMeanFilePath();

    void setSceneLabelFilePath(std::string filePath);
    std::string getSceneLabelFilePath();

    void setSceneAnalyticsConfidenceThreshold(float confidence);
    float getSceneAnalyticsConfidenceThreshold();

    void setSceneBlobDeleteTime(int frames);
    int getSceneBlobDeleteTime();

    void setMinimumSceneBlobFrames(int frames);
    int getMinimumSceneBlobFrames();

    // TextAnalytics
    void setTextAnalyticsAlgo(std::string algo);
    std::string getTextAnalyticsAlgo();

    void setTextBlobDeleteTime(int frames);
    int getTextBlobDeleteTime();

    void setMinimumTextBlobFrames(int frames);
    int getMinimumTextBlobFrames();

    void setTextRectDistanceThresh(int threshold);
    int getTextRectDistanceThresh();

    // Vehicle Analytics
    void setVehicleAnalyticsAlgo(std::string algo);
    std::string getVehicleAnalyticsAlgo();

    void setVehicleBlobDeleteTime(int frames);
    int getVehicleBlobDeleteTime();

    void setMinimumVehicleBlobFrames(int frames);
    int getMinimumVehicleBlobFrames();

    void setMinimumVehicleSize(int x, int y);
    std::vector<int> getMinimumVehicleSize();

    void setMaximumVehicleSize(int x, int y);
    std::vector<int> getMaximumVehicleSize();

    // NSFW Analytics
    void setNSFWAnalyticsAlgo(std::string algo);
    std::string getNSFWAnalyticsAlgo();

    void setNSFWCaffePrototxtPath(std::string filePath);
    std::string getNSFWCaffePrototxtPath();

    void setNSFWCaffeCaffemodelPath(std::string filePath);
    std::string getNSFWCaffeCaffemodelPath();

    void setNSFWCaffeLabelFilePath(std::string filePath);
    std::string getNSFWCaffeLabelFilePath();

    void setNSFWAnalyticsConfidenceThreshold(float confidence);
    float getNSFWAnalyticsConfidenceThreshold();

    void setProcFPS(int procFPS);
    int getProcFPS();

    void loadSettings();
    void saveSettings();
    void resetDefaultValues();

    nlohmann::json settings;  // todo make private

 private:
     std::string instanceId;
     std::string settingsFileName;
     std::string settingsFilePath;
};
}  // namespace aiSaac

#endif  // SRC_AISAAC_AISAACSETTINGS_H_
