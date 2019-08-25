#include "summarization.h"

aiSaac::Summarization::Summarization(aiSaac::AiSaacSettings &aiSaacSettings,
    std::string videoFilePath):
    aiSaacSettings(aiSaacSettings){
    std::cout << "Summarization Analytics" << std::endl;
    this->videoFilePath = videoFilePath;
    this->loadAlgorithmType();
}

aiSaac::Summarization::Summarization(aiSaac::AiSaacSettings &aiSaacSettings,
    std::string videoFilePath,
    aiSaac::CaffeClassifier &sceneClassifier,
    aiSaac::CaffeClassifier &ffObjectClassifier,
    aiSaac::CaffeClassifier &dnObjectClassifier,
    aiSaac::CaffeClassifier &faceGenderClassifier,
    aiSaac::CaffeClassifier &faceAgeClassifier,
    aiSaac::CaffeClassifier &faceRecClassifier,
    aiSaac::ObjectAnalytics &objectAnalytics,
    aiSaac::FaceAnalytics &faceAnalytics):
    aiSaacSettings(aiSaacSettings) {
    std::cout << "Summarization Analytics" << std::endl;
    this->videoFilePath = videoFilePath;
    this->sceneClassifier = &sceneClassifier;
    this->ffObjectClassifier = &ffObjectClassifier;
    this->dnObjectClassifier = &dnObjectClassifier;
    this->faceGenderClassifier = &faceGenderClassifier;
    this->faceAgeClassifier = &faceAgeClassifier;
    this->faceRecClassifier = &faceRecClassifier;

    this->objectAnalytics = &objectAnalytics;
    this->faceAnalytics = &faceAnalytics;
}

aiSaac::Summarization::~Summarization() {
    if (this->loadedAlgos) {
        delete this->sceneClassifier;
        delete this->ffObjectClassifier;
        delete this->dnObjectClassifier;
        delete this->faceGenderClassifier;
        delete this->faceAgeClassifier;
        delete this->faceRecClassifier;
    }
}

void aiSaac::Summarization::computeColorHist(cv::Mat &rawFrame, cv::Mat &hist) {
    int hBins = 10;  // hyper parameter to be put into aiSaacSettings
    int sBins = 10;  // hyper parameter to be put into aiSaacSettings
    int histSize[] = { hBins, sBins };
    float hRanges[] = { 0, 180 };
    float sRanges[] = { 0, 256 };
    const float* ranges[] = { hRanges, sRanges };
    int channels[] = { 0, 1 };
    // cv::Mat frame = rawFrame.clone();
    cv::calcHist(&rawFrame,
        1,
        channels,
        cv::Mat(),
        hist,
        2,
        histSize,
        ranges,
        true,
        false );
    cv::normalize(hist, hist, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());
}

