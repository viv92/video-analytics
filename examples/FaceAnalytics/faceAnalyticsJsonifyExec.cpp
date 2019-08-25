#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <dirent.h>
#include <vector>
#include <signal.h>
#include <fstream>
#include "datk.h"
#include "../../src/aiSaacSettings.h"
#include "../../tools/videoSources/fileStreamer.h"

#ifdef USE_MONGO
#include "../../tools/mongoLink/mongoLink.h"
#else
#include "../../tools/mongoLink/serialLink.h"
#endif //USE_MONGO

nlohmann::json blobFace;
nlohmann::json FaceAnalyticsBlobs;
int frameNumber = 0;
int procFPS = 3;
volatile sig_atomic_t flag = 1;

void handler(int sig){ // can be called asynchronously
    flag = 0; // set flag
}

int main(int argc, char *argv[]) {
    // codeReview(Anurag): argument parser needed
    try {
        if (argc < 6) {
            std::cout << "usage: ./faceRecognitionExampleJsonify [filePath]  "
                << "[aiSaacSettingsPath] [caSettingsFilePath] [resultTableID] "
                << "[resultTableFeature]" << std::endl;
            return -1;
        }
        std::string file = argv[1];
        std::string aiSaacSettingsPath = argv[2];
        std::string caSettingsPath = argv[3];
        std::string resultTableId = argv[4];
        std::string resultTableFeature = argv[5];
        signal(SIGINT, handler);

        aiSaac::CASettings *caSettings = new aiSaac::CASettings(caSettingsPath);
        aiSaac::AiSaacSettings aiSaacSettings = aiSaac::AiSaacSettings(aiSaacSettingsPath);
        aiSaac::FaceAnalytics faceAnalytics = aiSaac::FaceAnalytics(aiSaacSettings);
        aiSaac::AgeRecognition ageRecognition = aiSaac::AgeRecognition(aiSaacSettings);
        aiSaac::GenderRecognition genderRecognition = aiSaac::GenderRecognition(aiSaacSettings);
        caSettings->setIsTrained(true);
        aiSaac::CustomAnalytics ca1 = aiSaac::CustomAnalytics(caSettings);
        // codeReview(Anurag): no need to train if DATK Model is ready
        ca1.train();
        // ca1.loadDatkModel();
        bool isCropped;
        cv::Mat image = cv::imread(file, CV_LOAD_IMAGE_COLOR);

        if( !image.data ) {
            debugMsg("Unable to open file for reading");
            return -1;
        }

        mongoLink mongoObject = mongoLink(1);

        std::cout << "AISAAC_LOG: Detecting faces" << std::endl;
        std::vector<aiSaac::FaceBlob> blobContainerSingle;
        aiSaac::FaceBlob imageFace;
        imageFace.ID = 1;
        imageFace.firstFrameNumber = 1;
        imageFace.lastFrameNumber = 1;
        imageFace.frameCount = 1;
        imageFace.label = ca1.detect(image);
        cv::Rect imageRect(0, 0, image.cols, image.rows);
        imageFace.firstRectangle = imageRect;
        imageFace.lastRectangle = imageRect;
        blobContainerSingle.push_back(imageFace);

        std::cout << "AISAAC_LOG: Detecting age" << std::endl;
        std::string age = ageRecognition.runAlgo(image);

        std::cout << "AISAAC_LOG: Detecting gender" << std::endl;
        std::string gender = genderRecognition.runAlgo(image);
        std::cout << "AISAAC_RESULT: " << imageFace.label
            << ", " << age << ", " << gender << std::endl;
        mongoObject.faceAnalyticsMONGO(blobContainerSingle, 1, resultTableId, resultTableFeature);
        std::cout << "AISAAC_LOG: Your image has been processed." << std::endl;

        return 0;
    } catch (...) {
        return -1;
    }
}
