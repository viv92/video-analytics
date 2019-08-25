#include "../../src/utils/datk/src/datk.h"
#include "../../src/aiSaac.h"

class SubsetSelection {
  public:
    SubsetSelection(aiSaac::CustomAnalytics *ca);
    ~SubsetSelection();
    void runExperiment(int algo,
        double budget,
        int k,
        int &numCorrect,
        int &numInCorrect,
        int mode,
        int &total,
        nlohmann::json fullTrainJsonData,
        nlohmann::json fullTrainFileNamesJsonData);
  private:
    datk::Set getBIndicesRandom(int n, std::vector<std::vector<double>> &featureVectors, double budget);
    datk::Set getBIndicesFacilityLocation(int n, std::vector<std::vector<double>> &featureVectors, double budget);
    datk::Set getBIndicesDisparityMin(int n, std::vector<std::vector<double>> &featureVectors, double budget);
    datk::Set getBIndicesFeatureBasedFunctions(int n, std::vector<std::vector<double>> &featureVectors, double budget, int mode);
    aiSaac::CustomAnalytics *ca;
};