// input: featurizationFolder, choose featurization models, videoFile
void aiSaac::Summarization::featurizeVideo (int procFPS) {
    int frameNumber = 0;
    cv::VideoCapture capture(this->videoFilePath);
    cv::Mat currentFrame;
    procFPS = (procFPS != -1) ? procFPS : capture.get(CV_CAP_PROP_FPS);
    this->sceneCollection["procFPS"] = procFPS;
    this->ffObjectCollection["procFPS"] = procFPS;
    this->dnObjectCollection["procFPS"] = procFPS;
    this->faceGenderCollection["procFPS"] = procFPS;
    this->faceAgeCollection["procFPS"] = procFPS;
    this->faceRecCollection["procFPS"] = procFPS;
    while (capture.read(currentFrame)) {
        if (frameNumber % (int) capture.get(CV_CAP_PROP_FPS) / procFPS == 0) {

            std::cout << "Computing Scene Features" << std::endl;

            std::vector<std::pair<std::string, float>> scenePrediction =
                this->sceneClassifier->Classify(currentFrame, 1);
            this->sceneCollection["labels"].push_back(scenePrediction[0].first);
            this->sceneCollection["features"].push_back(this->sceneClassifier->getLayerOutput(
                this->aiSaacSettings.getSummarizationSceneFeatureExtractionLayer()));
            this->sceneCollection["frameNumber"].push_back(frameNumber);
            cv::Mat sceneHist;
            this->computeColorHist(currentFrame, sceneHist);
            this->sceneHists.push_back(sceneHist);

            std::cout << "Computing FF Object Features" << std::endl;

            std::vector<std::pair<std::string, float>> ffObjectPrediction =
                this->ffObjectClassifier->Classify(currentFrame, 1);
            this->ffObjectCollection["labels"].push_back(ffObjectPrediction[0].first);
            this->ffObjectCollection["features"].push_back(this->ffObjectClassifier->getLayerOutput(
                this->aiSaacSettings.getSummarizationFfObjectFeatureExtractionLayer()));
            this->ffObjectCollection["frameNumber"].push_back(frameNumber);

            cv::Mat ffObjectHist;
            computeColorHist(currentFrame, ffObjectHist);
            this->ffObjectHists.push_back(ffObjectHist);

            std::cout << "Computing DN Object Features" << std::endl;

            std::vector<std::pair<std::string, cv::Rect>> localizedLabeledRectangles;
            this->objectAnalytics->localize(currentFrame, localizedLabeledRectangles);
            std::cout << "found localizations" << std::endl;
            for (int i = 0; i < localizedLabeledRectangles.size(); i++) {
                std::cout << "darknet found: " << localizedLabeledRectangles[i].first << std::endl;
                this->dnObjectCollection["labels"].push_back(localizedLabeledRectangles[i].first);
                this->dnObjectCollection["features"].push_back(
                    this->dnObjectClassifier->Predict(currentFrame(
                        localizedLabeledRectangles[i].second),
                        this->aiSaacSettings.getSummarizationDnObjectFeatureExtractionLayer()));
                this->dnObjectCollection["rectangle"].push_back({ localizedLabeledRectangles[i].second.x,
                    localizedLabeledRectangles[i].second.y,
                    localizedLabeledRectangles[i].second.width,
                    localizedLabeledRectangles[i].second.height
                });
                this->dnObjectCollection["frameNumber"].push_back(frameNumber);

                cv::Mat dnObjectHist;
                cv::Mat localizedFrame = currentFrame(localizedLabeledRectangles[i].second);
                computeColorHist(localizedFrame, dnObjectHist);
                this->dnObjectHists.push_back(dnObjectHist);
            }

            std::cout << "Computing Face Features" << std::endl;

            std::vector<cv::Rect> localizedRectangles;
            this->faceAnalytics->localize(currentFrame, localizedRectangles);
            for (int i = 0; i < localizedRectangles.size(); i++) {
                std::vector<std::pair<std::string, float>> faceGenderPrediction =
                    this->faceGenderClassifier->Classify(currentFrame(localizedRectangles[i]), 1);

                // should store rectangle info too in json
                this->faceGenderCollection["labels"].push_back(faceGenderPrediction[0].first);
                this->faceGenderCollection["features"].push_back(
                    this->faceGenderClassifier->getLayerOutput(
                        this->aiSaacSettings.getSummarizationFaceGenderFeatureExtractionLayer()));
                this->faceGenderCollection["rectangle"].push_back({
                    localizedRectangles[i].x,
                    localizedRectangles[i].y,
                    localizedRectangles[i].width,
                    localizedRectangles[i].height
                });
                this->faceGenderCollection["frameNumber"].push_back(frameNumber);

                cv::Mat faceGenderHist;
                cv::Mat localizedFrame = currentFrame(localizedRectangles[i]);
                computeColorHist(localizedFrame, faceGenderHist);
                this->faceGenderHists.push_back(faceGenderHist);

                std::vector<std::pair<std::string, float>> faceAgePrediction =
                    this->faceAgeClassifier->Classify(currentFrame(localizedRectangles[i]), 1);

                // should store rectangle info too in json
                this->faceAgeCollection["labels"].push_back(faceAgePrediction[0].first);
                this->faceAgeCollection["features"].push_back(
                    this->faceAgeClassifier->getLayerOutput(
                        this->aiSaacSettings.getSummarizationFaceAgeFeatureExtractionLayer()));
                this->faceAgeCollection["rectangle"].push_back({ localizedRectangles[i].x,
                    localizedRectangles[i].y,
                    localizedRectangles[i].width,
                    localizedRectangles[i].height
                });
                this->faceAgeCollection["frameNumber"].push_back(frameNumber);

                cv::Mat faceAgeHist;
                computeColorHist(localizedFrame, faceAgeHist);
                this->faceAgeHists.push_back(faceAgeHist);

                std::vector<std::pair<std::string, float>> faceRecPrediction =
                    this->faceRecClassifier->Classify(currentFrame(localizedRectangles[i]), 1);

                // should store rectangle info too in json
                this->faceRecCollection["features"].push_back(
                    this->faceRecClassifier->getLayerOutput(
                        this->aiSaacSettings.getSummarizationFaceRecFeatureExtractionLayer()));
                this->faceRecCollection["rectangle"].push_back({
                    localizedRectangles[i].x,
                    localizedRectangles[i].y,
                    localizedRectangles[i].width,
                    localizedRectangles[i].height
                });
                this->faceRecCollection["labels"].push_back("Unknown");
                this->faceRecCollection["frameNumber"].push_back(frameNumber);

                cv::Mat faceRecHist;
                computeColorHist(localizedFrame, faceRecHist);
                faceRecHists.push_back(faceRecHist);
            }
        }
        frameNumber++;
    }

    std::cout << "Done going over video. Computing kernel now: " << std::endl;
    std::cout << "scene kernel" << std::endl;
    std::cout << "Size of scene hist: " << this->sceneHists.size() << std::endl;
    std::vector<std::vector<double>> sceneFeatureVectors = this->sceneCollection["features"];
    this->computeKernel(sceneFeatureVectors, this->sceneHists, this->sceneCollection);

    std::cout << "ff object kernel" << std::endl;
    std::cout << "Size of ff object hist: " << this->ffObjectHists.size() << std::endl;
    std::vector<std::vector<double>> ffObjectFeatureVectors = this->ffObjectCollection["features"];
    this->computeKernel(ffObjectFeatureVectors, this->ffObjectHists, this->ffObjectCollection);

    if (this->dnObjectCollection["features"].size()) {
        std::cout << "dn object kernel" << std::endl;
        std::cout << "Size of dn object hist: " << this->dnObjectHists.size() << std::endl;
        std::vector<std::vector<double>> dnObjectFeatureVectors =
            this->dnObjectCollection["features"];
        this->computeKernel(dnObjectFeatureVectors, this->dnObjectHists, this->dnObjectCollection);
    }

    if (this->faceGenderCollection["features"].size()) {
        std::cout << "face gender kernel" << std::endl;
        std::cout << "Size of face gender hist: " << this->faceGenderHists.size() << std::endl;
        std::vector<std::vector<double>> faceGenderFeatureVectors =
            this->faceGenderCollection["features"];
        this->computeKernel(faceGenderFeatureVectors,
            this->faceGenderHists,
            this->faceGenderCollection);
    }

    if (this->faceAgeCollection["features"].size()) {
        std::cout << "face age kernel" << std::endl;
        std::cout << "Size of face age hist: " << this->faceAgeHists.size() << std::endl;
        std::vector<std::vector<double>> faceAgeFeatureVectors =
            this->faceAgeCollection["features"];
        this->computeKernel(faceAgeFeatureVectors, this->faceAgeHists, this->faceAgeCollection);
    }

    if (this->faceRecCollection["features"].size()) {
        std::cout << "face rec kernel" << std::endl;
        std::cout << "Size of face rec hist: " << this->faceRecHists.size() << std::endl;
        std::vector<std::vector<double>> faceRecFeatureVectors =
            this->faceRecCollection["features"];
        this->computeKernel(faceRecFeatureVectors, this->faceRecHists, this->faceRecCollection);
    }

    std::cout << "Done computing kernel. Saving collections now: " << std::endl;
    this->saveCollection(this->aiSaacSettings.getSummarizationCollectionDir() + "scene",
        this->sceneCollection,
        this->sceneHists);
    std::cout << "Successfully saved scene json" << std::endl;
    this->saveCollection(this->aiSaacSettings.getSummarizationCollectionDir() + "ff-object",
        this->ffObjectCollection,
        this->ffObjectHists);
    std::cout << "Successfully saved ff object json" << std::endl;
    this->saveCollection(this->aiSaacSettings.getSummarizationCollectionDir() + "dn-object",
        this->dnObjectCollection,
        this->dnObjectHists);
    std::cout << "Successfully saved dn object json" << std::endl;
    this->saveCollection(this->aiSaacSettings.getSummarizationCollectionDir() + "face-gender",
        this->faceGenderCollection,
        this->faceGenderHists);
    std::cout << "Successfully saved face gender json" << std::endl;
    this->saveCollection(this->aiSaacSettings.getSummarizationCollectionDir() + "face-age",
        this->faceAgeCollection,
        this->faceAgeHists);
    std::cout << "Successfully saved face age json" << std::endl;
    this->saveCollection(this->aiSaacSettings.getSummarizationCollectionDir() + "face-rec",
        this->faceRecCollection,
        this->faceRecHists);
    std::cout << "Successfully saved face rec json" << std::endl;
}

