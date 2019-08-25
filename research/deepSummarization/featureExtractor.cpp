#include <iostream>
#include <cv.h>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "../../src/aiSaac.h"
#include "../../tools/videoSources/fileStreamer.h"
#include "../../tools/developerConfig.h"
#include "../../tools/logging.h"
#include "../../src/utils/caffeClassifier.h"
#include "datk.h"

int hBins = 10;
int sBins = 10;

void colorHist(const cv::Mat &rawFrame, cv::Mat &hist) {
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

void addToSceneDb(nlohmann::json &SceneSchema,
    std::vector<cv::Mat> &sceneHists,
    aiSaac::CaffeClassifier &caffeClassifier,
    std::string layer,
    const cv::Mat &currentFrame,
    int frameNumber) {
    std::cout << "Computing Scene Features" << std::endl;

    std::vector<std::pair<std::string, float>> prediction =
        caffeClassifier.Classify(currentFrame, 1);
    SceneSchema["labels"].push_back(prediction[0].first);
    SceneSchema["features"].push_back(caffeClassifier.getLayerOutput(layer));
    SceneSchema["frameNumber"].push_back(frameNumber);

    cv::Mat sceneHist;
    colorHist(currentFrame, sceneHist);
    sceneHists.push_back(sceneHist);
}

void addToFFObjectDB(nlohmann::json &ffObjectSchema,
    std::vector<cv::Mat> &ffObjectHists,
    aiSaac::CaffeClassifier &caffeClassifier,
    std::string layer,
    const cv::Mat &currentFrame,
    int frameNumber) {
    std::cout << "Computing FF Object Features" << std::endl;

    std::vector<std::pair<std::string, float>> prediction =
        caffeClassifier.Classify(currentFrame, 1);
    ffObjectSchema["labels"].push_back(prediction[0].first);
    ffObjectSchema["features"].push_back(caffeClassifier.getLayerOutput(layer));
    ffObjectSchema["frameNumber"].push_back(frameNumber);

    cv::Mat ffObjectHist;
    colorHist(currentFrame, ffObjectHist);
    ffObjectHists.push_back(ffObjectHist);
}

void addToDNObjectDB(nlohmann::json &dnObjectSchema,
    std::vector<cv::Mat> &dnObjectHists,
    aiSaac::ObjectAnalytics &objectAnalytics,
    aiSaac::CaffeClassifier &caffeClassifier,
    std::string layer,
    const cv::Mat &currentFrame,
    int frameNumber) {
    std::cout << "Computing DN Object Features" << std::endl;

    //extra step of localizing objects using DN
    // should store rectangle info too in json
    std::vector<std::pair<std::string, cv::Rect>> localizedLabeledRectangles;
    objectAnalytics.localize(currentFrame, localizedLabeledRectangles);
    std::cout << "found localizations" << std::endl;
    for (int i = 0; i < localizedLabeledRectangles.size(); i++) {
        std::cout << "darknet found: " << localizedLabeledRectangles[i].first << std::endl;
        dnObjectSchema["labels"].push_back(localizedLabeledRectangles[i].first);

        localizedLabeledRectangles[i].second.x = (localizedLabeledRectangles[i].second.x < 0) ? 0 : localizedLabeledRectangles[i].second.x;
        localizedLabeledRectangles[i].second.y = (localizedLabeledRectangles[i].second.y < 0) ? 0 : localizedLabeledRectangles[i].second.y;
        localizedLabeledRectangles[i].second.width = ((localizedLabeledRectangles[i].second.width + localizedLabeledRectangles[i].second.x) > currentFrame.cols) ? currentFrame.cols - localizedLabeledRectangles[i].second.x : localizedLabeledRectangles[i].second.width;
        localizedLabeledRectangles[i].second.height = ((localizedLabeledRectangles[i].second.height + localizedLabeledRectangles[i].second.y) > currentFrame.rows) ? currentFrame.rows - localizedLabeledRectangles[i].second.y : localizedLabeledRectangles[i].second.height;

        // std::cout << "\t image width" << currentFrame.cols << std::endl;
        // std::cout << "\t image height" << currentFrame.rows << std::endl;
        //
        // std::cout << "\t" << localizedLabeledRectangles[i].second.x << std::endl;
        // std::cout << "\t" << localizedLabeledRectangles[i].second.y << std::endl;
        // std::cout << "\t" << localizedLabeledRectangles[i].second.width << std::endl;
        // std::cout << "\t" << localizedLabeledRectangles[i].second.height << std::endl;

        dnObjectSchema["features"].push_back(caffeClassifier.Predict(currentFrame(
            localizedLabeledRectangles[i].second),
            layer));
        dnObjectSchema["rectangle"].push_back({ localizedLabeledRectangles[i].second.x,
            localizedLabeledRectangles[i].second.y,
            localizedLabeledRectangles[i].second.width,
            localizedLabeledRectangles[i].second.height
        });
        dnObjectSchema["frameNumber"].push_back(frameNumber);

        cv::Mat dnObjectHist;
        colorHist(currentFrame(localizedLabeledRectangles[i].second), dnObjectHist);
        dnObjectHists.push_back(dnObjectHist);
    }
}

void addToFaceGenderDB(nlohmann::json &FaceGenderSchema,
    std::vector<cv::Mat> &faceGenderHists,
    aiSaac::FaceAnalytics &faceAnalytics,
    aiSaac::CaffeClassifier &caffeClassifier,
    std::string layer,
    const cv::Mat &currentFrame,
    int frameNumber) {
    std::cout << "Computing Face Gender Features" << std::endl;

    std::vector<cv::Rect> localizedRectangles;
    faceAnalytics.localize(currentFrame, localizedRectangles);
    for (int i = 0; i < localizedRectangles.size(); i++) {
        std::vector<std::pair<std::string, float>> prediction = caffeClassifier.Classify(
            currentFrame(localizedRectangles[i]), 1);

        // should store rectangle info too in json
        FaceGenderSchema["labels"].push_back(prediction[0].first);
        FaceGenderSchema["features"].push_back(caffeClassifier.getLayerOutput(layer));
        FaceGenderSchema["rectangle"].push_back({ localizedRectangles[i].x,
            localizedRectangles[i].y,
            localizedRectangles[i].width,
            localizedRectangles[i].height
        });
        FaceGenderSchema["frameNumber"].push_back(frameNumber);

        cv::Mat faceGenderHist;
        colorHist(currentFrame(localizedRectangles[i]), faceGenderHist);
        faceGenderHists.push_back(faceGenderHist);
    }
}

void addToFaceAgeDB(nlohmann::json &FaceAgeSchema,
    std::vector<cv::Mat> &faceAgeHists,
    aiSaac::FaceAnalytics &faceAnalytics,
    aiSaac::CaffeClassifier &caffeClassifier,
    std::string layer,
    const cv::Mat &currentFrame,
    int frameNumber) {
    std::cout << "Computing Face Age Features" << std::endl;

    std::vector<cv::Rect> localizedRectangles;
    faceAnalytics.localize(currentFrame, localizedRectangles);
    for (int i = 0; i < localizedRectangles.size(); i++) {
        std::vector<std::pair<std::string, float>> prediction = caffeClassifier.Classify(
            currentFrame(localizedRectangles[i]), 1);

        // should store rectangle info too in json
        FaceAgeSchema["labels"].push_back(prediction[0].first);
        FaceAgeSchema["features"].push_back(caffeClassifier.getLayerOutput(layer));
        FaceAgeSchema["rectangle"].push_back({ localizedRectangles[i].x,
            localizedRectangles[i].y,
            localizedRectangles[i].width,
            localizedRectangles[i].height
        });
        FaceAgeSchema["frameNumber"].push_back(frameNumber);

        cv::Mat faceAgeHist;
        colorHist(currentFrame, faceAgeHist);
        faceAgeHists.push_back(faceAgeHist);
    }
}

void addToFaceRecDB(nlohmann::json &FaceRecSchema,
    std::vector<cv::Mat> &faceRecHists,
    aiSaac::FaceAnalytics &faceAnalytics,
    aiSaac::CaffeClassifier &caffeClassifier,
    std::string layer,
    const cv::Mat &currentFrame,
    int frameNumber) {
    std::cout << "Computing Face Rec Features" << std::endl;

    std::vector<cv::Rect> localizedRectangles;
    faceAnalytics.localize(currentFrame, localizedRectangles);
    for (int i = 0; i < localizedRectangles.size(); i++) {
        std::vector<std::pair<std::string, float>> prediction = caffeClassifier.Classify(
            currentFrame(localizedRectangles[i]), 1);

        // should store rectangle info too in json
        FaceRecSchema["features"].push_back(caffeClassifier.getLayerOutput(layer));
        FaceRecSchema["rectangle"].push_back({ localizedRectangles[i].x,
            localizedRectangles[i].y,
            localizedRectangles[i].width,
            localizedRectangles[i].height
        });
        FaceRecSchema["labels"].push_back("Unknown");
        FaceRecSchema["frameNumber"].push_back(frameNumber);

        cv::Mat faceRecHist;
        colorHist(currentFrame(localizedRectangles[i]), faceRecHist);
        faceRecHists.push_back(faceRecHist);
    }
}

void computeHistKernel(std::vector<cv::Mat> &hists, std::vector<std::vector<float>> &histKernel) {
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

void computeKernel(std::vector<std::vector<double>> featureVectors,
    std::vector<cv::Mat> hists,
    nlohmann::json &Schema) {
    std::cout << "inside computeKernel()" << std::endl;
    std::cout << "Size of featureVectors: " << featureVectors.size() << std::endl;
    std::cout << "Size of hists: " << hists.size() << std::endl;

    std::vector<std::vector<float>> histKernel;
    computeHistKernel(hists, histKernel);
    std::cout << "Size of histKernel: "
        << histKernel.size()
        << "x"
        << histKernel[0].size()
        << std::endl;

    double val = 0;
    double min = std::numeric_limits<double>::max();
    double max = std::numeric_limits<double>::min();
    for (int i = 0; i < featureVectors.size(); i++) {
        std::vector<float> currentVector;
        for (int j = 0; j < featureVectors.size(); j++) {
            val = datk::innerProduct(featureVectors[i], featureVectors[j]);
            val += histKernel[i][j];
            max = (val > max) ? val : max;
            min = (val < min) ? val : min;
            currentVector.push_back(val);
        }
        Schema["kernel"].push_back(currentVector);
    }
    // Normalize kernel matrix:
    double range = max - min;
    for (int i = 0; i < Schema["kernel"].size(); i++) {
        for (int j = 0; j < Schema["kernel"].size(); j++) {
            Schema["kernel"][i][j] = ((double) Schema["kernel"][i][j] - min) / range;
        }
    }
}

void saveSchema(std::string schemaFilePath, nlohmann::json &Schema) {
    std::cout << "saving file" << schemaFilePath << std::endl;
    std::ofstream configFile;
    configFile.open(schemaFilePath);
    configFile << Schema.dump(4);
    configFile.close();
}

int main (int argc, char *argv[]) {
    if (argc < 2) {
        std::cout << "Usage: ./featureExtractor <input_video_file> <store_feature_db_dir> <procFPS> <scene_model_dir> <scene_layer> <ff_object_model_dir> <ff_object_layer> <dn_object_model_dir (after dn)> <dn_object_layer> <face_gender_model_dir> <face_gender_layer> <face_age_model_dir> <face_age_layer> <face_rec_model_dir> <face_rec_layer>" << std::endl;
        return 0;
    }
    setDeveloperVariables(developerName);

    nlohmann::json SceneSchema;
    nlohmann::json FFObjectSchema;
    nlohmann::json DNObjectSchema;
    nlohmann::json FaceGenderSchema;
    nlohmann::json FaceAgeSchema;
    nlohmann::json FaceRecSchema;

    std::string videoFilePath = argv[1];
    std::string featureDbPath = argv[2];

    std::cout << "Initializing scene classifier" << std::endl;
    std::string sceneModelDir = argv[4];
    std::string sceneLayer = argv[5];
    aiSaac::CaffeClassifier sceneClassifier(
        sceneModelDir + "/deploy.prototxt",
        sceneModelDir + "/deploy.caffemodel",
        sceneModelDir + "/mean.binaryproto",
        sceneModelDir + "/labels.txt"
    );

    std::cout << "Initializing ff object classifier" << std::endl;
    std::string ffObjectModelDir = argv[6];
    std::string ffObjectLayer = argv[7];
    aiSaac::CaffeClassifier ffObjectClassifier(
        ffObjectModelDir + "/deploy.prototxt",
        ffObjectModelDir + "/deploy.caffemodel",
        ffObjectModelDir + "/mean.binaryproto",
        ffObjectModelDir + "/labels.txt"
    );

    std::cout << "Initializing aiSaacSettings" << std::endl;
    aiSaac::AiSaacSettings aiSaacSettings(storagePath + "/config1.json");

    std::cout << "Initializing object analytics" << std::endl;
    aiSaac::ObjectAnalytics objectAnalytics(aiSaacSettings);
    std::cout << "Initializing dn object classifier" << std::endl;
    std::string dnObjectModelDir = argv[8];
    std::string dnObjectLayer = argv[9];
    aiSaac::CaffeClassifier dnObjectClassifier(
        dnObjectModelDir + "/deploy.prototxt",
        dnObjectModelDir + "/deploy.caffemodel",
        dnObjectModelDir + "/mean.binaryproto",
        dnObjectModelDir + "/labels.txt"
    );

    std::cout << "Initializing faceAnalytics" << std::endl;
    aiSaac::FaceAnalytics faceAnalytics(aiSaacSettings);
    std::cout << "Initializing gender classifier" << std::endl;
    std::string faceGenderModelDir = argv[10];
    std::string faceGenderLayer = argv[11];
    aiSaac::CaffeClassifier faceGenderClassifier(
        faceGenderModelDir + "/deploy.prototxt",
        faceGenderModelDir + "/deploy.caffemodel",
        faceGenderModelDir + "/mean.binaryproto",
        faceGenderModelDir + "/labels.txt"
    );

    std::cout << "Initializing age classifier" << std::endl;
    std::string faceAgeModelDir = argv[12];
    std::string faceAgeLayer = argv[13];
    aiSaac::CaffeClassifier faceAgeClassifier(
        faceAgeModelDir + "/deploy.prototxt",
        faceAgeModelDir + "/deploy.caffemodel",
        faceAgeModelDir + "/mean.binaryproto",
        faceAgeModelDir + "/labels.txt"
    );

    std::cout << "Initializing face rec classifier" << std::endl;
    std::string faceRecModelDir = argv[14];
    std::string faceRecLayer = argv[15];
    aiSaac::CaffeClassifier faceRecClassifier(
        faceRecModelDir + "/deploy.prototxt",
        faceRecModelDir + "/deploy.caffemodel"
    );

    FileStreamer fileStreamer(videoFilePath);

    std::cout << "Waiting for video to stream ..." << std::endl;
    while (!fileStreamer.isStreaming()) {
        std::cout << "." << std::endl;
    }
    std::cout << " Video ready to stream" << std::endl;

    int procFPS = (std::atoi(argv[3]) != 0) ? std::atoi(argv[3]) : fileStreamer.getFileFPS();

    cv::Mat currentFrame;
    int frameNumber = 0;
    std::vector<cv::Mat> sceneHists;
    std::vector<cv::Mat> ffObjectHists;
    std::vector<cv::Mat> dnObjectHists;
    std::vector<cv::Mat> faceGenderHists;
    std::vector<cv::Mat> faceAgeHists;
    std::vector<cv::Mat> faceRecHists;

    while (true) {
        currentFrame = fileStreamer.getFrame();

        if (currentFrame.data == NULL ||
            currentFrame.empty() ||
            !fileStreamer.isStreaming()) {
            std::cout << "Stopping file reading." << std::endl;
            break;
        }

        if (frameNumber % fileStreamer.getFileFPS() / procFPS == 0) {
            addToSceneDb(SceneSchema,
                sceneHists,
                sceneClassifier,
                sceneLayer,
                currentFrame,
                frameNumber);

            addToFFObjectDB(FFObjectSchema,
                ffObjectHists,
                ffObjectClassifier,
                ffObjectLayer,
                currentFrame,
                frameNumber);

            addToDNObjectDB(DNObjectSchema,
                dnObjectHists,
                objectAnalytics,
                dnObjectClassifier,
                dnObjectLayer,
                currentFrame,
                frameNumber);

            addToFaceGenderDB(FaceGenderSchema,
                faceGenderHists,
                faceAnalytics,
                faceGenderClassifier,
                faceGenderLayer,
                currentFrame,
                frameNumber);

            addToFaceAgeDB(FaceAgeSchema,
                faceAgeHists,
                faceAnalytics,
                faceAgeClassifier,
                faceAgeLayer,
                currentFrame,
                frameNumber);

            addToFaceRecDB(FaceRecSchema,
                faceRecHists,
                faceAnalytics,
                faceRecClassifier,
                faceRecLayer,
                currentFrame,
                frameNumber);
        }
        frameNumber++;
    }

    // Need to compute and store kernel here.
    std::cout << "Done going over video. Computing kernel now: " << std::endl;
    std::cout << "scene kernel" << std::endl;
    std::cout << "Size of scene hist: " << sceneHists.size() << std::endl;
    std::vector<std::vector<double>> sceneFeatureVectors = SceneSchema["features"];
    computeKernel(sceneFeatureVectors, sceneHists, SceneSchema);

    std::cout << "ff object kernel" << std::endl;
    std::cout << "Size of ff object hist: " << ffObjectHists.size() << std::endl;
    std::vector<std::vector<double>> ffObjectFeatureVectors = FFObjectSchema["features"];
    computeKernel(ffObjectFeatureVectors, ffObjectHists, FFObjectSchema);

    if (DNObjectSchema["features"].size()) {
        std::cout << "dn object kernel" << std::endl;
        std::cout << "Size of dn object hist: " << dnObjectHists.size() << std::endl;
        std::vector<std::vector<double>> dnObjectFeatureVectors = DNObjectSchema["features"];
        computeKernel(dnObjectFeatureVectors, dnObjectHists, DNObjectSchema);
    }

    if (FaceGenderSchema["features"].size()) {
        std::cout << "face gender kernel" << std::endl;
        std::cout << "Size of face gender hist: " << faceGenderHists.size() << std::endl;
        std::vector<std::vector<double>> faceGenderFeatureVectors = FaceGenderSchema["features"];
        computeKernel(faceGenderFeatureVectors, faceGenderHists, FaceGenderSchema);
    }

    if (FaceAgeSchema["features"].size()) {
        std::cout << "face age kernel" << std::endl;
        std::cout << "Size of face age hist: " << faceAgeHists.size() << std::endl;
        std::vector<std::vector<double>> faceAgeFeatureVectors = FaceAgeSchema["features"];
        computeKernel(faceAgeFeatureVectors, faceAgeHists, FaceAgeSchema);
    }

    if (FaceRecSchema["features"].size()) {
        std::cout << "face rec kernel" << std::endl;
        std::cout << "Size of face rec hist: " << faceRecHists.size() << std::endl;
        std::vector<std::vector<double>> faceRecFeatureVectors = FaceRecSchema["features"];
        computeKernel(faceRecFeatureVectors, faceRecHists, FaceRecSchema);
    }

    std::cout << "Done computing kernel. Saving schema now: " << std::endl;
    saveSchema(featureDbPath + "/SceneSchema.json", SceneSchema);
    std::cout << "Successfully saved scene json" << std::endl;
    saveSchema(featureDbPath + "/FFObjectSchema.json", FFObjectSchema);
    std::cout << "Successfully saved ff object json" << std::endl;
    saveSchema(featureDbPath + "/DNObjectSchema.json", DNObjectSchema);
    std::cout << "Successfully saved dn object json" << std::endl;
    saveSchema(featureDbPath + "/FaceGenderSchema.json", FaceGenderSchema);
    std::cout << "Successfully saved face gender json" << std::endl;
    saveSchema(featureDbPath + "/FaceAgeSchema.json", FaceAgeSchema);
    std::cout << "Successfully saved face age json" << std::endl;
    saveSchema(featureDbPath + "/FaceRecSchema.json", FaceRecSchema);
    std::cout << "Successfully saved face vgg json" << std::endl;
}
