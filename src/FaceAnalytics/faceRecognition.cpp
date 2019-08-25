/*
    Copyright 2016 AITOE
*/

/*
    1. add aiSaac Algo Setting (getFaceRecognitionFeaturizationAlgo) to choose which way to featurize face (VGG/OpenFace)
    2. add aiSaac Algo Setting (getFaceRecognitionModelAlgo) to choose which Machine Learning algo to use (KNN/SVM/Logistic Regression/FineTuning)
    3. add KNN code to DATK
    4. add train()
    5. Make the extracted features Sparse (DATK)
*/

#include "faceRecognition.h"
#include "faceAnalytics.h"

struct Similarity {
    std::string labelName;
    float sim;
};

bool sortBySim(const Similarity &a, const Similarity &b) {
    return a.sim > b.sim;
}

aiSaac::FaceRecognition::FaceRecognition(aiSaac::AiSaacSettings &aiSaacSettings) : aiSaacSettings(aiSaacSettings) {
    this->initialize();
}

aiSaac::FaceRecognition::~FaceRecognition() {}

void aiSaac::FaceRecognition::initialize() {
    this->caffeClassifier = new aiSaac::CaffeClassifier(
        this->aiSaacSettings.getVGGFacePrototxtPath(),
        this->aiSaacSettings.getVGGFaceCaffemodelPath());
    this->loadFaceDB();
    // this->addStoredFacesToDB();  // codeReview(Anurag): should this be here?
}

std::string aiSaac::FaceRecognition::recognize(const cv::Mat &rawFrame) {
    std::string faceRecognizeAlgo = this->aiSaacSettings.getFaceRecognitionModelAlgo();
    cv::Mat avgRawFrame(rawFrame.rows,
        rawFrame.cols,
        CV_8UC3,
        cv::Scalar(93.5940, 104.7624, 129.1863));
    cv::Mat predictionFrame = rawFrame - avgRawFrame;
    std::string layer= "fc7";
    std::vector<float> prediction = this->caffeClassifier->Predict(predictionFrame, layer);
    // std::vector<double> similarityVector;
    // std::vector<std::string> labels;
    // std::vector<double> topTenSimilarities;
    // std::vector<double> topTenLabels;
    // double smallestSimValue = 1;
    // int smallestSimIndex = 0;
    // double maxSimValue = 0;
    std::string mostLikelyLabel;

    if (faceRecognizeAlgo == "KNN") {
        std::string finalLabel;
        // codeReview(Anurag): Move this to DATK
        std::vector<Similarity> similarities;
        for (nlohmann::json::iterator person = this->faceDB.begin();
            person != this->faceDB.end();
            person++) {
            for (nlohmann::json::iterator storedFace = person.value().begin();
                storedFace != person.value().end();
                storedFace++) {
                std::vector<float> storedFeature = *storedFace;
                float innerProduct = 0;
                for (int i = 0; i < prediction.size(); i++) {
                    innerProduct += storedFeature[i] * prediction[i];
                }
                Similarity similarity;
                similarity.labelName = person.key();
                similarity.sim = innerProduct /
                    (norm(storedFeature, cv::NORM_L2) * norm(prediction, cv::NORM_L2));
                similarities.push_back(similarity);
                // if (similarity > maxSimValue) {
                //     maxSimValue = similarity;
                //     finalLabel = person.key();
                // }
            }
        }

        std::sort(similarities.begin(), similarities.end(), sortBySim);
        std::unordered_map<std::string, int> topTenSims;
        for (int i = 0;
            i < this->aiSaacSettings.getKnnThreshold() && i != similarities.size();
            i++) {
            if (!topTenSims.count(similarities[i].labelName)) {
                topTenSims[similarities[i].labelName] = 0;
            }
            topTenSims[similarities[i].labelName]++;
        }
        int maxCount = -1;
        for (auto i = topTenSims.begin(); i != topTenSims.end(); i++) {
            // std::cout << "labelName: " << i->first << ", count: " << i->second << std::endl;
            if (i->second > maxCount) {
                maxCount = i->second;
                mostLikelyLabel = i->first;
            }
        }
    }

    // std::cout << "mostLikelyLabel: " << mostLikelyLabel << std::endl;

    // std::cout << "MaxSimValue: " << maxSimValue << std::endl;
    return mostLikelyLabel;
}

void aiSaac::FaceRecognition::addToFaceDB(const cv::Mat &rawFrame, std::string label) {
    cv::Mat avgRawFrame(rawFrame.rows,
        rawFrame.cols,
        CV_8UC3,
        cv::Scalar(93.5940, 104.7624, 129.1863));

    cv::Mat predictionFrame = rawFrame - avgRawFrame;
    std::vector<float> prediction = this->caffeClassifier->Predict(predictionFrame, "fc7");
    this->faceDB[label].push_back(prediction);
    this->saveFaceDB();
}

void aiSaac::FaceRecognition::addStoredFacesToDB() {
    // check if DB already exists
    // unsigned char isFolder =0x4;
    std::cout << "adding stored faces in faceStorage folder to faceDB" << std::endl;
    DIR *faceStorageDir;
    struct dirent *direntFaceStorageDir;
    DIR *personPath;
    struct dirent *direntPersonPath;
    faceStorageDir = opendir(this->aiSaacSettings.getFaceRecognitionTrainingDataPath().c_str());
    std::string personFolder;
    std::string personName;
    std::string faceImagePath;

    while((direntFaceStorageDir = readdir(faceStorageDir)) != NULL) {
        // std::cout << direntFaceStorageDir->d_name;
        personFolder = this->aiSaacSettings.getFaceRecognitionTrainingDataPath();
        if (direntFaceStorageDir->d_type == DT_DIR
            && strcmp(direntFaceStorageDir->d_name, ".")
            && strcmp(direntFaceStorageDir->d_name, "..")) {
            personName = direntFaceStorageDir->d_name;
            personFolder = personFolder + "/" + direntFaceStorageDir->d_name;
            personPath = opendir(personFolder.c_str());
            while ((direntPersonPath = readdir(personPath))) {
                if (direntPersonPath->d_type == DT_REG) {
                    std::string fileName(direntPersonPath->d_name);
                    if (fileName.substr(fileName.find_last_of(".") + 1) != "DS_Store") {
                        faceImagePath = personFolder + "/" + fileName;
                        std::cout << "final face image path: " << faceImagePath << std::endl;
                        this->addToFaceDB(cv::imread(faceImagePath), personName);
                    } else {
                        std::cout << "Ignoring " << fileName << std::endl;
                    }
                }
            }
        }
    }
}

void aiSaac::FaceRecognition::loadFaceDB() {
    std::ifstream f(this->aiSaacSettings.getFaceRecognitionDBPath());
    if (f.good()) {
        std::cout << "Loading settings" << std::endl;
        std::ifstream configFile;
        configFile.open(this->aiSaacSettings.getFaceRecognitionDBPath());
        configFile >> this->faceDB;
    } else {
        std::cout << "Cannot find settings file. Creating." << std::endl;
        this->saveFaceDB();
        // todo advanceSettings->resetDefaultValues();
    }
}

void aiSaac::FaceRecognition::saveFaceDB() {
    std::cout << "saving faceDB" << this->aiSaacSettings.getFaceRecognitionDBPath() << std::endl;
    std::ofstream configFile;
    configFile.open(this->aiSaacSettings.getFaceRecognitionDBPath());
    configFile << this->faceDB.dump(4);
    configFile.close();
}