void aiSaac::Summarization::computeKernel(std::vector<std::vector<double>> featureVectors,
    std::vector<cv::Mat> hists,
    nlohmann::json &collection) {
    std::cout << "inside computeKernel()" << std::endl;
    std::cout << "Size of featureVectors: " << featureVectors.size() << std::endl;
    std::cout << "Size of hists: " << hists.size() << std::endl;

    std::vector<std::vector<float>> histKernel;
    this->computeHistKernel(hists, histKernel);
    std::cout << "Size of histKernel: "
        << histKernel.size()
        << "x"
        << histKernel[0].size()
        << std::endl;

    double val = 0;
    double min = std::numeric_limits<double>::max();
    double max = std::numeric_limits<double>::min();
    std::vector<std::vector<float>> deepFeatureKernel;
    for (int i = 0; i < featureVectors.size(); i++) {
        std::vector<float> currentVector;
        for (int j = 0; j < featureVectors.size(); j++) {
            val = datk::innerProduct(featureVectors[i], featureVectors[j]);
            max = (val > max) ? val : max;
            min = (val < min) ? val : min;
            currentVector.push_back(val);
        }
        deepFeatureKernel.push_back(currentVector);
    }
    // Normalize kernel matrix:
    double range = max - min;
    double aggValue = 0;
    double aggMin = std::numeric_limits<double>::max();
    double aggMax = std::numeric_limits<double>::min();
    for (int i = 0; i < deepFeatureKernel.size(); i++) {
        std::vector<float> currentVector;
        for (int j = 0; j < deepFeatureKernel.size(); j++) {
            aggValue = ((double) deepFeatureKernel[i][j] - min) / range;
            aggValue += histKernel[i][j];
            aggMax = (aggValue > aggMax) ? aggValue : aggMax;
            aggMin = (aggValue < aggMin) ? aggValue : aggMin;
            currentVector.push_back(aggValue);
        }
        collection["kernel"].push_back(currentVector);
    }

    double aggRange = aggMax - aggMin;
    for (int i = 0; i < collection["kernel"].size(); i++) {
        for (int j = 0; j < collection["kernel"].size(); j++) {
            collection["kernel"][i][j] = ((double) collection["kernel"][i][j] - aggMin) / aggRange;
        }
    }
}

void aiSaac::Summarization::computeHistKernel(std::vector<cv::Mat> &hists,
    std::vector<std::vector<float>> &histKernel) {
    std::cout << "Computing histogram kernel" << std::endl;
    float max = 0;
    for (int i = 0; i < hists.size(); i++) {
        std::vector<float> currVector;
        for (int j = 0; j < hists.size(); j++) {
            float val = cv::compareHist(hists[i], hists[j], 0);  // compare method is 0
            max = (val > max) ? val : max;
            currVector.push_back(val);
        }
        histKernel.push_back(currVector);
    }
    for (int i = 0; i < histKernel.size(); i++) {
        for (int j = 0; j < histKernel[i].size(); j++) {
            histKernel[i][j] = histKernel[i][j] / max;
            histKernel[i][j] = (histKernel[i][j] + 1) / 2;
        }
    }
}

void aiSaac::Summarization::loadCollection(std::string collectionFilePath,
    nlohmann::json &collection) {
    std::ifstream configFile;
    configFile.open(collectionFilePath);
    configFile >> collection;
}

void aiSaac::Summarization::saveCollection(std::string collectionDirPath,
    nlohmann::json &collection,
    std::vector<cv::Mat> &hists) {
    std::cout << "saving file" << collectionDirPath + "/collection.json" << std::endl;
    std::ofstream configFile;
    configFile.open(collectionDirPath + "/collection.json");
    configFile << collection.dump(4);
    configFile.close();

    std::string histCollectionDir = collectionDirPath + "/hist-collection";
    std::string command = "mkdir -p " + histCollectionDir;
    system(command.c_str());

    for (int i = 0; i < collection["frameNumber"].size(); i++) {
        std::string frameNumber = collection["frameNumber"][i];
        cv::imwrite(histCollectionDir +
            "/" +
            std::to_string(i) +
            "-" +
            frameNumber +
            ".jpg",
             hists[i]);
    }
}

void aiSaac::Summarization::loadAlgorithmType() {
    this->loadedAlgos = true;
    if (!this->aiSaacSettings.getSummarizationScenePrototxtPath().empty() &&
        !this->aiSaacSettings.getSummarizationSceneCaffemodelPath().empty() &&
        !this->aiSaacSettings.getSummarizationSceneMeanFilePath().empty() &&
        !this->aiSaacSettings.getSummarizationSceneLabelFilePath().empty()) {
        this->sceneClassifier = new aiSaac::CaffeClassifier(
            this->aiSaacSettings.getSummarizationScenePrototxtPath(),
            this->aiSaacSettings.getSummarizationSceneCaffemodelPath(),
            this->aiSaacSettings.getSummarizationSceneMeanFilePath(),
            this->aiSaacSettings.getSummarizationSceneLabelFilePath());
    }

    if (!this->aiSaacSettings.getSummarizationFfObjectPrototxtPath().empty() &&
        !this->aiSaacSettings.getSummarizationFfObjectCaffemodelPath().empty() &&
        !this->aiSaacSettings.getSummarizationFfObjectMeanFilePath().empty() &&
        !this->aiSaacSettings.getSummarizationFfObjectLabelFilePath().empty()) {
        this->ffObjectClassifier = new aiSaac::CaffeClassifier(
            this->aiSaacSettings.getSummarizationFfObjectPrototxtPath(),
            this->aiSaacSettings.getSummarizationFfObjectCaffemodelPath(),
            this->aiSaacSettings.getSummarizationFfObjectMeanFilePath(),
            this->aiSaacSettings.getSummarizationFfObjectLabelFilePath());
    }

    this->objectAnalytics = new aiSaac::ObjectAnalytics(this->aiSaacSettings);

    if (!this->aiSaacSettings.getSummarizationDnObjectPrototxtPath().empty() &&
        !this->aiSaacSettings.getSummarizationDnObjectCaffemodelPath().empty() &&
        !this->aiSaacSettings.getSummarizationDnObjectMeanFilePath().empty() &&
        !this->aiSaacSettings.getSummarizationDnObjectLabelFilePath().empty()) {
        this->dnObjectClassifier = new aiSaac::CaffeClassifier(
            this->aiSaacSettings.getSummarizationDnObjectPrototxtPath(),
            this->aiSaacSettings.getSummarizationDnObjectCaffemodelPath(),
            this->aiSaacSettings.getSummarizationDnObjectMeanFilePath(),
            this->aiSaacSettings.getSummarizationDnObjectLabelFilePath());
    }


    this->faceAnalytics = new aiSaac::FaceAnalytics(this->aiSaacSettings);

    if (!this->aiSaacSettings.getSummarizationFaceGenderPrototxtPath().empty() &&
        !this->aiSaacSettings.getSummarizationFaceGenderCaffemodelPath().empty() &&
        !this->aiSaacSettings.getSummarizationFaceGenderMeanFilePath().empty() &&
        !this->aiSaacSettings.getSummarizationFaceGenderLabelFilePath().empty()) {
        this->faceGenderClassifier = new aiSaac::CaffeClassifier(
            this->aiSaacSettings.getSummarizationFaceGenderPrototxtPath(),
            this->aiSaacSettings.getSummarizationFaceGenderCaffemodelPath(),
            this->aiSaacSettings.getSummarizationFaceGenderMeanFilePath(),
            this->aiSaacSettings.getSummarizationFaceGenderLabelFilePath());
    }

    if (!this->aiSaacSettings.getSummarizationFaceAgePrototxtPath().empty() &&
        !this->aiSaacSettings.getSummarizationFaceAgeCaffemodelPath().empty() &&
        !this->aiSaacSettings.getSummarizationFaceAgeMeanFilePath().empty() &&
        !this->aiSaacSettings.getSummarizationFaceAgeLabelFilePath().empty()) {
        this->faceAgeClassifier = new aiSaac::CaffeClassifier(
            this->aiSaacSettings.getSummarizationFaceAgePrototxtPath(),
            this->aiSaacSettings.getSummarizationFaceAgeCaffemodelPath(),
            this->aiSaacSettings.getSummarizationFaceAgeMeanFilePath(),
            this->aiSaacSettings.getSummarizationFaceAgeLabelFilePath());
    }

    if (!this->aiSaacSettings.getSummarizationFaceRecPrototxtPath().empty() &&
        !this->aiSaacSettings.getSummarizationFaceRecCaffemodelPath().empty()) {
        this->faceRecClassifier = new aiSaac::CaffeClassifier(
            this->aiSaacSettings.getSummarizationFaceRecPrototxtPath(),
            this->aiSaacSettings.getSummarizationFaceRecCaffemodelPath());
    }
}
